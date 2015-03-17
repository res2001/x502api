#include "x502api_private.h"
#include "x502_eeprom.h"
#include "fast_crc.h"
#include <stdlib.h>
#include <string.h>

#define X502_CHECK_ADDR(addr, size) ((addr+size)>X502_EEPROM_SIZE ? \
    X502_ERR_FLASH_INVALID_ADDR : (size==0) ? X502_ERR_FLASH_INVALID_SIZE : X502_ERR_OK)


X502_EXPORT(int32_t) X502_FlashRead(t_x502_hnd hnd, uint32_t addr, uint8_t* data,
                                    uint32_t size) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if (err==X502_ERR_OK) {
        err = X502_CHECK_ADDR(addr, size);
    }
    if (err==X502_ERR_OK) {

        for ( ; (size!=0) && (err==X502_ERR_OK); ) {
            uint32_t rd_size = size;
            if (rd_size > hnd->iface->flash_rd_size)
                rd_size = hnd->iface->flash_rd_size;

            err = hnd->iface->flash_rd(hnd, addr, data, rd_size);
            if (err==X502_ERR_OK) {
                data+=rd_size;
                addr+=rd_size;
                size-=rd_size;
            }
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_FlashWrite(t_x502_hnd hnd, uint32_t addr,
                                     const uint8_t* data, uint32_t size) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if (err==X502_ERR_OK) {
        err = X502_CHECK_ADDR(addr, size);
    }
    if (err==X502_ERR_OK) {
        uint32_t wr_size = size;
        if (wr_size > hnd->iface->flash_wr_size)
            wr_size = hnd->iface->flash_wr_size;

        err = hnd->iface->flash_wr(hnd, addr, data, wr_size);
        if (err==X502_ERR_OK) {
            data+=wr_size;
            addr+=wr_size;
            size-=wr_size;
        }
    }
    return err;
}

X502_EXPORT(int32_t) X502_FlashErase(t_x502_hnd hnd, uint32_t addr, uint32_t size) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if (err==X502_ERR_OK) {
        err = X502_CHECK_ADDR(addr, size);
    }
    if (err==X502_ERR_OK) {
        err = hnd->iface->flash_erase(hnd, addr, size);
    }
    return err;
}

X502_EXPORT(int32_t) X502_FlashWriteEnable(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if (err==X502_ERR_OK) {
        err = hnd->iface->flash_set_prot(hnd, X502_EEPROM_PROT_WR_USER, NULL, 0);
    }
    return err;
}

X502_EXPORT(int32_t) X502_FlashWriteDisable(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if (err==X502_ERR_OK) {
        err = hnd->iface->flash_set_prot(hnd, X502_EEPROM_PROT_ALL, NULL, 0);
    }
    return err;
}


X502_EXPORT(int32_t) X502_FlashSetProtection(t_x502_hnd hnd, uint32_t prot, uint8_t *prot_data, uint32_t prot_data_size) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if (err==X502_ERR_OK) {
        err = hnd->iface->flash_set_prot(hnd, prot, prot_data, prot_data_size);
    }
    return err;
}


/** Функция проверяет правильность информации о устройстве, записанной в EEPROM.
    При наличии верной инофрмации, из EEPROM считывается название устройства и
    серийный номер, а так же, при наличии, калибровочные коэффициенты */
