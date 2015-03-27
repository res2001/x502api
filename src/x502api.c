#include "x502api.h"
#include "x502api_private.h"
#include "x502_fpga_regs.h"

#include <string.h>
#include <stdlib.h>


static const double f_scales[] = {10., 5., 2., 1., 0.5, 0.2};


X502_EXPORT(t_x502_hnd) X502_Create(void) {
    t_x502_hnd hnd = calloc(sizeof(t_x502), 1);
    if (hnd != NULL) {
        hnd->sign = X502_SIGN;
    }

    return hnd;
}

X502_EXPORT(int32_t) X502_Free(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err) {
        if (hnd->flags & PRIV_FLAGS_OPENED)
            err = X502_Close(hnd);

        hnd->sign = 0;
        free(hnd);
    }
    return err;
}



X502_EXPORT(int32_t) X502_Close(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err && (hnd->flags  & PRIV_FLAGS_OPENED)) {
        int32_t stop_err;
        if (hnd->flags & PRIV_FLAGS_STREAM_RUN) {
            /* остановка потока */
            err = X502_StreamsStop(hnd);
        }

        hnd->flags &= ~PRIV_FLAGS_OPENED;


        stop_err = hnd->iface_hnd->close(hnd);
        if (!err)
            err = stop_err;

        if (hnd->mutex_cfg!=OSSPEC_INVALID_MUTEX) {
            stop_err = osspec_mutex_destroy(hnd->mutex_cfg);
            hnd->mutex_cfg = OSSPEC_INVALID_MUTEX;
            if (!err)
                err = stop_err;
        }

        if (hnd->mutex_bf!=OSSPEC_INVALID_MUTEX) {
            stop_err = osspec_mutex_destroy(hnd->mutex_bf);
            hnd->mutex_bf = OSSPEC_INVALID_MUTEX;
            if (!err)
                err = stop_err;
        }
    }
    return err;
}



