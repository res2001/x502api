#ifndef E502API_PRIVATE_H
#define E502API_PRIVATE_H

#include "e502api.h"
#include "x502api_private.h"
#include "e502_cm4_defs.h"

#define E502_DEVICE_NAME "E502"

int32_t e502_iface_fpga_read(t_x502_hnd hnd, uint16_t addr, uint32_t *val);
int32_t e502_iface_fpga_write(t_x502_hnd hnd, uint16_t addr, uint32_t val);
int32_t e502_iface_stream_running(t_x502_hnd hnd, uint32_t ch, int32_t* running);
int32_t e502_iface_bf_mem_block_rd(t_x502_hnd hnd, uint32_t addr, uint32_t *block, uint32_t size);
int32_t e502_iface_bf_mem_block_wr(t_x502_hnd hnd, uint32_t addr, const uint32_t *block, uint32_t size);
int32_t e502_iface_bf_firm_load(t_x502_hnd hnd, FILE* f);



int32_t e502_iface_flash_rd(t_x502_hnd hnd, uint32_t addr, uint8_t* data, uint32_t size);
int32_t e502_iface_flash_wr(t_x502_hnd hnd, uint32_t addr, const uint8_t* data, uint32_t size);
int32_t e502_iface_flash_erase(t_x502_hnd hnd, uint32_t addr, uint32_t size);
int32_t e502_iface_flash_set_prot(t_x502_hnd hnd, uint32_t prot, const uint8_t* prot_data, uint32_t size);

#endif // E502API_PRIVATE_H
