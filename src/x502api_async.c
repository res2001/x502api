#include "x502api_private.h"
#include "ltimer.h"

uint32_t prepare_dac_wrd(t_x502_hnd hnd, double val, uint32_t flags, const t_x502_cbr_coef* coef);


X502_EXPORT(int32_t) X502_AsyncOutDac(t_x502_hnd hnd, uint32_t ch, double data, uint32_t flags) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if (!err && (ch!=X502_DAC_CH1) && (ch!=X502_DAC_CH2))
        err = X502_ERR_INVALID_DAC_CHANNEL;
    if (!err && !(hnd->info.devflags & X502_DEVFLAGS_DAC_PRESENT))
        err = X502_ERR_DAC_NOT_PRESENT;
    if (!err) {
        uint32_t wr_val = prepare_dac_wrd(hnd, data, flags, &hnd->info.cbr.dac[ch]);

        if (ch==X502_DAC_CH1) {
            wr_val |= X502_STREAM_OUT_WORD_TYPE_DAC1;
        } else {
            wr_val |= X502_STREAM_OUT_WORD_TYPE_DAC2;
        }

        if (hnd->mode == X502_MODE_FPGA) {
            err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_ASYNC_OUT, wr_val);
        } else if (hnd->mode == X502_MODE_DSP) {
            err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_ASYNC_OUT,  ch==X502_DAC_CH1 ?
                                 L502_BF_CMD_ASYNC_TYPE_DAC1 : L502_BF_CMD_ASYNC_TYPE_DAC2,
                                 (uint32_t*)&wr_val, 1, NULL, 0, X502_BF_CMD_DEFAULT_TOUT, NULL);
        } else {
            err = X502_ERR_INVALID_MODE;
        }
    }
    return err;
}


X502_EXPORT(int32_t) X502_AsyncOutDig(t_x502_hnd hnd, uint32_t val, uint32_t msk) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if (!err) {
        if(hnd->mode == X502_MODE_FPGA) {
            err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
            if (!err) {
                int32_t release_err;
                /* маскированные биты берем из последнего выведенного значения */
                if (msk) {
                    val = val & ~msk;
                    val |= hnd->last_dout & msk;
                }

                err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_ASYNC_OUT,
                                                 val | X502_STREAM_OUT_WORD_TYPE_DOUT);
                /* сохраняем выведенное значения, для последующего использования
                   в маске */
                if (!err)
                    hnd->last_dout = val;

                release_err = osspec_mutex_release(hnd->mutex_cfg);
                if (!err)
                    err = release_err;
            }
        } else if (hnd->mode == X502_MODE_DSP) {
            uint32_t wrds[2] = {val, msk};
            err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_ASYNC_OUT, L502_BF_CMD_ASYNC_TYPE_DOUT,
                                 wrds, 2, NULL, 0, X502_BF_CMD_DEFAULT_TOUT, NULL);
        } else {
            err = X502_ERR_INVALID_MODE;
        }
    }
    return err;
}


static int32_t f_read_digin(t_x502_hnd hnd, uint32_t* din) {
    int32_t err = 0;
    uint32_t val;
    int rdy=0;
    t_ltimer tmr;

    /* читаем состояние входов, чтобы сбросить флаг готовности */
    err = hnd->iface->fpga_reg_read(hnd, X502_REGS_IOARITH_DIN_ASYNC, &val);

    /* читаем синхронный ввод до готовности данных или пока не
       выйдем по таймауту*/
    ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(500));
    while (!rdy && !ltimer_expired(&tmr) && (err == X502_ERR_OK)) {
        err = hnd->iface->fpga_reg_read(hnd, X502_REGS_IOARITH_DIN_ASYNC, &val);
        if (!err && (val & 0x80000000)) {
            rdy = 1;
            *din = (val & 0x3FFFF);
        }
    }

    if (!err && !rdy)
        err = X502_ERR_DIG_IN_NOT_RDY;
    return err;
}



X502_EXPORT(int32_t) X502_AsyncInDig(t_x502_hnd hnd, uint32_t* din) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if (!err & (din==NULL))
        err = X502_ERR_INVALID_POINTER;
    if (!err)
         err = osspec_mutex_lock(hnd->mutex_cfg, X502_MUTEX_CFG_LOCK_TOUT);
    if (!err) {
        if (hnd->mode == X502_MODE_FPGA) {
            if (hnd->flags & _FLAGS_STREAM_RUN) {
                err = f_read_digin(hnd, din);
            } else {
                /* запрещаем прием данных */
                err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOARITH_IN_STREAM_ENABLE, 0);
                if (!err) {
                    err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_PRELOAD_ADC, 1);
                    if (!err) {
                        /* запускаем чтение цифровых входов */
                        err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 1);
                    }
                }

                if (!err) {
                    int32_t stop_err;
                    err = f_read_digin(hnd, din);

                    /* останавливаем сбор данных */
                    stop_err = hnd->iface->fpga_reg_write(hnd, X502_REGS_IOHARD_GO_SYNC_IO, 0);
                    if (!err)
                        err = stop_err;
                }
            }
        } else if (hnd->mode == X502_MODE_DSP) {
            err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_ASYNC_DIG_IN,
                                 0, NULL, 0, din, 1, X502_BF_CMD_DEFAULT_TOUT, NULL);
        } else {
            err = X502_ERR_INVALID_MODE;
        }
        osspec_mutex_release(hnd->mutex_cfg);
    }
    return err;
}