X502_EXPORT(int32_t) X502_OpenByDevRecord(t_x502* hnd, const t_x502_devrec* info) {
    int32_t err  = X502_CHECK_HND(hnd);
    if ((err == X502_ERR_OK) && ((info==NULL) || (info->sign!=X502_DEVREC_SIGN)))
        err = X502_ERR_INVALID_DEVICE_RECORD;
    if ((err == X502_ERR_OK) && (hnd->flags & PRIV_FLAGS_OPENED))
        err = X502_ERR_ALREADY_OPENED;
    if (err == X502_ERR_OK) {
        hnd->iface_hnd = (const t_x502_dev_iface *)(info->internal->iface);
        memcpy(hnd->info.serial, info->serial, X502_SERIAL_SIZE);
        memcpy(hnd->info.name, info->devname, X502_DEVNAME_SIZE);
        hnd->iface = info->iface;
        err = hnd->iface_hnd->open(hnd, info);
        if (err == X502_ERR_OK) {
            X502_SetLChannel(hnd, 0, 0, X502_LCH_MODE_COMM, X502_ADC_RANGE_10, 0);
            hnd->set.lch_cnt = 1;
            hnd->set.adc_freq_div = 1;
            hnd->set.adc_frame_delay = 0;
            hnd->set.sync_mode = X502_SYNC_INTERNAL;
            hnd->set.sync_mode = X502_SYNC_INTERNAL;
            hnd->set.sync_start_mode = X502_SYNC_INTERNAL;

            hnd->set.din_freq_div = 1;
            hnd->set.out_freq_div = 2;

            hnd->set.ref_freq = X502_REF_FREQ_2000KHZ;
            hnd->streams = 0;

            hnd->flags = PRIV_FLAGS_OPENED;


            /* читаем информацию о версии прошивки ПЛИС'ов и наличии опций */
            if (err == X502_ERR_OK) {
                uint32_t hard_id=0;
                err = hnd->iface_hnd->fpga_reg_read(hnd, hnd->iface_hnd->id_reg_addr, &hard_id);
                if (!err) {
                    hnd->info.fpga_ver = (hard_id >> 16) & 0x7FFF;
                    hnd->info.plda_ver = (hard_id >> 4) & 0xF;
                    hnd->info.board_rev  = (hard_id >> 8) & 0xF;                    
                    FILL_HARD_ID_FLAGS(hnd->info.devflags, hard_id);
                }
            }

            /* определяем - в каком режиме работаем (BF или FPGA) */
            if (err == X502_ERR_OK) {
                uint32_t bf_ctl;
                err = hnd->iface_hnd->fpga_reg_read(hnd, X502_REGS_BF_CTL, &bf_ctl);
                if (!err) {
                    uint32_t mode = bf_ctl;
                    if (mode & X502_REGBIT_BF_CTL_DBG_MODE_Msk) {
                        hnd->mode = X502_MODE_DEBUG;
                    } else if (mode & X502_REGBIT_BF_CTL_DSP_MODE_Msk) {
                        hnd->mode = X502_MODE_DSP;
                    } else {
                        hnd->mode = X502_MODE_FPGA;
                    }
                }

                /** @todo Для BlackFin проверить наличие прошивки */

                if (hnd->mode==X502_MODE_DSP) {
                    err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_BF_CMD, X502_BF_CMD_HDMA_RST);
                }
            }

            /* если был запущен сбор - то останавливаем его */
            if ((err == X502_ERR_OK) && (hnd->mode==X502_MODE_FPGA)) {
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 0);
                hnd->last_dout = 0;

                if (err == X502_ERR_OK)
                    err =  hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_OUTSWAP_BFCTL, 0);

                if ((err == X502_ERR_OK) && (hnd->iface_hnd->stream_running != NULL)) {
                    int32_t running;
                    unsigned ch;
                    for (ch=0; (ch < X502_STREAM_CH_CNT) && !err; ch++) {
                        err = hnd->iface_hnd->stream_running(hnd, ch, &running);
                        if (!err && running) {
                            err = hnd->iface_hnd->stream_stop(hnd, ch);
                        }
                    }
                }
            }


            if ((err == X502_ERR_OK) && (hnd->info.devflags & X502_DEVFLAGS_BF_PRESENT)) {
                hnd->mutex_bf = osspec_mutex_create();
                if (hnd->mutex_bf == OSSPEC_INVALID_MUTEX)
                    err = X502_ERR_MUTEX_CREATE;
            } else {
                hnd->mutex_bf = OSSPEC_INVALID_MUTEX;
            }


            if (err == X502_ERR_OK) {
                hnd->mutex_cfg = osspec_mutex_create();
                if (hnd->mutex_cfg == OSSPEC_INVALID_MUTEX)
                    err = X502_ERR_MUTEX_CREATE;
            }
        }

        /* читаем информацию из EEPROM (в первую очередь -
           калибровочные коэффициенты) */
        if (err == X502_ERR_OK) {
            int i;

            for (i=0; i < X502_ADC_RANGE_CNT; i++) {
                hnd->info.cbr.adc[i].offs = 0;
                hnd->info.cbr.adc[i].k = 1.;
            }

            for (i=0; i < X502_DAC_CH_CNT; i++) {
                hnd->info.cbr.dac[i].offs = 0;
                hnd->info.cbr.dac[i].k = 1;
            }

            x502_check_eeprom(hnd);
        }

        /* записываем конфигурацию по умолчанию, чтобы быть уверенным,
           что установлена нужная конфигурация */
        if ((err == X502_ERR_OK) && (hnd->mode==X502_MODE_FPGA))
            err = X502_Configure(hnd, 0);
        //if (!err)
        //    err = _fpga_reg_write(hnd, L502_REGS_IOHARD_PRELOAD_ADC, 1);

        if (err != X502_ERR_OK)
            X502_Close(hnd);
    }

    return err;
}


