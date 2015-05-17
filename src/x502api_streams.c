#include "x502api_private.h"
#include "x502_fpga_regs.h"
#include "l502_bf_cmd_defs.h"

#include <string.h>
#include <stdlib.h>


/* разрешение/запрещение потоков ввода вывода в соответствии с полем hnd->streams */
static int32_t f_set_streams(t_x502_hnd hnd, uint32_t streams) {
    int32_t err = X502_ERR_OK;
    if (hnd->mode == X502_MODE_FPGA)  {
        err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOARITH_IN_STREAM_ENABLE,
                                        (streams & X502_STREAM_ADC ? 0x01 : 0) |
                                        (streams & X502_STREAM_DIN ? 0x02 : 0));
    }

    if (err == X502_ERR_OK) {
        hnd->streams = streams;
    }
    return err;
}



/* Функция автоматического рассчета параметов DMA для входного потока
   на основе частоты сбора данных */
static int32_t f_stream_in_cfg(t_x502 *hnd) {
    int32_t err = X502_ERR_OK;


    t_x502_stream_ch_params params;
    double din_freq = 0;
    uint32_t total_size = 0;
    int ref_freq = hnd->set.ref_freq;


    memset(&params, 0, sizeof(params));

    /* рассчитываем частоту сбора для потоков АЦП и DIN */
    if (hnd->streams & X502_STREAM_ADC) {
        double f_frame;
        X502_GetAdcFreq(hnd, NULL, &f_frame);
        din_freq = f_frame*hnd->set.lch_cnt;
    }

    if (hnd->streams & X502_STREAM_DIN) {
        din_freq+=ref_freq/hnd->set.din_freq_div;
    }

    /* размер полного буфера определяем таким, чтобы его хватило на
       L502_DMA_IN_BUF_FOR_SEC секунд постоянного сбора, но не меньше минимального
       размера */
    total_size = (uint32_t)(X502_DMA_IN_BUF_FOR_SEC*din_freq);
    if (total_size < hnd->iface_hnd->in_stream_buf_min)
        total_size = hnd->iface_hnd->in_stream_buf_min;






    /* рассчитываем IRQ_STEP, чтобы он был  L502_DMA_IN_MAX_IRQ_PER_SEC
       в секунду */

    params.step = hnd->stream_pars[X502_STREAM_CH_IN].step ?
                hnd->stream_pars[X502_STREAM_CH_IN].step :
                (din_freq>X502_DMA_IN_MAX_IRQ_PER_SEC) ?
                (uint32_t)(din_freq/X502_DMA_IN_MAX_IRQ_PER_SEC) : 1;

    /* для эффиктивности делаем размер буфера кратным irq_step */
    total_size = ((total_size+params.step-1)/params.step)*params.step;

    params.buf_size = hnd->stream_pars[X502_STREAM_CH_IN].buf_size ?
                hnd->stream_pars[X502_STREAM_CH_IN].buf_size : total_size;

    err = hnd->iface_hnd->stream_cfg(hnd, X502_STREAM_CH_IN, &params);

    if ((err == X502_ERR_OK) && (hnd->mode == X502_MODE_DSP)) {
        /* для BlackFin нужно так же установить еще шаг прерываний для
           приема данных от SPORT'а */
        uint32_t size;
        err = x502_bf_get_par(hnd, L502_BF_PARAM_IN_BUF_SIZE, &size, 1);
        if (err == X502_ERR_OK) {
            uint32_t instep = params.step;
            if (instep>size/4) {
                instep=size/4;
            }
            if (instep > 0x8000)
                instep = 0x8000;
            err = x502_bf_set_par(hnd, L502_BF_PARAM_IN_STEP_SIZE, &instep, 1);
        }
    }
    return err;
}





