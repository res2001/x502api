#include "l502api_private.h"
#include "ltimer.h"
#include "l502_fpga_regs.h"

#define L502_EEPROM_BIG_SECTOR_SIZE    (64*1024)
#define L502_EEPROM_SMALL_SECTOR_SIZE  (4*1024)

#define L502_FLASH_WRITE_TOUT  500
#define L502_FLASH_ERASE_TOUT  500

/** биты регистра статуса */
typedef enum {
    SST25_STATUS_BUSY   = 0x01,
    SST25_STATUS_WEL    = 0x02,
    SST25_STATUS_BP0    = 0x04,
    SST25_STATUS_BP1    = 0x08,
    SST25_STATUS_BP2    = 0x10,
    SST25_STATUS_BP3    = 0x20,
    SST25_STATUS_AAI    = 0x40,
    SST25_STATUS_BPL    = 0x80
} t_sst25_status_bits;

static uint8_t f_prot_bits[] = {
    SST25_STATUS_BP2 | SST25_STATUS_BP1 | SST25_STATUS_BP0,
    SST25_STATUS_BP2 | SST25_STATUS_BP0,
    SST25_STATUS_BP2,
    SST25_STATUS_BP0,
    0
};



static LINLINE int32_t f_eeprom_rd_status(t_x502_hnd hnd, uint8_t* stat) {
    uint32_t val;
    int32_t err = l502_port_fpga_reg_read(hnd, L502_REGS_EEPROM_RD_STATUS, &val);
    if (err == X502_ERR_OK)
        *stat = (val>>24)&0xFF;
    return err;
}

static LINLINE int32_t f_eeprom_wr_status(t_x502_hnd hnd, uint8_t stat) {
    int32_t err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_WR_STATUS_EN, 1);
    if (err == X502_ERR_OK) {
        err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_WR_STATUS, stat);
    }
    return err;
}


static int32_t f_eeprom_wr_byte(t_x502_hnd hnd, uint32_t addr, uint8_t val) {
    int32_t err = X502_ERR_OK;
    t_ltimer tmr;
    uint8_t stat = SST25_STATUS_BUSY;

    /* разрешаем запись в EEPROM */
    err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_WR_EN, 1);
    if (err == X502_ERR_OK) {
        err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_WR_BYTE, ((addr & 0xFFFFFF) << 8) | val);
        if (err != X502_ERR_OK)
            l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_WR_DIS, 1);
    }

    if (err == X502_ERR_OK)
        ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(L502_FLASH_WRITE_TOUT));

    /* Ожидаем завершения записи */
    while ((err == X502_ERR_OK) && (stat & SST25_STATUS_BUSY) && !ltimer_expired(&tmr)) {
        err = f_eeprom_rd_status(hnd, &stat);
    }

    if ((err == X502_ERR_OK) && (stat & SST25_STATUS_BUSY)) {
        err = X502_ERR_FLASH_WRITE_TOUT;
    }
    return err;
}





int32_t l502_iface_flash_rd(t_x502_hnd hnd, uint32_t addr, uint8_t* data, uint32_t size) {
    int32_t err = X502_ERR_OK;
    uint32_t val;

    err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_SET_RD_ADDR, (addr & 0xFFFFFF) << 8);
    if (err == X502_ERR_OK)
        err = l502_port_fpga_reg_read(hnd, L502_REGS_EEPROM_RD_DWORD, &val);
    if (err == X502_ERR_OK) {
        unsigned int i;
        for (i=0; (i < sizeof(val)) && size; i++, size--) {
            *data++ = val & 0xFF;
            val >>= 8;
        }
    }
    return err;
}

int32_t l502_iface_flash_wr(t_x502_hnd hnd, uint32_t addr, const uint8_t* data, uint32_t size) {
    uint32_t i;
    int32_t err = X502_ERR_OK;
    for (i=0; (i < size) && (err == X502_ERR_OK); i++) {
        err = f_eeprom_wr_byte(hnd, addr+i, data[i]);
    }
    return err;
}

int32_t l502_iface_flash_erase(t_x502_hnd hnd, uint32_t addr, uint32_t size) {
    int32_t err = X502_ERR_OK;
    if (((addr & (L502_EEPROM_SMALL_SECTOR_SIZE-1)) ||
             (size & (L502_EEPROM_SMALL_SECTOR_SIZE-1)))) {
        err = X502_ERR_FLASH_SECTOR_BOUNDARY;
    }

    while((size != 0) && (err == X502_ERR_OK)) {
        uint32_t er_size;
        /* разрешаем запись в EEPROM */
        err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_WR_EN, 1);
        if (err == X502_ERR_OK) {
            uint8_t stat = SST25_STATUS_BUSY;
            t_ltimer tmr;
            /* проверяем - можем ли стереть целиком большой сектор или
               придется писать в мелкий */
            if ((size >= L502_EEPROM_BIG_SECTOR_SIZE) &&
                                !(size & (L502_EEPROM_BIG_SECTOR_SIZE-1))) {
                er_size = L502_EEPROM_BIG_SECTOR_SIZE;
                err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_ERASE_64K, addr<<8);
            } else {
                er_size = L502_EEPROM_SMALL_SECTOR_SIZE;
                err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_ERASE_4K, addr<<8);
            }

            if (err == X502_ERR_OK)
                ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(L502_FLASH_ERASE_TOUT));

            /* ожидаем завершения стирания */
            while ((err == X502_ERR_OK) && (stat & SST25_STATUS_BUSY) && !ltimer_expired(&tmr)) {
                err = f_eeprom_rd_status(hnd, &stat);
            }

            if ((err == X502_ERR_OK) && (stat & SST25_STATUS_BUSY)) {
                err = X502_ERR_FLASH_ERASE_TOUT;
            }

            /* запрещаем запись, если произошла ошибка. при успешном стирании
               запись будут запрещена атоматически */
            if (err != X502_ERR_OK)
                l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_WR_EN, 0);

            if (err == X502_ERR_OK) {
                addr += er_size;
                size -= er_size;
            }
        }
    }
    return err;
}

int32_t l502_iface_flash_set_prot(t_x502_hnd hnd, uint32_t prot, const uint8_t* prot_data, uint32_t size) {
    int32_t err = X502_ERR_OK;
    uint16_t prot_code = 0;
    if (size == 2) {
        prot_code = ((uint16_t)prot_data[1] << 8) | prot_data[0];
    }

    err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_WR_STATUS_EN, 1);
    if ((err == X502_ERR_OK) && (prot_code != 0))
        err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_HARD_WR_STATUS_EN, prot_code);
    if (err == X502_ERR_OK)
       err = l502_port_fpga_reg_write(hnd, L502_REGS_EEPROM_WR_STATUS,  f_prot_bits[prot]);
    return err;
}