X502_EXPORT(int32_t) X502_Open(t_x502_hnd hnd, const char* serial,
                                const char *devname, t_x502_get_devinfo_list_cb get_list) {
    int32_t err = X502_CHECK_HND(hnd);
    int32_t get_info_res = 0;
    uint32_t fnd_cnt;

    if (!err) {
        get_info_res = get_list(NULL, 0, 0, &fnd_cnt);
        if (get_info_res < 0) {
            err = get_info_res;
        } else if (!fnd_cnt) {
            err = X502_ERR_DEVICE_NOT_FOUND;
        }
    }

    if (!err) {
        t_x502_devrec *info_list = malloc(sizeof(t_x502_devrec)*fnd_cnt);
        if (info_list==NULL) {
            err = X502_ERR_MEMORY_ALLOC;
        } else {
            /* получаем информацию по всем устройствам драйвера lpcie */
            get_info_res = get_list(info_list, fnd_cnt, 0, NULL);
            if (get_info_res < 0) {
                err = get_info_res;
            } else {
                int32_t i, ser_size=0, fnd=0, open_err = 0;

                if (serial!=NULL) {
                    /* если серийный задан - смотрим его размер, отсекая
                     * все что начинается с признака кноца строки */
                    for (ser_size=0; (ser_size < X502_SERIAL_SIZE) &&
                         !((serial[ser_size]=='\0') ||
                           (serial[ser_size]=='\n') ||
                           (serial[ser_size]=='\r')); ser_size++)
                    {}
                }

                for (i=0; !fnd && (i < get_info_res); i++)  {
                    /* ищем устройство L502 с совпадающим серийным */
                    if (((devname==NULL) || !strcmp(info_list[i].devname, devname)) &&
                            ((ser_size==0) || !strncmp(serial, info_list[i].serial,
                                                        ser_size))) {
                        /* пробуем открыть устройство */
                        err = X502_OpenByDevRecord(hnd, &info_list[i]);

                        /* если серийный номер не был указан, то сохраняем
                           код ошибки и идем дальше, пробовать открыть
                           следующее устройство */
                        if (err && (ser_size==0)) {
                            open_err = err;
                            err = 0;
                        } else {
                            /* иначе заканчиваем поиск */
                            fnd = 1;
                        }
                    }
                }

                /* если не нашли устройство - устанавливаем соотвествующий код
                   ошибки */
                if (!fnd) {
                    err = open_err ? open_err : X502_ERR_DEVICE_NOT_FOUND;
                }
            }


            X502_FreeDevRecordList(info_list, fnd_cnt);
            free(info_list);
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_GetSerialList(char serials[][X502_SERIAL_SIZE], uint32_t size,
                                        uint32_t flags, uint32_t *devcnt, const char *devname,
                                        t_x502_get_devinfo_list_cb get_list) {
    uint32_t fnd_cnt=0, put_cnt=0;
    /* получаем количество устройств, поддерживаемых драйвером lpcie */
    int32_t res = get_list(NULL, 0, flags, &fnd_cnt);
    if ((res>=0) && fnd_cnt) {
        t_x502_devrec *info_list = malloc(sizeof(t_x502_devrec)*fnd_cnt);
        if (info_list==NULL) {
            res = X502_ERR_MEMORY_ALLOC;
        } else {
            /* получаем информацию по всем устройствам драйвера lpcie */
            res = get_list(info_list, fnd_cnt, flags, NULL);
            if (res>0) {
                int32_t i;
                for (i=0; i < res; i++) {
                    /* проверяем, что это устройство - E502 */
                    if (!strcmp(info_list[i].devname, devname)) {
                        /* если есть место в списке, то сохраняем серийный номер
                           устройства */
                        if (put_cnt < size) {
                            memcpy(serials[put_cnt], info_list[i].serial,
                                   X502_SERIAL_SIZE);
                        }
                        put_cnt++;
                    }
                }
            }

            X502_FreeDevRecordList(info_list, fnd_cnt);
            free(info_list);
        }
    }

    if (devcnt != NULL)
        *devcnt = put_cnt;

    return res < 0 ? res : put_cnt > size ? (int32_t)size : (int32_t)put_cnt;
}

X502_EXPORT(int32_t) X502_FreeDevRecordList(t_x502_devrec *list, uint32_t size) {
    uint32_t i;
    int32_t err = X502_ERR_OK;
    if (list!=NULL) {
        for (i=0; (i < size) && (err==X502_ERR_OK); i++) {
            if (list[i].sign!=X502_DEVREC_SIGN) {
                err = X502_ERR_INVALID_DEVICE_RECORD;
            } else {
                t_x502_devrec_inptr *devinfo_ptr = (t_x502_devrec_inptr*)list[i].internal;
                if (devinfo_ptr != NULL) {
                    const t_x502_dev_iface *iface = (const t_x502_dev_iface *)devinfo_ptr->iface;
                    if (iface!=NULL)
                        iface->free_devinfo_ptr(devinfo_ptr);

                }
                list[i].internal = NULL;
            }
        }
    }
    return err;
}


X502_EXPORT(int32_t) X502_GetDevInfo(t_x502_hnd hnd, t_x502_info* info) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (info==NULL))
        err = X502_ERR_INVALID_POINTER;

    if (!err)
        *info = hnd->info;

    return err;
}




X502_EXPORT(int32_t) X502_GetNextExpectedLchNum(t_x502_hnd hnd, uint32_t *lch) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (lch==NULL))
        err = X502_ERR_INVALID_POINTER;
    if (!err)
        *lch = hnd->proc_adc_ch;
    return err;
}