static int32_t f_out_stream_preload(t_x502 *hnd) {
    int32_t err;
    STREAM_OUT_CFG(hnd, err);
    if (err == X502_ERR_OK)
        err = hnd->iface_hnd->stream_start(hnd, X502_STREAM_CH_OUT, 0);

    if ((err == X502_ERR_OK) && (hnd->mode == X502_MODE_DSP)) {
        err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_PRELOAD, 0, NULL, 0,
                             NULL, 0, X502_BF_REQ_TOUT, NULL);
    }

    if (err == X502_ERR_OK) {
        hnd->flags |= PRIV_FLAGS_PRELOAD_DONE;
    }
    return err;
}





X502_EXPORT(int32_t) X502_StreamsEnable(t_x502_hnd hnd, uint32_t streams) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
    if (err == X502_ERR_OK) {
        if (hnd->mode == X502_MODE_DSP) {
            err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_STREAM_EN, streams,
                                 NULL, 0, NULL, 0, X502_BF_REQ_TOUT, NULL);
            if (err == X502_ERR_OK)
                hnd->streams |= streams;
        } else {
            uint32_t old_streams = hnd->streams;
            err = f_set_streams(hnd, hnd->streams | streams);

            if (hnd->flags & PRIV_FLAGS_STREAM_RUN) {
                /* если не было разрешено потока на ввод до этого,
                   а при вызове стал разрешен => инициализируем его */
                if ((err == X502_ERR_OK) && !(old_streams & X502_STREAM_ALL_IN) &&
                        (streams & X502_STREAM_ALL_IN)) {
                    err = f_stream_in_cfg(hnd);
                    if (err == X502_ERR_OK)
                        err = hnd->iface_hnd->stream_start(hnd, X502_STREAM_CH_IN, 0);
                } else if ((err == X502_ERR_OK) && ((old_streams & X502_STREAM_ALL_IN) !=
                                  (streams & X502_STREAM_ALL_IN))) {
                    err =f_stream_in_cfg(hnd);
                }
            }
        }
        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}

X502_EXPORT(int32_t) X502_StreamsDisable(t_x502_hnd hnd, uint32_t streams) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);

    if (err == X502_ERR_OK) {
        if (hnd->mode == X502_MODE_DSP) {
            err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_STREAM_DIS, streams,
                                 NULL, 0, NULL, 0, X502_BF_REQ_TOUT, NULL);
            if (err == X502_ERR_OK)
                hnd->streams &= ~streams;

        } else {
            uint32_t old_streams = hnd->streams;
            err = f_set_streams(hnd, hnd->streams & ~streams);

            if (hnd->flags & PRIV_FLAGS_STREAM_RUN) {
                /* если все потоки на ввод были запрещены, то
                   останавливаем их */
                if ((err == X502_ERR_OK) && (old_streams & X502_STREAM_ALL_IN) &&
                        !(hnd->streams & X502_STREAM_ALL_IN)) {
                    err = hnd->iface_hnd->stream_stop(hnd, X502_STREAM_CH_IN, 0);
                }

                if ((err == X502_ERR_OK) && (old_streams & X502_STREAM_ALL_OUT) &&
                        !(hnd->streams & X502_STREAM_ALL_OUT)) {
                    err = hnd->iface_hnd->stream_stop(hnd, X502_STREAM_CH_OUT, 0);
                    if (err == X502_ERR_OK) {
                        hnd->flags &= ~PRIV_FLAGS_PRELOAD_DONE;
                    }
                }
            }
        }

        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}

X502_EXPORT(int32_t) X502_GetEnabledStreams(t_x502_hnd hnd, uint32_t* streams) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if ((err == X502_ERR_OK) && (streams == NULL))
        err = X502_ERR_INVALID_POINTER;
    if (err == X502_ERR_OK)
        *streams = hnd->streams;
    return err;
}



