#include "x502api_private.h"
#include "x502_fpga_regs.h"

#define SYNC_FREQ_CODE(mode) ((mode == X502_SYNC_DI_SYN2_RISE) ? X502_SYNC_DI_SYN1_FALL : \
                               (mode == X502_SYNC_DI_SYN1_FALL) ? X502_SYNC_DI_SYN2_RISE : mode)

#define CHECK_SYNC_MODE(mode) (((mode)) != X502_SYNC_INTERNAL) \
    && ((mode)!=X502_SYNC_EXTERNAL_MASTER) \
    && ((mode)!=X502_SYNC_DI_SYN1_RISE) \
    && ((mode)!=X502_SYNC_DI_SYN2_RISE) \
    && ((mode)!=X502_SYNC_DI_SYN1_FALL) \
    && ((mode)!=X502_SYNC_DI_SYN2_FALL) \
    ? X502_ERR_INVALID_SYNC_MODE : X502_ERR_OK

static const uint16_t f_regadd_k[X502_ADC_RANGE_CNT] = {X502_REGS_IOARITH_K10,
                                                        X502_REGS_IOARITH_K5,
                                                        X502_REGS_IOARITH_K2,
                                                        X502_REGS_IOARITH_K1,
                                                        X502_REGS_IOARITH_K05,
                                                        X502_REGS_IOARITH_K02};

static const uint16_t f_regadd_offs[X502_ADC_RANGE_CNT] = {X502_REGS_IOARITH_B10,
                                                        X502_REGS_IOARITH_B5,
                                                        X502_REGS_IOARITH_B2,
                                                        X502_REGS_IOARITH_B1,
                                                        X502_REGS_IOARITH_B05,
                                                        X502_REGS_IOARITH_B02};




