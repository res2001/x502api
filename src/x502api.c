#include "x502api.h"
#include "x502api_private.h"
#include "x502_fpga_regs.h"

#include <string.h>
#include <stdlib.h>





LPCIE_EXPORT(t_x502_hnd) X502_Create(void) {
    t_x502_hnd hnd = calloc(sizeof(t_x502), 1);
    if (hnd != NULL) {
        hnd->sign = X502_SIGN;
    }

    return hnd;
}

LPCIE_EXPORT(int32_t) X502_Free(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err) {
        if (hnd->flags & _FLAGS_OPENED)
            err = X502_Close(hnd);

        hnd->sign = 0;
        free(hnd);
    }
    return err;
}



LPCIE_EXPORT(int32_t) X502_Close(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err && (hnd->flags  & _FLAGS_OPENED)) {
        int32_t stop_err;
#if 0
        if (hnd->flags & _FLAGS_STREAM_RUN) {
            /* остановка потока */
            err = L502_StreamsStop(hnd);
        }

        if (hnd->mutex_bf!=L_INVALID_MUTEX)
        {
            stop_err = osspec_mutex_close(hnd->mutex_bf);
            hnd->mutex_bf = L_INVALID_MUTEX;
            if (!err)
                err = stop_err;
        }
#endif

        hnd->flags &= ~_FLAGS_OPENED;

        if (hnd->mutex_cfg!=L_INVALID_MUTEX) {
            stop_err = osspec_mutex_close(hnd->mutex_cfg);
            hnd->mutex_cfg = L_INVALID_MUTEX;
            if (!err)
                err = stop_err;
        }


        stop_err = hnd->iface->close(hnd);
        if (!err)
            err = stop_err;
    }
    return err;
}



LPCIE_EXPORT(int32_t) X502_OpenByDevinfo(t_x502* hnd, const t_lpcie_devinfo* info) {
    int32_t err  = X502_CHECK_HND(hnd);
    if (!err && (info==NULL))
        err = X502_ERR_INVALID_POINTER;
    if (!err && (hnd->flags & _FLAGS_OPENED))
        err = X502_ERR_ALREADY_OPENED;
    if (!err) {
        hnd->iface = (const t_x502_dev_iface *)(((t_lpcie_devinfo_inptr*)info->internal)->iface);
        memcpy(hnd->info.serial, info->serial, X502_SERIAL_SIZE);
        memcpy(hnd->info.name, info->devname, X502_DEVNAME_SIZE);

        err = hnd->iface->open(hnd, info);
        if (!err) {
            X502_SetLChannel(hnd, 0, 0, X502_LCH_MODE_COMM, X502_ADC_RANGE_10, 0);
            hnd->set.lch_cnt = 1;
            hnd->set.adc_freq_div = 1;
            hnd->set.adc_frame_delay = 0;
            hnd->set.sync_mode = X502_SYNC_INTERNAL;
            hnd->set.sync_mode = X502_SYNC_INTERNAL;
            hnd->set.sync_start_mode = X502_SYNC_INTERNAL;

            hnd->set.din_freq_div = 1;
            hnd->set.dac_freq_div = 2;

            hnd->set.ref_freq = X502_REF_FREQ_2000KHZ;
            hnd->streams = 0;

            hnd->flags = _FLAGS_OPENED;


            /* читаем информацию о версии прошивки ПЛИС'ов и наличии опций */
            if (!err) {
                uint32_t hard_id=0;
                err = hnd->iface->fpga_reg_read(hnd, hnd->iface->id_reg_addr, &hard_id);
                if (!err) {
                    hnd->info.fpga_ver = (hard_id >> 16) & 0xFFFF;
                    hnd->info.plda_ver = (hard_id >> 4) & 0xF;
                    hnd->info.board_rev  = (hard_id >> 8) & 0xF;
                    if (hard_id & 0x01)
                        hnd->info.devflags |= X502_DEVFLAGS_DAC_PRESENT;
                    if (hard_id & 0x02)
                        hnd->info.devflags |= X502_DEVFLAGS_GAL_PRESENT;
                    if (hard_id & 0x04)
                        hnd->info.devflags |= X502_DEVFLAGS_BF_PRESENT;
                }
            }

            /* определяем - в каком режиме работаем (BF или FPGA) */
            if (!err) {
                uint32_t bf_ctl;
                err = hnd->iface->fpga_reg_read(hnd, X502_REGS_BF_CTL, &bf_ctl);
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
                    err = hnd->iface->fpga_reg_write(hnd, X502_REGS_BF_CMD, X502_BF_CMD_HDMA_RST);
                }
            }

            /* если был запущен сбор - то останавливаем его */
            if (!err && (hnd->mode==X502_MODE_FPGA)) {
                err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 0);
                hnd->last_dout = 0;

                if (!err)
                    err =  hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_OUTSWAP_BFCTL, 0);
            }