X502_EXPORT(int32_t) X502_StreamsStart(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if ((err == X502_ERR_OK) && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if ((err == X502_ERR_OK) && (hnd->mode==X502_MODE_FPGA)) {
        uint32_t reg;
        err = hnd->iface_hnd->fpga_reg_read(hnd, X502_REGS_IOHARD_IO_MODE, &reg);
        if ((err == X502_ERR_OK) && !(reg & X502_REGBIT_ADC_SLV_CLK_LOCK_Msk))
            err = X502_ERR_REF_FREQ_NOT_LOCKED;
    }

    if (err == X502_ERR_OK)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);

    if (err == X502_ERR_OK) {
        int in_started = 0;
        f_set_streams(hnd, hnd->streams);

        /** @todo корректные действия, если сбор в Bf не остановлен */

        /* выполняем предзагрузку первого слова выходного потока и
           коммутатора АЦП (при наличии DSP это делает DSP) */
        if ((err == X502_ERR_OK) && (hnd->mode==X502_MODE_FPGA)) {
            /* предзагрузку значения на вывод делаем только если реально данные уже были
             * предзагружены в буфер платы */
            if ((hnd->streams & X502_STREAM_ALL_OUT) &&
                    (hnd->flags & (PRIV_FLAGS_PRELOAD_DONE | PRIV_FLGAS_CYCLE_MODE))) {
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_OUTSWAP_BFCTL, 1);
                //if (!err)
                //    err = _fpga_reg_write(hnd, L502_REGS_IOHARD_DAC_DIGOUT_SWAP, 1);
                //if (!err)
                //    err = _fpga_reg_write(hnd, L502_REGS_IOHARD_DAC_DIGOUT_SWAP, 1);
            }


            /* предзагрузку АЦП должны делать всегда, так как по этой функции
             * выполняется часть инициализации параметров синхронного сбора! */
            if (err == X502_ERR_OK) {
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_PRELOAD_ADC, 1);
            }

            SLEEP_MS(20);
        }


        /* запуск потока на ввод выполняем при запуске синхронного ввода-вывода */
        if ((err == X502_ERR_OK) && (hnd->streams & X502_STREAM_ALL_IN)) {
            err = f_stream_in_cfg(hnd);
            if (err == X502_ERR_OK)
                err = hnd->iface_hnd->stream_start(hnd, X502_STREAM_CH_IN, 0);
            if (err == X502_ERR_OK)
                in_started = 1;
        }

        if (err == X502_ERR_OK) {
            if (hnd->mode == X502_MODE_FPGA) {
                /* взводим сигнал GO, указывающий что запущен синхронных ввод-вывод */
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 1);
            } else if (hnd->mode == X502_MODE_DSP) {
                err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_STREAM_START, 0,
                                  NULL, 0, NULL, 0, X502_BF_CMD_DEFAULT_TOUT, NULL);
            } else {
                err = X502_ERR_INVALID_MODE;
            }
        }

        if (err && in_started) {
            hnd->iface_hnd->stream_free(hnd, X502_STREAM_CH_IN, 0);
        }


        if (err == X502_ERR_OK) {
            hnd->flags |= PRIV_FLAGS_STREAM_RUN;
            hnd->proc_adc_ch = 0;
        }

        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}


X502_EXPORT(int32_t) X502_StreamsStop(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
    if (err == X502_ERR_OK) {
         int32_t stop_err1, stop_err2;

        if (hnd->mode==X502_MODE_FPGA) {
            err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 0);
        } else if (hnd->mode == X502_MODE_DSP) {           
            err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_STREAM_STOP, 0,
                              NULL, 0, NULL, 0, X502_BF_CMD_DEFAULT_TOUT, NULL);
        }

        stop_err1 = hnd->iface_hnd->stream_free(hnd, X502_STREAM_CH_IN, 0);
        stop_err2 = hnd->iface_hnd->stream_free(hnd, X502_STREAM_CH_OUT, 0);
        if (err == X502_ERR_OK)
            err = stop_err1;
        if (err == X502_ERR_OK)
            err = stop_err2;

        hnd->flags &= ~(PRIV_FLAGS_STREAM_RUN | PRIV_FLAGS_PRELOAD_DONE);

        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}