X502_EXPORT(int32_t) X502_SetLChannel(t_x502_hnd hnd, uint32_t lch, uint32_t phy_ch,
                                       uint32_t mode, uint32_t range, uint32_t avg) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err) {
        /* Для измерения собственного нуля номер канала не имеет значения */
        if (mode == X502_LCH_MODE_ZERO)
            phy_ch = 0;

        if ((mode != X502_LCH_MODE_COMM) && (mode != X502_LCH_MODE_DIFF) &&
                (mode != X502_LCH_MODE_ZERO)) {
            err = X502_ERR_INVALID_LCH_MODE;
        } else if ((phy_ch >= X502_ADC_COMM_CH_CNT) ||
                   ((mode == X502_LCH_MODE_DIFF) && (phy_ch >= X502_ADC_DIFF_CH_CNT))) {
            err = X502_ERR_INVALID_LCH_PHY_NUMBER;
        } else if (range >= X502_ADC_RANGE_CNT) {
            err = X502_ERR_INVALID_LCH_RANGE;
        } else if (avg > X502_LCH_AVG_SIZE_MAX) {
            err = X502_ERR_INVALID_LCH_AVG_SIZE;
        }

        if (!err) {
            hnd->set.lch[lch].ch = phy_ch;
            hnd->set.lch[lch].range = range;
            hnd->set.lch[lch].mode = mode;
            hnd->set.lch[lch].avg = avg;
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetLChannelCount(t_x502_hnd hnd, uint32_t lch_cnt) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err) {
        if (lch_cnt > X502_LTABLE_MAX_CH_CNT) {
            err = X502_ERR_INVALID_LTABLE_SIZE;
        } else {
            hnd->set.lch_cnt = lch_cnt;
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_GetLChannelCount(t_x502_hnd hnd, uint32_t* lch_cnt) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (lch_cnt==NULL))
        err = X502_ERR_INVALID_POINTER;

    if (!err)
        *lch_cnt = hnd->set.lch_cnt;

    return err;
}

X502_EXPORT(int32_t) X502_SetAdcFreqDivider(t_x502_hnd hnd, uint32_t adc_freq_div) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err)  {
        if ((adc_freq_div==0) || (adc_freq_div > X502_ADC_FREQ_DIV_MAX)) {
            err = X502_ERR_INVALID_ADC_FREQ_DIV;
        } else {
            hnd->set.adc_freq_div = adc_freq_div;
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetAdcInterframeDelay(t_x502_hnd hnd, uint32_t delay) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err && (delay > X502_ADC_INTERFRAME_DELAY_MAX))
        err = X502_ERR_INVALID_INTERFRAME_DELAY;

    if (!err) {
        hnd->set.adc_frame_delay = delay;
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetDinFreqDivider(t_x502_hnd hnd, uint32_t din_freq_div) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err) {
        if ((din_freq_div==0) || (din_freq_div > X502_DIN_FREQ_DIV_MAX)) {
            err = X502_ERR_INVALID_ADC_FREQ_DIV;
        } else {
            hnd->set.din_freq_div = din_freq_div;
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetOutFreqDivider(t_x502_hnd hnd, uint32_t out_freq_div) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err)  {
        /* Проверяем, поддерживается ли возможность установить значение, отличное от стандартного */
        if (out_freq_div != X502_OUT_FREQ_DIV_DEFAULT) {
            err = X502_CheckFeature(hnd, X502_FEATURE_OUT_FREQ_DIV);
        }
    }

    if (!err) {
        if ((out_freq_div<X502_OUT_FREQ_DIV_MIN) || (out_freq_div > X502_OUT_FREQ_DIV_MAX)) {
            err = X502_ERR_INVALID_OUT_FREQ_DIV;
        } else {
            hnd->set.out_freq_div = out_freq_div;
        }
    }
    return err;
}






X502_EXPORT(int32_t) X502_SetMode(t_x502_hnd hnd, uint32_t mode) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err && (mode==X502_MODE_DSP) &&
            !(hnd->info.devflags & X502_DEVFLAGS_BF_PRESENT)) {
        err = X502_ERR_BF_NOT_PRESENT;
    }

    if (!err) {
        uint32_t val;
        err = hnd->iface_hnd->fpga_reg_read(hnd, X502_REGS_BF_CTL, &val);
        if (!err) {
            val &= ~(X502_REGBIT_BF_CTL_CLK_DIV_Msk | X502_REGBIT_BF_CTL_DBG_MODE_Msk | X502_REGBIT_BF_CTL_DSP_MODE_Msk);
            if (mode==X502_MODE_DSP) {
                /* в DSP режиме сброс должен быть снят с blackfin, иначе не будут работать
                   команды по HostDMA */
                val |= X502_REGBIT_BF_CTL_DSP_MODE_Msk | X502_REGBIT_BF_CTL_BF_RESET_Msk;
            } else if (mode==X502_MODE_DEBUG) {
                val |= X502_REGBIT_BF_CTL_DBG_MODE_Msk;
            } else if (mode!=X502_MODE_FPGA) {
                err = X502_ERR_INVALID_MODE;
            }
        }

        if (!err)
            err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_BF_CTL, val);

        /* при переходе в режим DSP сбрасываем автомат HDMA */
        if (!err && (mode==X502_MODE_DSP))
            err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_BF_CMD, X502_BF_CMD_HDMA_RST);

        if (!err)
            hnd->mode = mode;
    }
    return err;
}

X502_EXPORT(int32_t) X502_GetMode(t_x502_hnd hnd, uint32_t* mode) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (mode==NULL))
        err = X502_ERR_INVALID_POINTER;

    if (!err) {
        *mode = hnd->mode;
    }
    return err;
}

