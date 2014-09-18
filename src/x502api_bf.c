#include "x502api_private.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BF_LDR_HDR_SIZE  (16)
#define BF_LDR_HDRSGN    (0xAD)

#define BF_LDR_HDRPOS_SGN  (3)

#define BF_LDR_FLAG_SAVE      (0x0010) //не используется
#define BF_LDR_FLAG_AUX       (0x0020) //не используется
#define BF_LDR_FLAG_FILL      (0x0100)
#define BF_LDR_FLAG_QUICKBOOT (0x0200) //не используется
#define BF_LDR_FLAG_CALLBACK  (0x0400) //не используется
#define BF_LDR_FLAG_INIT      (0x0800) //не используется
#define BF_LDR_FLAG_IGNORE    (0x1000)
#define BF_LDR_FLAG_INDIRECT  (0x2000) //не используется
#define BF_LDR_FLAG_FIRST     (0x4000)
#define BF_LDR_FLAG_FINAL     (0x8000)

#define BF_WAIT_LOAD_RDY_TOUT  500
#define BF_MUTEX_LOCK_TOUT  1000

#define LDR_BUFF_SIZE 4096



#define BF_CHECK_ADDR(addr)  (((addr) < 0xFFA0C000) && ((addr)>= 0xFFA0000)) || \
    (((addr) < 0xFF908000) && ((addr) >=0xFF900000)) || \
    (((addr) < 0xFF808000) && ((addr) >=0xFF800000)) || \
    (((addr) < 0x2000000)) ? 0 : X502_ERR_BF_INVALID_ADDR

#define BF_CHECK_ADDR_SIZE(addr, size) BF_CHECK_ADDR(addr) ? X502_ERR_BF_INVALID_ADDR : \
    BF_CHECK_ADDR(addr+size*4-1) ? X502_ERR_BF_INVALID_ADDR : 0


#define BF_CMD_FIRST_DATA_BLOCK_SIZE ((X502_BF_REQ_DATA_SIZE_MAX-offsetof(t_l502_bf_cmd, data))/4)

typedef struct st_bf_ldr_pkt {
    uint8_t res;
    uint8_t dma_mode;
    uint16_t flags;
    uint32_t addr;
    uint32_t size;
    uint32_t arg;
} t_bf_ldr_pkt;

/* Разбираем заголовок блока LDR-формата из буфера размером BF_LDR_HDR_SIZE
   и сохраняем параметры в структуре pkt */
int f_parse_ldr_hdr(const uint8_t* hdr, t_bf_ldr_pkt* pkt) {
    int err = 0;
    uint32_t* pdw_buff = (uint32_t*)hdr;
    uint8_t xor_ch = 0;
    int i;
    for (i=0; i < BF_LDR_HDR_SIZE; i++)  {
        xor_ch ^= hdr[i];
    }

    if ((xor_ch!=0) || (hdr[BF_LDR_HDRPOS_SGN] != BF_LDR_HDRSGN)) {
        err = X502_ERR_LDR_FILE_FORMAT;
    } else {
        pkt->res = 0;
        pkt->dma_mode = pdw_buff[0]&0xF;
        pkt->flags = pdw_buff[0]&0xFFF0;
        pkt->addr = pdw_buff[1];
        pkt->size = pdw_buff[2];
        pkt->arg = pdw_buff[3];

        if ((pkt->flags & BF_LDR_FLAG_INIT) && (pkt->flags & BF_LDR_FLAG_FILL))
            err = X502_ERR_LDR_FILE_FORMAT;
        else if (pkt->flags & (BF_LDR_FLAG_CALLBACK | BF_LDR_FLAG_INDIRECT | BF_LDR_FLAG_INIT))
            err = X502_ERR_LDR_FILE_UNSUP_FEATURE;
        else if ((pkt->flags & BF_LDR_FLAG_INIT) && (pkt->addr != 0xFFA00000))
            err = X502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR;
    }
    return err;
}

static int32_t f_bf_mem_wr(t_x502_hnd hnd, uint32_t addr, const uint32_t* regs, uint32_t size) {
    int32_t err = 0;

    /* данные записываем блоками по L502_BF_REQ_DATA_SIZE */
    while (!err && size) {
        int put_size = (size < hnd->iface->bf_mem_block_size) ? size :
                                         hnd->iface->bf_mem_block_size;
        err = hnd->iface->bf_mem_block_wr(hnd, addr, regs, put_size);
        if (!err) {
            size -= put_size;
            regs += put_size;
            addr += put_size*4;
        }
    }
    return err;
}

int32_t f_bf_mem_rd(t_x502_hnd hnd, uint32_t addr, uint32_t* regs, uint32_t size) {
    int err = 0;
    while (!err && size) {
        int get_size = (size < hnd->iface->bf_mem_block_size) ? size :
                                         hnd->iface->bf_mem_block_size;

        err = hnd->iface->bf_mem_block_rd(hnd, addr, regs, get_size);

        if (!err) {
            size -= get_size;
            regs += get_size;
            addr += get_size*4;
        }
    }
    return err;
}