#define STREAM_IN_WRD_TYPE(wrd) wrd & 0x80000000 ? STREAM_IN_WRD_ADC : \
      (wrd & 0xFF000000) == 0x0 ? STREAM_IN_WRD_DIN : \
    ((wrd & 0xFF000000)>>24) == 0x01 ? STREAM_IN_WRD_MSG : STREAM_IN_WRD_USR


X502_EXPORT(int32_t) X502_ProcessAdcData(t_x502_hnd hnd, const uint32_t* src,  double *dest,
                            uint32_t *size, uint32_t flags) {
    if (size == NULL)
        return X502_ERR_INVALID_POINTER;
    return X502_ProcessDataWithUserExt(hnd, src, *size, flags, dest, size,
                                        NULL, NULL, NULL, NULL);
}

X502_EXPORT(int32_t) X502_ProcessData(t_x502_hnd hnd, const uint32_t *src, uint32_t size, uint32_t flags,
                           double *adc_data, uint32_t *adc_data_size,
                           uint32_t *din_data, uint32_t *din_data_size) {
    return X502_ProcessDataWithUserExt(hnd, src, size, flags, adc_data, adc_data_size,
                                      din_data, din_data_size, NULL, NULL);
}


X502_EXPORT(int32_t) X502_ProcessDataWithUserExt(t_x502_hnd hnd, const uint32_t* src, uint32_t size,
                                   uint32_t flags, double *adc_data,
                                   uint32_t *adc_data_size, uint32_t *din_data, uint32_t *din_data_size,
                                   uint32_t *usr_data, uint32_t *usr_data_size) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (adc_data_size==NULL) && (adc_data!=NULL))
        err = X502_ERR_INVALID_POINTER;
    if (!err && (din_data_size==NULL) && (din_data!=NULL))
        err = X502_ERR_INVALID_POINTER;
    if (!err && (usr_data_size==NULL) && (usr_data!=NULL))
        err = X502_ERR_INVALID_POINTER;
    if (!err) {
        uint32_t adc_cnt = 0, din_cnt=0, usr_cnt = 0;
        uint32_t i;

        for (i=0; (i<size) && !err; i++) {
            register uint32_t wrd = src[i];
            t_stream_in_wrd_type type = STREAM_IN_WRD_TYPE(wrd);
            /* проверяем - это данные от АЦП или цифровых входов */
            switch (type) {
                case STREAM_IN_WRD_ADC: {
                    uint32_t ch_num = (wrd >> 24) & 0xF;
                    uint32_t ch_mode = (wrd >> 28) & 0x3;
                    int32_t val;
                    uint32_t range;

                    /* проверяем совпадение каналов */
                    switch (ch_mode) {
                        case 0:
                            ch_mode = X502_LCH_MODE_DIFF;
                            break;
                        case 1:
                            ch_mode = X502_LCH_MODE_COMM;
                            break;
                        case 2:
                            ch_mode = X502_LCH_MODE_COMM;
                            ch_num+=16;
                            break;
                        case 3:
                            ch_mode = X502_LCH_MODE_ZERO;
                            break;
                    }

                    if (!(flags & X502_PROC_FLAGS_DONT_CHECK_CH) &&
                            ((hnd->set.lch[hnd->proc_adc_ch].mode != ch_mode) ||
                            (hnd->set.lch[hnd->proc_adc_ch].ch != ch_num))) {
                        err = X502_ERR_PROC_INVALID_CH_NUM;
                    } else {
                        /* проверяем формат - пришло откалиброванное 24-битное слово,
                          или неоткалиброванное 16-битное */
                        if (wrd & 0x40000000) {
                            val = wrd & 0xFFFFFF;
                            if (wrd & 0x800000)
                                val |= 0xFF000000;

                            range = hnd->set.lch[hnd->proc_adc_ch].range;
                        } else {
                            range = (wrd >> 16) & 0x7;
                            if (!(flags & X502_PROC_FLAGS_DONT_CHECK_CH) &&
                                    (range != hnd->set.lch[hnd->proc_adc_ch].range)) {
                                err = X502_ERR_PROC_INVALID_CH_RANGE;
                            } else {
                                val = wrd & 0xFFFF;
                                if (wrd & 0x8000)
                                    val |= 0xFFFF0000;
                            }
                        }
                    }

                    if (!err) {
                        double res_val = val;
                        if (flags & X502_PROC_FLAGS_VOLT) {
                            res_val = f_scales[range]*res_val/X502_ADC_SCALE_CODE_MAX;
                        }


                        if ((adc_data!=NULL) && (adc_cnt<*adc_data_size)) {
                            *adc_data++ = res_val;
                            adc_cnt++;
                        } else if (adc_data==NULL) {
                            adc_cnt++;
                        }

                        if (++hnd->proc_adc_ch ==hnd->set.lch_cnt)
                            hnd->proc_adc_ch = 0;
                    }
                }
                break;
            case STREAM_IN_WRD_DIN:
                if ((din_data!=NULL) && (din_cnt<*din_data_size)) {
                    *din_data++ = wrd & 0x3FFFF;
                    din_cnt++;
                } else if (din_data==NULL) {
                    din_cnt++;
                }
                break;
            case STREAM_IN_WRD_MSG:
                err = wrd==X502_STREAM_IN_MSG_OVERFLOW ? X502_ERR_STREAM_OVERFLOW :
                                                         X502_ERR_UNSUP_STREAM_MSG;
                break;
            case STREAM_IN_WRD_USR:
                if ((usr_data!=NULL) && (usr_cnt<*usr_data_size)) {
                    *usr_data++ = wrd;
                    usr_cnt++;
                } else if (usr_data==NULL) {
                    usr_cnt++;
                }
                break;
            }
        }


        if (adc_data_size!=NULL)
            *adc_data_size = adc_cnt;
        if (din_data_size!=NULL)
            *din_data_size = din_cnt;
        if (usr_data_size!=NULL)
            *usr_data_size = usr_cnt;
    }

    return err;
}

