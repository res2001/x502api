#include "x502api_private.h"
#include "x502_fpga_regs.h"

#include <string.h>


/* разрешение/запрещение потоков ввода вывода в соответствии с полем hnd->streams */
static int32_t f_set_streams(t_x502_hnd hnd, uint32_t streams) {
    int32_t err = 0;
    if (hnd->mode == X502_MODE_FPGA)  {
        err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOARITH_IN_STREAM_ENABLE,
                                        (streams & X502_STREAM_ADC ? 0x01 : 0) |
                                        (streams & X502_STREAM_DIN ? 0x02 : 0));
    }

    if (!err) {
        hnd->streams = streams;
    }
    return err;
}



/* Функция автоматического рассчета параметов DMA для входного потока
   на основе частоты сбора данных */
static int32_t f_stream_in_cfg(t_x502* hnd) {
    int32_t err = 0;


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
    if (total_size < X502_DMA_IN_BUF_SIZE_MIN)
        total_size = X502_DMA_IN_BUF_SIZE_MIN;






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

    err = hnd->iface->stream_cfg(hnd, X502_STREAM_CH_IN, &params);

    if (!err && (hnd->mode == X502_MODE_DSP)) {
        /** @todo */
#if 0
        /* для BlackFin нужно так же установить еще шаг прерываний для
           приема данных от SPORT'а */
        uint32_t size;
        err = _get_bf_par(hnd, L502_BF_PARAM_IN_BUF_SIZE, &size, 1);
        if (!err) {

            uint32_t instep = params.irq_step;
            if (instep>size/4) {
                instep=size/4;
            }
            if (instep > 0x8000)
                instep = 0x8000;
            err = _set_bf_par(hnd, L502_BF_PARAM_IN_STEP_SIZE, &instep, 1);
        }
#endif
    }
    return err;
}




LPCIE_EXPORT(int32_t) X502_StreamsEnable(t_x502_hnd hnd, uint32_t streams) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
    if (!err) {
        if (hnd->mode == X502_MODE_DSP) {
            /** @todo */
#if 0
            err = L502_BfExecCmd(hnd, L502_BF_CMD_CODE_STREAM_EN, streams,
                                 NULL, 0, NULL, 0, L502_BF_REQ_TOUT, NULL);
#endif
            if (!err)
                hnd->streams |= streams;
        } else {
            uint32_t old_streams = hnd->streams;
            err = f_set_streams(hnd, hnd->streams | streams);

            if (hnd->flags & _FLAGS_STREAM_RUN) {
                /* если не было разрешено потока на ввод до этого,
                   а при вызове стал разрешен => инициализируем его */
                if (!err && !(old_streams & X502_STREAM_ALL_IN) &&
                        (streams & X502_STREAM_ALL_IN)) {
                    err = f_stream_in_cfg(hnd);
                    if (!err)
                        err = hnd->iface->stream_start(hnd, X502_STREAM_CH_IN, 0);
                } else if (!err && ((old_streams & X502_STREAM_ALL_IN) !=
                                  (streams & X502_STREAM_ALL_IN))) {
                    err =f_stream_in_cfg(hnd);
                }
            }
        }
        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}

LPCIE_EXPORT(int32_t) X502_StreamsDisable(t_x502_hnd hnd, uint32_t streams) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);

    if (!err) {
        if (hnd->mode == X502_MODE_DSP) {
            /** @todo */
#if 0
            err = L502_BfExecCmd(hnd, L502_BF_CMD_CODE_STREAM_DIS, streams,
                                 NULL, 0, NULL, 0, L502_BF_REQ_TOUT, NULL);
#endif
            if (!err)
                hnd->streams &= ~streams;

        } else {
            uint32_t old_streams = hnd->streams;
            err = f_set_streams(hnd, hnd->streams & ~streams);

            if (hnd->flags & _FLAGS_STREAM_RUN) {
                /* если все потоки на ввод были запрещены, то
                   останавливаем их */
                if (!err&& (old_streams & X502_STREAM_ALL_IN) &&
                        !(hnd->streams & X502_STREAM_ALL_IN)) {
                    err = hnd->iface->stream_stop(hnd, X502_STREAM_CH_IN);
                }

                if (!err&& (old_streams & X502_STREAM_ALL_OUT) &&
                        !(hnd->streams & X502_STREAM_ALL_OUT)) {
                    err = hnd->iface->stream_stop(hnd, X502_STREAM_CH_OUT);
                    if (!err) {
                        hnd->flags &= ~_FLAGS_PRELOAD_DONE;
                    }
                }
            }
        }

        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}