X502_EXPORT(int32_t) X502_CalcAdcFreq(double ref_freq, uint32_t lch_cnt, double *f_acq,
                                      double *f_frame, uint32_t *adc_freq_div, uint32_t *adc_frame_delay) {
    int32_t err = (f_acq==NULL) ? X502_ERR_INVALID_POINTER : X502_ERR_OK;
    if (err == X502_ERR_OK) {
        uint32_t cur_adc_freq_div, cur_frame_delay = 0;

        double set_freq = *f_acq;
        if (set_freq <= 0)
            set_freq = ref_freq;
        cur_adc_freq_div = (uint32_t)(ref_freq/set_freq+0.49);
        if (cur_adc_freq_div == 0)
            cur_adc_freq_div = 1;
        if (cur_adc_freq_div > X502_ADC_FREQ_DIV_MAX)
            cur_adc_freq_div = X502_ADC_FREQ_DIV_MAX;
        set_freq = ref_freq/cur_adc_freq_div;

        *f_acq = set_freq;

        if (f_frame==NULL) {
            cur_frame_delay = 0;
        } else {
            if (lch_cnt == 0)
                lch_cnt = 1;

            if (*f_frame <= 0) {
                cur_frame_delay = 0;
            } else {
                int32_t frame_div = (int32_t)((ref_freq/(*f_frame)
                                    - lch_cnt*ref_freq/set_freq)+0.49);

                cur_frame_delay = frame_div <=0 ? 0 :
                          frame_div > X502_ADC_INTERFRAME_DELAY_MAX ?
                          X502_ADC_INTERFRAME_DELAY_MAX : frame_div;
            }
            *f_frame = 1./(lch_cnt/set_freq +
                           cur_frame_delay/ref_freq);
        }

        if (adc_freq_div != NULL)
            *adc_freq_div = cur_adc_freq_div;
        if (adc_frame_delay != NULL)
            *adc_frame_delay = cur_frame_delay;
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetAdcFreq(t_x502_hnd hnd, double *f_acq, double *f_frame) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err) {
        double ref_freq;
        X502_GetRefFreqValue(hnd, &ref_freq);
        err = X502_CalcAdcFreq(ref_freq, hnd->set.lch_cnt, f_acq, f_frame,
                               &hnd->set.adc_freq_div, &hnd->set.adc_frame_delay);
    }
    return err;
}

X502_EXPORT(int32_t) X502_GetAdcFreq(t_x502_hnd hnd, double *f_acq, double *f_frame) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (f_acq == NULL) && (f_frame==NULL))
        err = X502_ERR_INVALID_POINTER;

    if (!err) {
        double ref_freq, set_freq;

        X502_GetRefFreqValue(hnd, &ref_freq);

        set_freq = ref_freq/hnd->set.adc_freq_div;
        if (f_acq!=NULL) {
            *f_acq = set_freq;
        }

        if (f_frame!=NULL) {
            *f_frame = 1./(hnd->set.lch_cnt/set_freq +
                           hnd->set.adc_frame_delay/ref_freq);
        }
    }
    return err;
}