static int32_t f_check_bf_firm(t_x502_hnd hnd) {
    int32_t err = hnd->iface->fpga_reg_write(hnd, X502_REGS_BF_CMD, X502_BF_CMD_HDMA_RST);
    if (!err) {
        uint32_t rcv_wrds[2], recvd;

        /* Проверяем версию прошивки BlackFin */
        err = X502_BfExecCmd(hnd, L502_BF_CMD_CODE_GET_PARAM, L502_BF_PARAM_FIRM_VERSION,
                             NULL, 0, rcv_wrds, 2, X502_BF_CMD_DEFAULT_TOUT, &recvd);
        if (!err) {
            if (recvd >= 1) {
                hnd->bf_ver = rcv_wrds[0];
            } else {
                err = X502_ERR_BF_CMD_RETURN_INSUF_DATA;
            }
            hnd->bf_features = recvd >= 2 ? rcv_wrds[1] : 0;
        }
    }

    /* Проверка состояния прошивки (запущен поток сбора или нет) */
    if (!err) {
        uint32_t mode, streams;

        err = x502_bf_get_par(hnd, L502_BF_PARAM_STREAM_MODE, &mode, 1);
        if (!err)
            err = x502_bf_get_par(hnd, L502_BF_PARAM_ENABLED_STREAMS, &streams, 1);
        if (!err) {
            err = osspec_mutex_lock(hnd->mutex_cfg, BF_MUTEX_LOCK_TOUT);
            if (!err) {
                if (mode==L502_BF_MODE_IDLE) {
                    hnd->flags &= ~_FLAGS_STREAM_RUN;
                } else {
                    hnd->flags |= _FLAGS_STREAM_RUN;
                }
                hnd->streams = streams;
                osspec_mutex_release(hnd->mutex_cfg);
            }
        }
    }


    /* передаем информацию о модуле */
    if (!err && !(hnd->flags & _FLAGS_STREAM_RUN)) {
        uint32_t put_wrds[3];
        uint32_t ch;
        put_wrds[0] = hnd->info.devflags;
        put_wrds[1] = hnd->info.fpga_ver | ((uint32_t)hnd->info.plda_ver<<16);
        err = x502_bf_set_par(hnd, L502_BF_PARAM_MODULE_INFO, put_wrds, 2);

        for (ch=0; !err && (ch < X502_DAC_CH_CNT); ch++) {
            float* pk = (float*)&put_wrds[1];
            float* po = (float*)&put_wrds[2];
            put_wrds[0] = ch;
            *pk = (float)hnd->info.cbr.dac[ch].k;
            *po = (float)hnd->info.cbr.dac[ch].offs;
            err = x502_bf_set_par(hnd, L502_BF_PARAM_DAC_COEF, put_wrds, 3);
        }
    }

    if (!err && (hnd->mode!=X502_MODE_DSP))
        err = X502_SetMode(hnd, X502_MODE_DSP);

    return err;
}




X502_EXPORT(int32_t) X502_BfCheckFirmwareIsLoaded(t_x502_hnd hnd, uint32_t *version) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err) {
        err = f_check_bf_firm(hnd);
        if (!err && version)
            *version = hnd->bf_ver;
    }
    return err;
}

X502_EXPORT(int32_t) X502_BfLoadFirmware(t_x502_hnd hnd, const char* filename) {
    uint32_t* ldr_buff = NULL;
    FILE* ldr_file=NULL;
    int32_t err = X502_CHECK_HND(hnd);


    if (!err && !(hnd->info.devflags & X502_DEVFLAGS_BF_PRESENT))
        err = X502_ERR_BF_NOT_PRESENT;


    if (!err && (filename!=NULL)) {
        ldr_file = fopen(filename, "rb");
        if (ldr_file==NULL)
            err = X502_ERR_LDR_FILE_OPEN;
    }

    if (!err) {
        err = osspec_mutex_lock(hnd->mutex_bf, BF_MUTEX_LOCK_TOUT);
        if (!err) {
            err = hnd->iface->bf_firm_load(hnd, ldr_file);
            osspec_mutex_release(hnd->mutex_bf);
        }
    }

    if (ldr_file!=NULL)
        fclose(ldr_file);
    free(ldr_buff);

    if (!err) {
        SLEEP_MS(100);
        err = f_check_bf_firm(hnd);
    }

    return err;
}




X502_EXPORT(int32_t) X502_BfMemWrite(t_x502_hnd hnd, uint32_t addr,
                                      const uint32_t* regs, uint32_t size) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err && !(hnd->info.devflags & X502_DEVFLAGS_BF_PRESENT))
        err = X502_ERR_BF_NOT_PRESENT;;
    if (!err)
        err = BF_CHECK_ADDR_SIZE(addr, size);
    if (!err)
        err = osspec_mutex_lock(hnd->mutex_bf, BF_MUTEX_LOCK_TOUT);

    if (!err) {
        int32_t release_err;
        err = f_bf_mem_wr(hnd, addr, regs, size);
        release_err = osspec_mutex_release(hnd->mutex_bf);
        if (!err)
            err = release_err;
    }
    return err;
}