LPCIE_EXPORT(int32_t) X502_StreamsStart(t_x502_hnd hnd) {
    int err = X502_CHECK_HND(hnd);
    if (!err && (hnd->flags & _FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err && (hnd->mode==X502_MODE_FPGA)) {
        uint32_t reg;
        err = hnd->iface->fpga_reg_read(hnd, X502_REGS_IOHARD_IO_MODE, &reg);
        if (!err && !(reg & X502_REGBIT_ADC_SLV_CLK_LOCK_Msk))
            err = X502_ERR_REF_FREQ_NOT_LOCKED;
    }

    if (!err)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);

    if (!err) {
        int in_started = 0;
        f_set_streams(hnd, hnd->streams);



        /* выполняем предзагрузку первого слова выходного потока и
           коммутатора АЦП (при наличии DSP это делает DSP) */
        if (!err && (hnd->mode==X502_MODE_FPGA)) {
            /* предзагрузку значения на вывод делаем только если реально данные уже были
             * предзагружены в буфер платы */
            if ((hnd->streams & X502_STREAM_ALL_OUT) &&
                    (hnd->flags & (_FLAGS_PRELOAD_DONE | _FLGAS_CYCLE_MODE))) {
                err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_OUTSWAP_BFCTL, 1);
                //if (!err)
                //    err = _fpga_reg_write(hnd, L502_REGS_IOHARD_DAC_DIGOUT_SWAP, 1);
                //if (!err)
                //    err = _fpga_reg_write(hnd, L502_REGS_IOHARD_DAC_DIGOUT_SWAP, 1);
            }


            /* предзагрузку АЦП должны делать всегда, так как по этой функции
             * выполняется часть инициализации параметров синхронного сбора! */
            if (!err) {
                err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_PRELOAD_ADC, 1);
            }

            SLEEP_MS(20);
        }


        /* запуск потока на ввод выполняем при запуске синхронного ввода-вывода */
        if (!err && (hnd->streams & X502_STREAM_ALL_IN)) {
            err = f_stream_in_cfg(hnd);
            if (!err)
                err = hnd->iface->stream_start(hnd, X502_STREAM_CH_IN, 0);
            if (!err)
                in_started = 1;
        }

        if (!err) {
            if (hnd->mode == X502_MODE_FPGA) {
                /* взводим сигнал GO, указывающий что запущен синхронных ввод-вывод */
                err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 1);
            } else if (hnd->mode == X502_MODE_DSP) {
                /** @todo */
#if 0
                err = L502_BfExecCmd(hnd, L502_BF_CMD_CODE_STREAM_START, 0,
                                  NULL, 0, NULL, 0, L502_BF_CMD_DEFAULT_TOUT, NULL);
#endif
            }
        }

        if (err && in_started) {
            err = hnd->iface->stream_free(hnd, X502_STREAM_CH_IN);
        }


        if (!err) {
            hnd->flags |= _FLAGS_STREAM_RUN;
            hnd->proc_adc_ch = 0;
        }

        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}


LPCIE_EXPORT(int32_t) X502_StreamsStop(t_x502_hnd hnd) {
    int err = X502_CHECK_HND(hnd);
    if (!err)
        err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
    if (!err) {
        if (hnd->mode==X502_MODE_FPGA) {
            err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 0);
        } else if (hnd->mode == X502_MODE_DSP) {
            /** @todo */
#if 0
            err = L502_BfExecCmd(hnd, L502_BF_CMD_CODE_STREAM_STOP, 0,
                              NULL, 0, NULL, 0, L502_BF_CMD_DEFAULT_TOUT, NULL);
#endif
        }

        if (!err)
            err = hnd->iface->stream_free(hnd, X502_STREAM_CH_IN);
        if (!err)
            err = hnd->iface->stream_free(hnd, X502_STREAM_CH_OUT);

        hnd->flags &= ~(_FLAGS_STREAM_RUN | _FLAGS_PRELOAD_DONE);

        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}

LPCIE_EXPORT(int32_t) X502_Recv(t_x502_hnd hnd, uint32_t* buf, uint32_t size, uint32_t tout) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err && (buf==NULL))
        err = X502_ERR_INVALID_POINTER;
    if (!err) {
        err = hnd->iface->stream_read(hnd, buf, size, tout);
    }
    return err;
}