X502_EXPORT(int32_t) X502_CalcDinFreq(double ref_freq, double *f_din, uint32_t *result_freq_div) {
    int32_t err = f_din==NULL ? X502_ERR_INVALID_POINTER : X502_ERR_OK;
    if (err == X502_ERR_OK) {
        uint32_t freq_div;
        double set_freq = *f_din;

        if (set_freq<=0)
            set_freq = ref_freq;
        freq_div = (uint32_t)(ref_freq/set_freq+0.49);
        if (freq_div == 0)
            freq_div = 1;
        if (freq_div > X502_DIN_FREQ_DIV_MAX)
            freq_div = X502_DIN_FREQ_DIV_MAX;
        set_freq = ref_freq/freq_div;
        *f_din = set_freq;

        if (result_freq_div != NULL)
            *result_freq_div = freq_div;
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetDinFreq(t_x502_hnd hnd, double *f_din) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;
    if (!err) {
        double ref_freq;
        X502_GetRefFreqValue(hnd, &ref_freq);
        err = X502_CalcDinFreq(ref_freq, f_din, &hnd->set.din_freq_div);
    }
    return err;
}


X502_EXPORT(int32_t) X502_CalcOutFreq(double ref_freq, double *f_dout, uint32_t *result_freq_div) {
    int32_t err = f_dout==NULL ? X502_ERR_INVALID_POINTER : X502_ERR_OK;
    if (err == X502_ERR_OK) {
        uint32_t out_freq_div;
        double set_freq = *f_dout;
        if (set_freq<=0)
            set_freq = ref_freq;

        out_freq_div = (uint32_t)(ref_freq/set_freq+0.49);
        if (out_freq_div < X502_OUT_FREQ_DIV_MIN)
            out_freq_div = X502_OUT_FREQ_DIV_MIN;
        if (out_freq_div > X502_OUT_FREQ_DIV_MAX)
            out_freq_div = X502_OUT_FREQ_DIV_MAX;

        set_freq = ref_freq/out_freq_div;
        *f_dout = set_freq;

        if (result_freq_div != NULL)
            *result_freq_div = out_freq_div;
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetOutFreq(t_x502_hnd hnd, double *f_dout) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;
    if (!err && (f_dout==NULL))
        err = X502_ERR_INVALID_POINTER;

    if (!err) {
        double ref_freq;

        X502_GetRefFreqValue(hnd, &ref_freq);
        /* Если не поддерживается возможность установки нестандартного делителя, то
           всегда устанавливаем стандартный */
        if (X502_CheckFeature(hnd, X502_FEATURE_OUT_FREQ_DIV) != X502_ERR_OK) {
            hnd->set.out_freq_div = X502_OUT_FREQ_DIV_DEFAULT;
            *f_dout = ref_freq/hnd->set.out_freq_div;
        } else {
            err = X502_CalcOutFreq(ref_freq, f_dout, &hnd->set.out_freq_div);
        }
    }
    return err;
}




X502_EXPORT(int32_t) X502_SetRefFreq(t_x502_hnd hnd, uint32_t freq) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err)
        hnd->set.ref_freq = freq;
    return err;
}



X502_EXPORT(int32_t) X502_SetExtRefFreqValue(t_x502_hnd hnd, double freq) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;
    if (!err)
        hnd->set.ext_ref_freq = freq;
    return err;
}

X502_EXPORT(int32_t) X502_GetRefFreqValue(t_x502_hnd hnd, double *freq) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);

    if (!err) {
        if ((hnd->set.sync_mode == X502_SYNC_INTERNAL) ||
             !(hnd->set.ext_ref_freq > 0)) {
            *freq = hnd->set.ref_freq;
        } else {
            *freq = hnd->set.ext_ref_freq;
        }
    }
    return err;
}



X502_EXPORT(int32_t) X502_SetSyncMode(t_x502_hnd hnd, uint32_t sync_mode) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err)
        err = CHECK_SYNC_MODE(sync_mode);

    if (!err) {
        hnd->set.sync_mode = sync_mode;
    }
    return err;
}

X502_EXPORT(int32_t) X502_SetSyncStartMode(t_x502_hnd hnd, uint32_t sync_start_mode) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err)
        err = CHECK_SYNC_MODE(sync_start_mode);

    if (!err) {
        hnd->set.sync_start_mode = sync_start_mode;
    }

    return err;
}