X502_EXPORT(int32_t) X502_BfMemRead(t_x502_hnd hnd, uint32_t addr,
                                     uint32_t* regs, uint32_t size) {
    int32_t err = X502_CHECK_HND(hnd);
    if (!err && !(hnd->info.devflags & X502_DEVFLAGS_BF_PRESENT))
        err = X502_ERR_BF_NOT_PRESENT;
    if (!err)
        err = BF_CHECK_ADDR_SIZE(addr,size);
    if (!err)
        err = osspec_mutex_lock(hnd->mutex_bf, BF_MUTEX_LOCK_TOUT);
    if (!err) {
        int32_t release_err;
        err = f_bf_mem_rd(hnd, addr, regs, size);
        release_err = osspec_mutex_release(hnd->mutex_bf);
        if (!err)
            err = release_err;
    }
    return err;
}

static int f_bf_start_cmd(t_x502_hnd hnd, uint16_t cmd_code, uint32_t par,
                          const uint32_t* data, uint32_t size) {
    int err = X502_CHECK_HND(hnd);
    if (!err && !(hnd->info.devflags & X502_DEVFLAGS_BF_PRESENT))
        err = X502_ERR_BF_NOT_PRESENT;
    if (size > L502_BF_CMD_DATA_SIZE_MAX)
        err = X502_ERR_BF_INVALID_CMD_DATA_SIZE;
    if (!err)
        err = osspec_mutex_lock(hnd->mutex_bf, BF_MUTEX_LOCK_TOUT);

    if (!err) {
        t_l502_bf_cmd cmd;
        cmd.code = cmd_code;
        cmd.data_size = size;
        cmd.param = par;
        cmd.result = 0;
        cmd.status = L502_BF_CMD_STATUS_REQ;



        if (size!=0) {
            memcpy(cmd.data, data, size*sizeof(data[0]));
        }

        /* если размер больше, чем можем записать за раз, то сперва записываем
         * хвост, чтобы не получилось, что информация о команде будет записана
         * раньше данных */
        if (size > BF_CMD_FIRST_DATA_BLOCK_SIZE) {
            err = f_bf_mem_wr(hnd, X502_BF_MEMADDR_CMD+X502_BF_REQ_DATA_SIZE_MAX*4,
                              &cmd.data[BF_CMD_FIRST_DATA_BLOCK_SIZE],
                              size - BF_CMD_FIRST_DATA_BLOCK_SIZE);
            size = BF_CMD_FIRST_DATA_BLOCK_SIZE;
        }

        if (!err) {
            err = f_bf_mem_wr(hnd, X502_BF_MEMADDR_CMD, (const uint32_t*)&cmd,
                            (offsetof(t_l502_bf_cmd, data))/4 + size);
        }

        if (err) {
            osspec_mutex_release(hnd->mutex_bf);
        }
    }
    return err;
}

static int f_bf_cmd_check(t_x502_hnd hnd, int32_t *res) {
    t_l502_bf_cmd cmd;
    int err = f_bf_mem_rd(hnd, X502_BF_MEMADDR_CMD, (uint32_t*)&cmd,
                          X502_BF_REQ_DATA_SIZE_MIN);
    if (!err) {
        if (cmd.status != L502_BF_CMD_STATUS_DONE) {
            err = X502_ERR_BF_CMD_IN_PROGRESS;
        } else if (res) {
            *res = cmd.result;
        }
    }

    if (err != X502_ERR_BF_CMD_IN_PROGRESS)
        osspec_mutex_release(hnd->mutex_bf);

    return err;
}

X502_EXPORT(int32_t) X502_BfExecCmd(t_x502_hnd hnd, uint16_t cmd_code, uint32_t par,
                        const uint32_t* snd_data, uint32_t snd_size,
                        uint32_t* rcv_data, uint32_t rcv_size, uint32_t tout,
                        uint32_t* recvd_size) {
    t_l502_bf_cmd cmd;
    int done = 0;
    t_timer tmr;
    int err;

    timer_set(&tmr, tout*CLOCK_CONF_SECOND/1000);
    err = f_bf_start_cmd(hnd, cmd_code, par, snd_data, snd_size);
    while (!err && !done) {
        err = f_bf_cmd_check(hnd, &cmd.result);
        if (!err) {
            done = 1;
            if (rcv_size != 0) {
                err = X502_BfMemRead(hnd, X502_BF_MEMADDR_CMD, (uint32_t*)&cmd,
                                   (offsetof(t_l502_bf_cmd, data))/4 + rcv_size);
                memcpy(rcv_data, cmd.data, rcv_size*sizeof(rcv_data[0]));
                if (recvd_size!=NULL)
                    *recvd_size = cmd.data_size;
            }
            if (!err)
                err = cmd.result;
        } else if (err==X502_ERR_BF_CMD_IN_PROGRESS) {
            if (timer_expired(&tmr)) {
                err = X502_ERR_BF_CMD_TIMEOUT;
                osspec_mutex_release(hnd->mutex_bf);
            } else {
                err = 0;
                SLEEP_MS(20);
            }
        }
    }
    return err;
}