X502_EXPORT(int32_t) X502_IsRunning(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    uint32_t bf_mode=0;
    if ((err == X502_ERR_OK) && (hnd->mode==X502_MODE_DSP)) {
        err = x502_bf_get_par(hnd, L502_BF_PARAM_STREAM_MODE, &bf_mode, 1);
    }

    if (err == X502_ERR_OK)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
    if (err == X502_ERR_OK) {
        if (hnd->mode==X502_MODE_DSP) {
            if (bf_mode==L502_BF_MODE_IDLE) {
                hnd->flags &= ~PRIV_FLAGS_STREAM_RUN;
            } else {
                hnd->flags |= PRIV_FLAGS_STREAM_RUN;
            }
        }

        if (!(hnd->flags & PRIV_FLAGS_STREAM_RUN))
            err = X502_ERR_STREAM_IS_NOT_RUNNING;

        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}

X502_EXPORT(int32_t) X502_Recv(t_x502_hnd hnd, uint32_t* buf, uint32_t size, uint32_t tout) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if ((err == X502_ERR_OK) && (buf==NULL))
        err = X502_ERR_INVALID_POINTER;
    if (err == X502_ERR_OK) {
        err = hnd->iface_hnd->stream_read(hnd, buf, size, tout);
    }
    return err;
}

X502_EXPORT(int32_t) X502_Send(t_x502_hnd hnd, const uint32_t* buf, uint32_t size, uint32_t tout) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if ((err == X502_ERR_OK) && (buf==NULL))
        err = X502_ERR_INVALID_POINTER;

    /* если разрешен синхронный вывод, но не было
     * вызова X502_PreloadStart() или не был установлен синхронных режим, то
     * делаем запуск потока вывода при первой записи */
    if ((err == X502_ERR_OK) && (hnd->streams & X502_STREAM_ALL_OUT)) {
        if (err == X502_ERR_OK)
            err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
        if (err == X502_ERR_OK) {
            if (!(hnd->flags & (PRIV_FLAGS_PRELOAD_DONE | PRIV_FLGAS_CYCLE_MODE))) {
                err = f_out_stream_preload(hnd);
            }
            osspec_mutex_release(hnd->mutex_cfg);
        }
    }

    if (err == X502_ERR_OK) {
        err = hnd->iface_hnd->stream_write(hnd, buf, size, tout);
    }
    return err;
}

X502_EXPORT(int32_t)X502_PreloadStart(t_x502_hnd hnd) {
    int err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
    if (err == X502_ERR_OK) {
        err = f_out_stream_preload(hnd);
        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}

X502_EXPORT(int32_t) X502_GetRecvReadyCount(t_x502_hnd hnd, uint32_t *rdy_cnt) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if ((err == X502_ERR_OK) && (rdy_cnt==NULL))
        err = X502_ERR_INVALID_POINTER;
    if (err == X502_ERR_OK)
        err = hnd->iface_hnd->stream_get_rdy_cnt(hnd, X502_STREAM_CH_IN, rdy_cnt);
    return err;
}

X502_EXPORT(int32_t) X502_GetSendReadyCount(t_x502_hnd hnd, uint32_t *rdy_cnt) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if ((err == X502_ERR_OK) && (rdy_cnt==NULL))
        err = X502_ERR_INVALID_POINTER;
    if (err == X502_ERR_OK)
        err = hnd->iface_hnd->stream_get_rdy_cnt(hnd, X502_STREAM_CH_OUT, rdy_cnt);
    return err;
}