X502_EXPORT(int32_t) X502_Configure(t_x502_hnd hnd, uint32_t flags) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err) {
        /* Проверяем правильность установленной опорной частоты.
           Для внутренней может быть только одно из двух значений,
           для внешней главное, чтобы не принимало требуемой */
        if ((hnd->set.sync_mode==X502_SYNC_INTERNAL)
                && (hnd->set.ref_freq!=X502_REF_FREQ_2000KHZ)
                &&(hnd->set.ref_freq!=X502_REF_FREQ_1500KHZ)) {
            err = X502_ERR_INVALID_REF_FREQ;
        } else if (hnd->set.ext_ref_freq > 2000000) {
            err = X502_ERR_INVALID_REF_FREQ;
        }
    }

    if (!err) {
        if (hnd->mode == X502_MODE_FPGA) {
            uint32_t ch;
            /* записываем логическую таблицу */
            for (ch=0; (ch < hnd->set.lch_cnt) && !err; ch++) {
                uint32_t wrd = ((hnd->set.lch[ch].ch & 0xF) << 3) | (hnd->set.lch[ch].range & 0x7);

                if (hnd->set.lch[ch].mode == X502_LCH_MODE_ZERO) {
                    wrd |= (3 << 7);
                } else if (hnd->set.lch[ch].mode == X502_LCH_MODE_COMM) {
                    wrd |= (hnd->set.lch[ch].ch & 0x10 ? 2 : 1) << 7;
                }

                if (hnd->set.lch[ch].avg) {
                    uint32_t avg = hnd->set.lch[ch].avg;
                    if (avg > hnd->set.adc_freq_div)
                        avg = hnd->set.adc_freq_div;
                    wrd |= ((avg-1) & 0x7F) << 9;
                }

                err = hnd->iface_hnd->fpga_reg_write(hnd, (X502_REGS_IOHARD_LTABLE +
                                                      hnd->set.lch_cnt - 1 - ch) & 0xFFFF, wrd);
            }
            if (!err) {
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_LCH_CNT,
                                                 hnd->set.lch_cnt-1);
            }

            if (!err) {
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_ADC_FREQ_DIV,
                                                 hnd->set.adc_freq_div-1);
            }

            if (!err) {
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOARITH_ADC_FREQ_DIV,
                                                 hnd->set.adc_freq_div-1);
            }


            if (!err) {
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_ADC_FRAME_DELAY,
                                                 hnd->set.adc_frame_delay);
            }

            if (!err) {
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_IO_MODE,
                                                 (SYNC_FREQ_CODE(hnd->set.sync_mode) & 0x7)
                                              | ((hnd->set.sync_start_mode&0x7)<<3)
                                              | ((hnd->set.ref_freq==X502_REF_FREQ_2000KHZ ?
                                                      0 : 2) << 7)
                                              | (((hnd->set.out_freq_div-1)&0x3FF)<<9));
            }

            if (!err) {
                err = hnd->iface_hnd->fpga_reg_write(hnd, X502_REGS_IOHARD_DIGIN_FREQ_DIV,
                                                 hnd->set.din_freq_div-1);
            }
        } else if (hnd->mode == X502_MODE_DSP) {
            uint32_t ch;
            err = x502_bf_set_par(hnd, L502_BF_PARAM_LCH_CNT, &hnd->set.lch_cnt, 1);
            for (ch=0; !err && (ch < hnd->set.lch_cnt); ch++) {
                uint32_t ch_par[] = {ch, hnd->set.lch[ch].ch,
                                     hnd->set.lch[ch].mode,
                                     hnd->set.lch[ch].range,
                                     hnd->set.lch[ch].avg > hnd->set.adc_freq_div ?
                                        hnd->set.adc_freq_div : hnd->set.lch[ch].avg} ;
                err = x502_bf_set_par(hnd, L502_BF_PARAM_LCH, ch_par,
                                        sizeof(ch_par)/sizeof(ch_par[0]));
            }
            if (!err) {
                err = x502_bf_set_par(hnd, L502_BF_PARAM_ADC_FREQ_DIV,
                                  &hnd->set.adc_freq_div, 1);
            }
            if (!err) {
                err = x502_bf_set_par(hnd, L502_BF_PARAM_ADC_FRAME_DELAY,
                                      &hnd->set.adc_frame_delay, 1);
            }
            if (!err) {
                err = x502_bf_set_par(hnd, L502_BF_PARAM_REF_FREQ_SRC,
                                       &hnd->set.ref_freq, 1);
            }
            if (!err) {
                uint32_t sync_code = SYNC_FREQ_CODE(hnd->set.sync_mode);
                err = x502_bf_set_par(hnd, L502_BF_PARAM_SYNC_MODE,
                                      &sync_code, 1);
            }
            if (!err) {
                err = x502_bf_set_par(hnd, L502_BF_PARAM_SYNC_START_MODE,
                                      &hnd->set.sync_start_mode, 1);
            }
            if (!err) {
                err = x502_bf_set_par(hnd, L502_BF_PARAM_DAC_FREQ_DIV,
                                      &hnd->set.out_freq_div, 1);
            }
            if (!err)
            {
                err = x502_bf_set_par(hnd, L502_BF_PARAM_DIN_FREQ_DIV,
                                      &hnd->set.din_freq_div, 1);
            }

            if (!err) {
                err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_CONFIGURE, 0, NULL,
                                   0, NULL, 0, X502_BF_CMD_DEFAULT_TOUT, NULL);
            }
        } else {
            err = X502_ERR_INVALID_MODE;
        }
    }
    return err;
}