uint32_t prepare_dac_wrd(t_x502_hnd hnd, double val, uint32_t flags, const t_x502_cbr_coef* coef) {
    int32_t wrd = 0;
    if (flags & X502_DAC_FLAGS_VOLT) {
        val = (val/X502_DAC_RANGE)*X502_DAC_SCALE_CODE_MAX;
    }
    if (flags & X502_DAC_FLAGS_CALIBR) {
        val = (val+coef->offs)*coef->k;
    }
    wrd = (int32_t)val;
    wrd &= 0xFFFF;
    return wrd;
}

X502_EXPORT(int32_t) X502_PrepareData(t_x502_hnd hnd, const double* dac1, const double* dac2,
                            const uint32_t* digout, uint32_t size, int32_t flags,
                            uint32_t* out_buf) {
    int err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (out_buf==NULL))
        err = X502_ERR_INVALID_POINTER;
    if (!err && ((dac1==NULL) && (dac2==NULL) && (digout==NULL)))
        err = X502_ERR_INVALID_POINTER;


    if (err == X502_ERR_OK) {
        uint32_t i;
        for (i = 0; (i < size) && (err == X502_ERR_OK); i++) {
            if ((dac1 != NULL) && (hnd->streams & X502_STREAM_DAC1)) {
                uint32_t wrd = prepare_dac_wrd(hnd, *dac1++, flags, &hnd->info.cbr.dac[0]);
                *out_buf++ = wrd | X502_STREAM_OUT_WORD_TYPE_DAC1;
            }
            if ((dac2 != NULL) && (hnd->streams & X502_STREAM_DAC2)) {
                uint32_t wrd = prepare_dac_wrd(hnd, *dac2++, flags, &hnd->info.cbr.dac[1]);
                *out_buf++ = wrd | X502_STREAM_OUT_WORD_TYPE_DAC2;
            }
            if ((digout != NULL) && (hnd->streams & X502_STREAM_DOUT)) {
                uint32_t wrd = *digout++;
                *out_buf++ = (wrd &0x3FFFF)
                             | X502_STREAM_OUT_WORD_TYPE_DOUT;
            }
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_FpgaRegWrite(t_x502_hnd hnd, uint32_t reg, uint32_t val) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err)
        err = hnd->iface_hnd->fpga_reg_write(hnd, reg & 0xFFFF, val);
    return err;
}

