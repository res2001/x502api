#include "l502api_private.h"
#include "ltimer.h"
#include "l502_fpga_regs.h"
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

#define L502_BF_WAIT_LOAD_RDY_TOUT  500

#define LDR_BUFF_SIZE 4096

#define BF_CHECK_ADDR(addr)  (((addr) < 0xFFA0C000) && ((addr)>= 0xFFA0000)) || \
    (((addr) < 0xFF908000) && ((addr) >=0xFF900000)) || \
    (((addr) < 0xFF808000) && ((addr) >=0xFF800000)) || \
    (((addr) < 0x2000000)) ? 0 : X502_ERR_BF_INVALID_ADDR

#define BF_CHECK_ADDR_SIZE(addr, size) BF_CHECK_ADDR(addr) ? X502_ERR_BF_INVALID_ADDR : \
    BF_CHECK_ADDR(addr+size*4-1) ? X502_ERR_BF_INVALID_ADDR : 0


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
int32_t f_parse_ldr_hdr(const uint8_t *hdr, t_bf_ldr_pkt *pkt) {
    int32_t err = X502_ERR_OK;
    uint32_t* pdw_buff = (uint32_t*)hdr;
    uint8_t xor_ch = 0;
    int i;
    for (i=0; i < BF_LDR_HDR_SIZE; i++) {
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

        if ((pkt->flags & BF_LDR_FLAG_INIT) && (pkt->flags & BF_LDR_FLAG_FILL)) {
            err = X502_ERR_LDR_FILE_FORMAT;
        } else if (pkt->flags & (BF_LDR_FLAG_CALLBACK | BF_LDR_FLAG_INDIRECT | BF_LDR_FLAG_INIT)) {
            err = X502_ERR_LDR_FILE_UNSUP_FEATURE;
        } else if ((pkt->flags & BF_LDR_FLAG_INIT) && (pkt->addr != 0xFFA00000)) {
            err = X502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR;
        }
    }
    return err;
}

static int32_t f_bf_wait_cmd_done(t_x502_hnd hnd) {
    int32_t err = X502_ERR_OK;
    t_ltimer tmr;
    uint32_t status;
    ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(X502_BF_REQ_TOUT));
    do {
        err = l502_port_fpga_reg_read(hnd, L502_REGS_BF_STATUS, &status);
    } while ((status &  L502_REGBIT_BF_STATUS_BUSY_Msk) &&
             (err == X502_ERR_OK) && !ltimer_expired(&tmr));

    if (!err && (status & L502_REGBIT_BF_STATUS_BUSY_Msk))
        err = X502_ERR_BF_REQ_TIMEOUT;
    return err;
}



int32_t l502_iface_bf_mem_block_rd(t_x502_hnd hnd, uint32_t addr, uint32_t *block, uint32_t size){
    uint32_t i;
    int32_t err = f_bf_wait_cmd_done(hnd);
    /* записываем переметры передачи - размер и адрес в памяти BlackFin */
    if (err == X502_ERR_OK)
        err = l502_port_fpga_reg_write(hnd,  L502_REGS_BF_REQ_SIZE, size);
    if (err == X502_ERR_OK)
        err = l502_port_fpga_reg_write(hnd, L502_REGS_BF_REQ_ADDR, addr);
    /* даем команду на запис */
    if (err == X502_ERR_OK)
        err = l502_port_fpga_reg_write(hnd, L502_REGS_BF_CMD, L502_BF_CMD_READ);

    /* ждем, пока операция не будет завершена */
    if (err == X502_ERR_OK)
        err = f_bf_wait_cmd_done(hnd);

    /* записываем блок данных в буфер ПЛИС */
    for (i=0; (i < size) && (err == X502_ERR_OK); i++) {
        err = l502_port_fpga_reg_read(hnd, L502_REGS_BF_REQ_DATA+i, &block[i]);
    }
    return err;
}

int32_t l502_iface_bf_mem_block_wr(t_x502_hnd hnd, uint32_t addr, const uint32_t *block, uint32_t size) {
    uint32_t i;
    int32_t err = f_bf_wait_cmd_done(hnd);
    if (err == X502_ERR_OK) {
        /* записываем блок данных в буфер ПЛИС */
        for (i=0; (i < size) && (err == X502_ERR_OK); i++) {
            err = l502_port_fpga_reg_write(hnd, L502_REGS_BF_REQ_DATA+i, block[i]);
        }

        /* записываем переметры передачи - размер и адрес в памяти BlackFin */
        if (err == X502_ERR_OK)
            err = l502_port_fpga_reg_write(hnd,  L502_REGS_BF_REQ_SIZE, size);
        if (err == X502_ERR_OK)
            err = l502_port_fpga_reg_write(hnd, L502_REGS_BF_REQ_ADDR, addr);
        /* даем команду на запис */
        if (err == X502_ERR_OK)
            err = l502_port_fpga_reg_write(hnd, L502_REGS_BF_CMD, L502_BF_CMD_WRITE);

        /* ждем, пока операция не будет завершена */
        if (err == X502_ERR_OK)
            err = f_bf_wait_cmd_done(hnd);
    }
    return err;
}


static int32_t f_bf_mem_wr(t_x502_hnd hnd, uint32_t addr, const uint32_t* regs, uint32_t size) {
    int32_t err = X502_ERR_OK;

    /* данные записываем блоками по L502_BF_REQ_DATA_SIZE */
    while ((err == X502_ERR_OK) && size) {
        int put_size = (size < hnd->iface_hnd->bf_mem_block_size) ? size :
                                         hnd->iface_hnd->bf_mem_block_size;
        err = hnd->iface_hnd->bf_mem_block_wr(hnd, addr, regs, put_size);
        if (!err) {
            size -= put_size;
            regs += put_size;
            addr += put_size*4;
        }
    }
    return err;
}