X502_EXPORT(int32_t) X502_SetAdcCoef(t_x502_hnd hnd, uint32_t range, double k, double offs) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);

    if (!err && (hnd->flags & PRIV_FLAGS_STREAM_RUN))
        err = X502_ERR_STREAM_IS_RUNNING;

    if (!err) {
        if (range >= X502_ADC_RANGE_CNT) {
            err = X502_ERR_INVALID_LCH_RANGE;
        }
    }

    if (!err) {
        uint32_t kval = (uint32_t)(k*0x400000);
        uint32_t offs_val = (uint32_t)(-offs);

        if (hnd->mode == X502_MODE_FPGA) {
            err = hnd->iface_hnd->fpga_reg_write(hnd, f_regadd_k[range], kval);
            if (!err)
                err = hnd->iface_hnd->fpga_reg_write(hnd, f_regadd_offs[range], offs_val);
        } else if (hnd->mode == X502_MODE_DSP) {
            uint32_t wrds[3] = {range, kval, offs_val};
            err = x502_bf_set_par(hnd, L502_BF_PARAM_ADC_COEF, wrds, 3);
        } else {
            err = X502_ERR_INVALID_MODE;
        }
    }

    if (!err) {
        hnd->info.cbr.adc[range].k = k;
        hnd->info.cbr.adc[range].offs = offs;
    }

    return err;
}


X502_EXPORT(int32_t) X502_GetAdcCoef(t_x502_hnd hnd, uint32_t range, double* k, double* offs) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err) {
        if (range >= X502_ADC_RANGE_CNT) {
            err = X502_ERR_INVALID_LCH_RANGE;
        }
    }

    if (!err) {
        *k = hnd->info.cbr.adc[range].k;
        *offs = hnd->info.cbr.adc[range].offs;
    }
    return err;
}



X502_EXPORT(int32_t) X502_SetDacCoef(t_x502_hnd hnd, uint32_t ch, double k, double offs) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);

    if (!err && (ch!=X502_DAC_CH1) && (ch!=X502_DAC_CH2)) {
        err = X502_ERR_INVALID_DAC_CHANNEL;
    }

    if (!err) {
        hnd->info.cbr.dac[ch].offs = offs;
        hnd->info.cbr.dac[ch].k = k;
    }
    return err;
}

X502_EXPORT(int32_t) X502_GetDacCoef(t_x502_hnd hnd, uint32_t ch, double* k, double* offs) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (!err && (ch!=X502_DAC_CH1) && (ch!=X502_DAC_CH2)) {
        err = X502_ERR_INVALID_DAC_CHANNEL;
    }

    if (!err) {
        if (offs!=NULL)
            *offs = hnd->info.cbr.dac[ch].offs;

        if (k!=NULL)
            *k = hnd->info.cbr.dac[ch].k;
    }
    return err;
}