X502_EXPORT(int32_t) X502_FpgaRegRead(t_x502_hnd hnd, uint32_t reg, uint32_t *val) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err)
        err = hnd->iface_hnd->fpga_reg_read(hnd, reg & 0xFFFF, val);
    return err;
}


X502_EXPORT(uint32_t) X502_GetLibraryVersion(void) {
    return (X502API_VER_MAJOR << 24) | (X502API_VER_MINOR<<16) |
            (X502API_VER_PATCH << 8);
}


X502_EXPORT(int32_t) X502_DevRecordInit(t_x502_devrec *rec) {
    if (rec!=NULL) {
        memset(rec, 0, sizeof(t_x502_devrec));
        rec->sign = X502_DEVREC_SIGN;
    }
    return X502_ERR_OK;
}

X502_EXPORT(int32_t) X502_LedBlink(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err) {
        if (hnd->mode == X502_MODE_DSP) {
            /** @todo */
            err = X502_ERR_NOT_IMPLEMENTED;
        } else {
            int32_t err2;
            err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_LED, 0);
            SLEEP_MS(200);
            err2 = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_LED, 1);
            if (!err)
                err = err2;
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetDigInPullup(t_x502_hnd hnd, uint32_t pullups) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err) {
        if (hnd->mode==X502_MODE_FPGA) {
            uint32_t val = 0;
            if (pullups & X502_PULLUPS_DI_L)
                val |= 0x1;
            if (pullups & X502_PULLUPS_DI_H)
                val |= 0x2;
            if (pullups & X502_PULLUPS_DI_SYN1)
                val |= 0x4;
            if (pullups & X502_PULLUPS_DI_SYN2)
                val |= 0x8;
            if (pullups & X502_PULLDOWN_CONV_IN)
                val |= 0x10;
            if (pullups & X502_PULLDOWN_SYNC_IN)
                val |= 0x20;
            err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_DIGIN_PULLUP, val);
        } else if (hnd->mode == X502_MODE_DSP) {
            err = X502_ERR_NOT_IMPLEMENTED;
        } else {
            err = X502_ERR_INVALID_MODE;
        }
    }
    return err;
}

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hmod, DWORD reason, LPVOID resvd) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif
