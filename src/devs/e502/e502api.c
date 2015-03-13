#include "e502api_private.h"
#include <stdlib.h>

#define BF_LOAD_TOUT  20000

int32_t e502_iface_fpga_read(t_x502_hnd hnd, uint16_t addr, uint32_t *val) {
    return hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_FPGA_REG_READ, addr, NULL, 0,
                                 val, sizeof(*val), NULL, 0);
}

int32_t e502_iface_fpga_write(t_x502_hnd hnd, uint16_t addr, uint32_t val) {
    return hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_FPGA_REG_WRITE, addr, &val, sizeof(val),
                                 NULL, 0, NULL,0);
}


int32_t e502_iface_stream_running(t_x502_hnd hnd, uint32_t ch, int32_t* running) {
    int32_t err;
    uint8_t l_run;
    err = hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_STREAM_IS_RUNNING, (ch << 16),
                                NULL, 0, &l_run, 1, NULL, 0);
    if (!err && (running!=NULL)) {
        *running = l_run;
    }
    return err;
}


int32_t e502_iface_bf_mem_block_rd(t_x502_hnd hnd, uint32_t addr, uint32_t *block, uint32_t size) {
    return  hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_BF_MEM_READ, addr, NULL, 0,
                                  block, size*4, NULL, 0);
}

int32_t e502_iface_bf_mem_block_wr(t_x502_hnd hnd, uint32_t addr, const uint32_t *block, uint32_t size) {
   return  hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_BF_MEM_WRITE, addr, block, size*4,
                                 NULL, 0, NULL, 0);
}

int32_t e502_iface_bf_firm_load(t_x502_hnd hnd, FILE* f) {
    int32_t err = 0;
    uint8_t *buf = malloc(hnd->iface->ioctl_max_data_size);
    long size, done=0;

    if (buf == NULL)
        err = X502_ERR_MEMORY_ALLOC;

    //определяем размер файла
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);


    /* данные записываем блоками по L502_BF_REQ_DATA_SIZE */
    while (!err && (size!=done)) {
        unsigned block_size = size-done;
        if (block_size > hnd->iface->ioctl_max_data_size)
            block_size = hnd->iface->ioctl_max_data_size;

        if (fread(buf, 1, block_size, f) != block_size) {
            err = X502_ERR_LDR_FILE_READ;
        }

        if (!err) {
            err = hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_FIRM_BUF_WRITE,
                                        done, buf, block_size, NULL, 0, NULL, 0);
        }

        if (!err) {
            done += block_size;
        }
    }

    if (!err) {
        err = hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_BF_FIRM_LOAD, 0,
                                    NULL, 0, NULL, 0, NULL, BF_LOAD_TOUT);
    }

    free(buf);

    return err;
}


int32_t e502_iface_flash_rd(t_x502_hnd hnd, uint32_t addr, uint8_t* data, uint32_t size) {
    return  hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_FLASH_RD, addr, NULL, 0,
                                  data, size, NULL, 0);
}

int32_t e502_iface_flash_wr(t_x502_hnd hnd, uint32_t addr, const uint8_t* data, uint32_t size) {
    return  hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_FLASH_WR, addr, data, size,
                                  NULL, 0, NULL, 0);
}

int32_t e502_iface_flash_erase(t_x502_hnd hnd, uint32_t addr, uint32_t size) {
    return hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_FLASH_ERASE, addr, &size, sizeof(size), NULL, 0, NULL, 0);
}

int32_t e502_iface_flash_set_prot(t_x502_hnd hnd, uint32_t prot, const uint8_t* prot_data, uint32_t size) {
    return hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_FLASH_SET_PORT, prot, prot_data, size, NULL, 0, NULL, 0);
}

int32_t e502_iface_reload_dev_info(t_x502_hnd hnd) {
    return hnd->iface->gen_ioctl(hnd, E502_CM4_CMD_FLASH_RELOAD_INFO, 0, NULL, 0, NULL, 0, NULL, 0);
}
