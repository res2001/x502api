#include "e502api_private.h"
#include <stdlib.h>
#include <string.h>

#define BF_LOAD_TOUT  20000

int32_t e502_iface_fpga_read(t_x502_hnd hnd, uint32_t addr, uint32_t *val) {
    return hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_FPGA_REG_READ, addr, NULL, 0,
                                 val, sizeof(*val), NULL, 0);
}

int32_t e502_iface_fpga_write(t_x502_hnd hnd, uint32_t addr, uint32_t val) {
    return hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_FPGA_REG_WRITE, addr, &val, sizeof(val),
                                 NULL, 0, NULL,0);
}


int32_t e502_iface_stream_running(t_x502_hnd hnd, uint32_t ch, int32_t* running) {
    int32_t err;
    uint8_t l_run;
    err = hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_STREAM_IS_RUNNING, (ch << 16),
                                NULL, 0, &l_run, 1, NULL, 0);
    if (!err && (running!=NULL)) {
        *running = l_run;
    }
    return err;
}


int32_t e502_iface_bf_mem_block_rd(t_x502_hnd hnd, uint32_t addr, uint32_t *block, uint32_t size) {
    return hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_BF_MEM_READ, addr, NULL, 0,
                                  block, size*4, NULL, 0);
}

int32_t e502_iface_bf_mem_block_wr(t_x502_hnd hnd, uint32_t addr, const uint32_t *block, uint32_t size) {
   return  hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_BF_MEM_WRITE, addr, block, size*4,
                                 NULL, 0, NULL, 0);
}

int32_t e502_iface_bf_firm_load(t_x502_hnd hnd, FILE* f) {
    int32_t err = 0;
    uint8_t *buf = malloc(hnd->iface_hnd->ioctl_max_data_size);
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
        if (block_size > hnd->iface_hnd->ioctl_max_data_size)
            block_size = hnd->iface_hnd->ioctl_max_data_size;

        if (fread(buf, 1, block_size, f) != block_size) {
            err = X502_ERR_LDR_FILE_READ;
        }

        if (!err) {
            err = hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_FIRM_BUF_WRITE,
                                        done, buf, block_size, NULL, 0, NULL, 0);
        }

        if (!err) {
            done += block_size;
        }
    }

    if (!err) {
        err = hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_BF_FIRM_LOAD, 0,
                                    NULL, 0, NULL, 0, NULL, BF_LOAD_TOUT);
    }

    free(buf);

    return err;
}


int32_t e502_iface_flash_rd(t_x502_hnd hnd, uint32_t addr, uint8_t* data, uint32_t size) {
    return  hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_FLASH_RD, addr, NULL, 0,
                                  data, size, NULL, 0);
}

int32_t e502_iface_flash_wr(t_x502_hnd hnd, uint32_t addr, const uint8_t* data, uint32_t size) {
    return  hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_FLASH_WR, addr, data, size,
                                  NULL, 0, NULL, 0);
}

int32_t e502_iface_flash_erase(t_x502_hnd hnd, uint32_t addr, uint32_t size) {
    return hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_FLASH_ERASE, addr, &size, sizeof(size), NULL, 0, NULL, 0);
}

int32_t e502_iface_flash_set_prot(t_x502_hnd hnd, uint32_t prot, const uint8_t* prot_data, uint32_t size) {
    return hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_FLASH_SET_PROTECT, prot, prot_data, size, NULL, 0, NULL, 0);
}

int32_t e502_iface_reload_dev_info(t_x502_hnd hnd) {
    return hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_FLASH_RELOAD_INFO, 0, NULL, 0, NULL, 0, NULL, 0);
}



int32_t e502_iface_cycle_load_start(t_x502_hnd hnd, uint32_t size) {
    return X502_ERR_NOT_IMPLEMENTED;
}

int32_t e502_iface_cycle_setup(t_x502_hnd hnd, uint32_t flags) {
    return X502_ERR_NOT_IMPLEMENTED;
}

int32_t e502_iface_cycle_stop(t_x502_hnd hnd, uint32_t flags) {
    return X502_ERR_NOT_IMPLEMENTED;
}

void e502_devinfo_init(t_x502_info *info, const t_lboot_devinfo *lboot_info) {
    int ver[4];
    int ver_comp_valid;

    strcpy(info->serial, lboot_info->serial);
    info->mcu_firmware_ver = 0;
    ver_comp_valid = sscanf(lboot_info->soft_ver, "%d.%d.%d.%d", &ver[0], &ver[1], &ver[2], &ver[3]);
    if (ver_comp_valid >= 1)
        info->mcu_firmware_ver |= (ver[0]&0xFF) << 24;
    if (ver_comp_valid >= 2)
        info->mcu_firmware_ver |= (ver[1]&0xFF) << 16;
    if (ver_comp_valid >= 3)
        info->mcu_firmware_ver |= (ver[2]&0xFF) << 8;
    if (ver_comp_valid >= 4)
        info->mcu_firmware_ver |= ver[3]&0xFF;
}

X502_EXPORT(int32_t) E502_SwitchToBootloader(t_x502_hnd hnd) {
    int32_t err = X502_CHECK_HND(hnd);
    if (err == X502_ERR_OK) {
        err = hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_BOOT, 0, NULL, 0, NULL, 0, NULL, 0);
    }
    return err;
}


#ifdef WIN32
#include <winsock2.h>
BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    WSADATA wsaData;
    WORD wVersionRequested;

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        wVersionRequested = MAKEWORD(2, 2);
        if (WSAStartup(wVersionRequested, &wsaData))
            return FALSE;
        if (wsaData.wVersion != wVersionRequested) {
            WSACleanup();
            return FALSE;
        }
        break;
    case DLL_PROCESS_DETACH:
        WSACleanup();
        break;
    }
    return TRUE;
}
#endif