#if 0
            if (!err && (hnd->info.devflags & X502_DEVFLAGS_BF_PRESENT))
            {
                hnd->mutex_bf = osspec_mutex_create();
                if (hnd->mutex_bf == L_INVALID_MUTEX)
                    err = L502_ERR_MUTEX_CREATE;
            }
            else
            {
                hnd->mutex_bf = L_INVALID_MUTEX;
            }
#endif

            if (!err) {
                hnd->mutex_cfg = osspec_mutex_create();
                if (hnd->mutex_cfg == L_INVALID_MUTEX)
                    err = X502_ERR_MUTEX_CREATE;
            }
        }

        /* читаем информацию из EEPROM (в первую очередь -
           калибровочные коэффициенты) */
        if (!err) {
            int i;

            for (i=0; i < X502_ADC_RANGE_CNT; i++) {
                hnd->info.cbr.adc[i].offs = 0;
                hnd->info.cbr.adc[i].k = 1.;
            }

            for (i=0; i < X502_DAC_CH_CNT; i++) {
                hnd->info.cbr.dac[i].offs = 0;
                hnd->info.cbr.dac[i].k = 1;
            }

#if 0
            /** @todo */
            _check_eeprom(hnd);
#endif
        }

        /* записываем конфигурацию по умолчанию, чтобы быть уверенным,
           что установлена нужная конфигурация */
        if (!err && (hnd->mode==X502_MODE_FPGA))
            err = X502_Configure(hnd, 0);
        //if (!err)
        //    err = _fpga_reg_write(hnd, L502_REGS_IOHARD_PRELOAD_ADC, 1);

        if (err)
            X502_Close(hnd);
    }

    return err;
}


LPCIE_EXPORT(int32_t) X502_Open(t_x502_hnd hnd, const char* serial,
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
        t_lpcie_devinfo *info_list = malloc(sizeof(t_lpcie_devinfo)*fnd_cnt);
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
                        err = X502_OpenByDevinfo(hnd, &info_list[i]);

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


            X502_FreeDevInfoList(info_list, fnd_cnt);
            free(info_list);
        }
    }
    return err;
}


LPCIE_EXPORT(int32_t) X502_FreeDevInfoList(t_lpcie_devinfo *list, uint32_t size) {
    uint32_t i;
    for (i=0; i < size; i++) {
        t_lpcie_devinfo_inptr* devinfo_ptr = (t_lpcie_devinfo_inptr*)list[i].internal;
        if (devinfo_ptr!=NULL) {
            const t_x502_dev_iface *iface = (const t_x502_dev_iface *)devinfo_ptr->iface;
            iface->free_devinfo_data(devinfo_ptr->iface_data);
            free(devinfo_ptr);
        }
        list[i].internal = NULL;
    }
    return 0;
}


LPCIE_EXPORT(int32_t) X502_GetDevInfo(t_x502_hnd hnd, t_x502_info* info) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err && (info==NULL))
        err = X502_ERR_INVALID_POINTER;

    if (!err)
        *info = hnd->info;

    return err;
}