int x502_check_eeprom(t_x502_hnd hnd) {
    int err;
    uint32_t sign, size;

    /* проверяем признак правильного описателя в EEPROM и его размер */
    err = X502_FlashRead(hnd, X502_EEPROM_ADDR_DESCR, (uint8_t*)&sign, (uint32_t)sizeof(sign));
    if (err == X502_ERR_OK)
        err = X502_FlashRead(hnd, X502_EEPROM_ADDR_DESCR+sizeof(sign), (uint8_t*)&size, (uint32_t)sizeof(size));

    if (err == X502_ERR_OK) {
        if ((sign == X502_EEPROM_SIGN) && (size >= X502_DESCR_MIN_SIZE) && (size <= X502_DESCR_MAX_SIZE)) {
            t_x502_descr* pdescr = (t_x502_descr*)malloc(size);
            /* читаем весь описатель */
            if (pdescr!=NULL) {
                err = X502_FlashRead(hnd, X502_EEPROM_ADDR_DESCR, (uint8_t*)pdescr, size);
            } else {
                err = X502_ERR_MEMORY_ALLOC;
            }

            /* сверяем crc */
            if (err == X502_ERR_OK) {
                uint32_t crc, crc2;
                crc = CRC32_Block8(0, (uint8_t*)pdescr, (uint32_t)(size-sizeof(crc)));
                crc2 = *(uint32_t*)(&((uint8_t*)pdescr)[pdescr->hdr.size-4]);
                if (crc == crc2) {

                    hnd->info.devflags |= X502_DEVFLAGS_FLASH_DATA_VALID;
                    memcpy(hnd->info.serial, pdescr->hdr.serial, sizeof(pdescr->hdr.serial));
                    memcpy(hnd->info.name, pdescr->hdr.name, sizeof(pdescr->hdr.name));
                    memcpy(hnd->info.factory_mac, pdescr->hdr.factory_mac, sizeof(pdescr->hdr.factory_mac));

                    if ((pdescr->cbr_adc.hdr.cbr_sign == X502_EEPROM_CBR_SIGN) &&
                            (pdescr->cbr_adc.hdr.format == X502_EEPROM_CBR_FROMAT) &&
                            (pdescr->cbr_adc.hdr.src == X502_EEPROM_CBR_SRC_ADC) &&
                            (pdescr->cbr_adc.hdr.range_cnt == X502_ADC_RANGE_CNT) &&
                            (pdescr->cbr_adc.hdr.channel_cnt == 1)) {
                        unsigned int i;

                        hnd->info.devflags |= X502_DEVFLAGS_FLASH_ADC_CALIBR_VALID;
                        for (i=0; (i < pdescr->cbr_adc.hdr.range_cnt) &&
                             (i < X502_ADC_RANGE_CNT) && (err == X502_ERR_OK); i++) {
                            err = X502_SetAdcCoef(hnd, i, pdescr->cbr_adc.coefs[i].k,
                                                    pdescr->cbr_adc.coefs[i].offs);
                        }
                    }


                    if ((pdescr->cbr_dac.hdr.cbr_sign == X502_EEPROM_CBR_SIGN) &&
                            (pdescr->cbr_dac.hdr.format == X502_EEPROM_CBR_FROMAT) &&
                            (pdescr->cbr_dac.hdr.src == X502_EEPROM_CBR_SRC_DAC) &&
                            (pdescr->cbr_dac.hdr.range_cnt == 1) &&
                            (pdescr->cbr_dac.hdr.channel_cnt == X502_DAC_CH_CNT)) {
                        unsigned int i;

                        hnd->info.devflags |= X502_DEVFLAGS_FLASH_DAC_CALIBR_VALID;
                        for (i=0; (i < pdescr->cbr_dac.hdr.channel_cnt) &&
                             (i < X502_ADC_RANGE_CNT) && (err == X502_ERR_OK); i++) {
                            err = X502_SetDacCoef(hnd, i, pdescr->cbr_dac.coefs[i].k,
                                                    pdescr->cbr_dac.coefs[i].offs);
                        }
                    }
                }
            }
            free(pdescr);
        }
    }
    return err;
}


X502_EXPORT(int32_t) X502_ReloadDevInfo(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND_OPEND(hnd);
    if ((err==X502_ERR_OK) && (hnd->iface->reload_dev_info!=NULL))
        err = hnd->iface->reload_dev_info(hnd);
    if (err==X502_ERR_OK)
        err = x502_check_eeprom(hnd);
    return err;
}