static int32_t f_check_stream_ch_par_en(t_x502_hnd hnd, uint32_t stream_ch) {
    int32_t err = 0;
    if (stream_ch == X502_STREAM_CH_IN) {
        if ((hnd->flags & PRIV_FLAGS_STREAM_RUN)
                && (hnd->streams & X502_STREAM_ALL_IN)) {
            err = X502_ERR_STREAM_IS_RUNNING;
        }
    } else if (stream_ch == X502_STREAM_CH_OUT) {
        if ((hnd->flags & (PRIV_FLAGS_PRELOAD_DONE | PRIV_FLAGS_STREAM_RUN))
                && (hnd->streams & X502_STREAM_ALL_OUT)) {
            err = X502_ERR_STREAM_IS_RUNNING;
        }
    } else {
        err = X502_ERR_INVALID_STREAM_CH;
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetStreamBufSize(t_x502_hnd hnd, uint32_t ch, uint32_t size) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = f_check_stream_ch_par_en(hnd, ch);
    if (err == X502_ERR_OK)
        hnd->stream_pars[ch].buf_size = size;
    return err;
}

X502_EXPORT(int32_t) X502_SetStreamStep(t_x502_hnd hnd, uint32_t ch, uint32_t step) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = f_check_stream_ch_par_en(hnd, ch);
    if (err == X502_ERR_OK)
        hnd->stream_pars[ch].step = step;
    return err;
}


X502_EXPORT(int32_t) X502_OutCycleLoadStart(t_x502_hnd hnd, uint32_t size) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK) {
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
        if (err == X502_ERR_OK) {
            /** @todo проверить правильность момента вызова */
            err = hnd->iface_hnd->cycle_load_start(hnd, size);
            if (err == X502_ERR_OK)
                hnd->flags |= PRIV_FLGAS_CYCLE_MODE;
            osspec_mutex_release(hnd->mutex_cfg);
        }
    }
    return err;
}





X502_EXPORT(int32_t) X502_OutCycleSetup(t_x502_hnd hnd, uint32_t flags) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = hnd->iface_hnd->cycle_setup(hnd, flags);
    return err;
}



X502_EXPORT(int32_t) X502_OutCycleStop(t_x502_hnd hnd, uint32_t flags) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = hnd->iface_hnd->cycle_stop(hnd, flags);
    if (err == X502_ERR_OK) {
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
        if (err == X502_ERR_OK) {
            hnd->flags &= ~PRIV_FLGAS_CYCLE_MODE;
            osspec_mutex_release(hnd->mutex_cfg);
        }
    }
    return err;
}