int32_t l502_iface_bf_firm_load(t_x502_hnd hnd, const char *filename) {
    int32_t err = X502_ERR_OK;
    FILE* ldr_file=fopen(filename, "rb");
    if (ldr_file==NULL) {
        err = X502_ERR_LDR_FILE_OPEN;
    } else {
        int32_t next_err = X502_ERR_OK;
        uint32_t *ldr_buff = NULL;
        ldr_buff = malloc(LDR_BUFF_SIZE);
        if (ldr_buff == NULL)
            err = X502_ERR_MEMORY_ALLOC;

        if (err == X502_ERR_OK) {
            int rd_size = 0;
            int stop = 0;
            uint32_t reg;
            uint8_t hdr[BF_LDR_HDR_SIZE];
            t_ltimer tmr;

            //uint32_t* pdw = (uint32_t*)ldr_buff;
            t_bf_ldr_pkt pkt, pkt_next;
            uint32_t bf_val = 0;
            memset(&pkt_next, 0, sizeof(pkt_next));

            l502_port_fpga_reg_read(hnd, L502_REGS_BF_CTL, &bf_val);
            l502_port_fpga_reg_write(hnd, L502_REGS_BF_CTL, L502_REGBIT_BF_CTL_DSP_MODE_Msk
                            | (bf_val & 0xF00)); //set rst
            SLEEP_MS(1);
            l502_port_fpga_reg_write(hnd, L502_REGS_BF_CTL, L502_REGBIT_BF_CTL_DSP_MODE_Msk |
                            L502_REGBIT_BF_CTL_BF_RESET_Msk | (bf_val & 0xF00));  //release rst

            ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(L502_BF_WAIT_LOAD_RDY_TOUT));
            do {
                l502_port_fpga_reg_read(hnd, L502_REGS_BF_CTL, &reg);
                if ((reg & L502_REGBIT_BF_CTL_HOST_WAIT_Msk) && ltimer_expired(&tmr))
                    err = X502_ERR_BF_LOAD_RDY_TOUT;
            } while ((err == X502_ERR_OK) && (reg & L502_REGBIT_BF_CTL_HOST_WAIT_Msk));

            if (err == X502_ERR_OK) {
                err = fread(hdr, 1, BF_LDR_HDR_SIZE, ldr_file) == BF_LDR_HDR_SIZE ?
                      f_parse_ldr_hdr(hdr, &pkt) : X502_ERR_LDR_FILE_READ;
            }

            while ((err == X502_ERR_OK) && !stop) {
                if (next_err != X502_ERR_OK) {
                    err = next_err;
                } else if (((pkt.flags & BF_LDR_FLAG_FILL) == 0) && (pkt.size != 0)) {
                    int r_size = (pkt.size > LDR_BUFF_SIZE) ? LDR_BUFF_SIZE : pkt.size;

                    rd_size = (int)fread(ldr_buff, 1, r_size, ldr_file);
                    if (rd_size!=r_size)
                        err = X502_ERR_LDR_FILE_READ;
                }
                if (err == X502_ERR_OK) {
                    if (pkt.size > LDR_BUFF_SIZE) {
                        pkt_next = pkt;
                        pkt_next.addr += LDR_BUFF_SIZE;
                        pkt_next.size -= LDR_BUFF_SIZE;
                        pkt.size = LDR_BUFF_SIZE;
                    } else {
                        next_err = fread(hdr, 1, BF_LDR_HDR_SIZE, ldr_file) == BF_LDR_HDR_SIZE ?
                                  f_parse_ldr_hdr(hdr, &pkt_next) : X502_ERR_LDR_FILE_READ;
                        if (next_err != X502_ERR_OK) {
                            pkt_next.size = 0;
                        }
                    }

                    if (pkt.size!=0) {
                        uint32_t size = ((pkt.size+31)/(32))*8;
                        if (pkt.flags & BF_LDR_FLAG_FILL) {
                            uint32_t i;
                            for (i=0; i < size; i++)
                                ldr_buff[i] = pkt.arg;
                        }

                        if ((pkt.flags & BF_LDR_FLAG_FINAL)
                            || ((pkt_next.flags & BF_LDR_FLAG_FINAL) && (pkt_next.size==0))) {
                            uint32_t buf_pos = 0;
                            err = BF_CHECK_ADDR_SIZE(pkt.addr, size);

                            if ((err == X502_ERR_OK) && (size > 8)) {
                                err = f_bf_mem_wr(hnd, pkt.addr, ldr_buff, size-8);
                                pkt.addr+=4*(size-8);                                
                                buf_pos = size-8;
                                size = 8;
                            }

                            if (err == X502_ERR_OK)
                                err = l502_port_fpga_reg_write(hnd, L502_REGS_BF_CMD, L502_BF_CMD_HIRQ);
                            if (err == X502_ERR_OK)
                                err = f_bf_mem_wr(hnd, pkt.addr, &ldr_buff[buf_pos], size);
                            stop=1;

                            if (err == X502_ERR_OK) {
                                err = l502_port_fpga_reg_write(hnd, L502_REGS_BF_CTL, L502_REGBIT_BF_CTL_DSP_MODE_Msk |
                                            L502_REGBIT_BF_CTL_BF_RESET_Msk);
                            }
                        } else if (!(pkt.flags & BF_LDR_FLAG_IGNORE)) {
                            err = BF_CHECK_ADDR_SIZE(pkt.addr, size);
                            if (!err)
                                err = f_bf_mem_wr(hnd, pkt.addr, ldr_buff, size);
                        }
                    }
                    pkt = pkt_next;
                }
            }
        }
        free(ldr_buff);

        fclose(ldr_file);
    }
    return err;
}