X502_EXPORT(int32_t) X502_AsyncGetAdcFrame(t_x502_hnd hnd, uint32_t flags,
                                            uint32_t tout, double* data) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if ((err == X502_ERR_OK) && (data==NULL))
        err = X502_ERR_INVALID_POINTER;
    if (err == X502_ERR_OK)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
    if (err == X502_ERR_OK) {
        if (hnd->mode == X502_MODE_FPGA) {
            /* если запущен хоть один поток на ввод синхронно, то ввод кадра
             * не возможне */
            if ((hnd->flags & PRIV_FLAGS_STREAM_RUN) && (hnd->streams & X502_STREAM_ALL_IN)) {
                err = X502_ERR_STREAM_IS_RUNNING;
            }

            if (err == X502_ERR_OK) {
                int need_stop = 0;
                int old_streams = hnd->streams;
                uint32_t *wrds = NULL;
                int32_t rcv_size = hnd->set.lch_cnt;

                /* разрешаем поток для АЦП */
                err = f_set_streams(hnd, (hnd->streams & ~X502_STREAM_ALL_IN)
                                    | X502_STREAM_ADC);

                hnd->proc_adc_ch = 0;

                if (err == X502_ERR_OK) {
                     /* инициализируем буфер для приема - достаточно всего на один кадр */
                    t_x502_stream_ch_params par = hnd->stream_pars[X502_STREAM_CH_IN];
                    hnd->stream_pars[X502_STREAM_CH_IN].buf_size = hnd->iface_hnd->in_stream_buf_min;
                    hnd->stream_pars[X502_STREAM_CH_IN].step = hnd->set.lch_cnt;
                    err = f_stream_in_cfg(hnd);

                    /* восстанавливаем параметры в структуре, что были заданы
                      до этой функции */
                    hnd->stream_pars[X502_STREAM_CH_IN] = par;
                }

                if (err == X502_ERR_OK) {
                    /* выделяем массив для необработанных отсчетов */
                    wrds = malloc(sizeof(wrds[0])*rcv_size);
                    if (wrds==NULL)
                        err = X502_ERR_MEMORY_ALLOC;
                }

                /* предзагрузка логической таблицы для АЦП */
                if (err == X502_ERR_OK)
                    err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_PRELOAD_ADC, 1);

                /* запуск канала DMA на прием данных */
                if (err == X502_ERR_OK) {
                    err = hnd->iface_hnd->stream_start(hnd, X502_STREAM_CH_IN,
                                                       X502_STREAM_FLAG_SINGLE);
                }

                /* если общий синхронный ввод не был запущен  - разрешаем его */
                if (err == X502_ERR_OK) {
                    if (!(hnd->flags & PRIV_FLAGS_STREAM_RUN)) {
                        err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 1);
                        if (err == X502_ERR_OK)
                            need_stop = 1;
                    }
                }

                if (err == X502_ERR_OK) {
                    /* принимаем отсчеты от одного кадра */
                    int32_t rcv = X502_Recv(hnd, wrds, rcv_size, tout);
                    if (rcv < 0) {
                        err = rcv;
                    } else if (rcv!=rcv_size) {
                        err = X502_ERR_RECV_INSUFFICIENT_WORDS;
                    } else {
                        err = X502_ProcessAdcData(hnd, wrds, data, (uint32_t*)&rcv_size, flags);
                    }
                }

                /* если в этой функции запустили синхронный сбор, то останвливаем его */
                if (need_stop)
                    hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 0);

                hnd->proc_adc_ch = 0;
                hnd->iface_hnd->stream_free(hnd, X502_STREAM_CH_IN, 0);
                /* восстанавливаем те потоки, которые были разрешены */
                f_set_streams(hnd, old_streams);
                free(wrds);
            }
        } else if (hnd->mode == X502_MODE_DSP) {
            err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_ADC_GET_FRAME,
                                 0, NULL, 0, (uint32_t*)data, 2*hnd->set.lch_cnt,
                                 X502_BF_CMD_DEFAULT_TOUT, NULL);
        } else {
            err = X502_ERR_INVALID_MODE;
        }
        osspec_mutex_release(hnd->mutex_cfg);
    }

    return err;
}


X502_EXPORT(int32_t) X502_ManualStreamStart(t_x502_hnd hnd, uint32_t stream_ch, uint32_t flags) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = X502_StreamsEnable(hnd, stream_ch == X502_STREAM_CH_IN ? X502_STREAM_ALL_IN : X502_STREAM_ALL_OUT );
    if (err == X502_ERR_OK) {
        if (stream_ch == X502_STREAM_CH_IN) {
            err = f_stream_in_cfg(hnd);
        } else {
            STREAM_OUT_CFG(hnd, err);
        }
    }
    if (err == X502_ERR_OK)
        err = hnd->iface_hnd->stream_start(hnd, stream_ch, flags);
    if ((err == X502_ERR_OK) && (stream_ch == X502_STREAM_CH_OUT)) {
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
        if (err==X502_ERR_OK) {
            hnd->flags |= PRIV_FLAGS_PRELOAD_DONE;
            osspec_mutex_release(hnd->mutex_cfg);
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_ManualStreamStop(t_x502_hnd hnd, uint32_t stream_ch) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK)
        err = X502_StreamsDisable(hnd, stream_ch == X502_STREAM_CH_IN ? X502_STREAM_ALL_IN : X502_STREAM_ALL_OUT );
    if (err == X502_ERR_OK)
        err = hnd->iface_hnd->stream_free(hnd, stream_ch, 0);
    return err;
}
