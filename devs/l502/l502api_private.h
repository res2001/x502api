#ifndef L502API_PRIVATE_H
#define L502API_PRIVATE_H

#include "x502api_private.h"
#include "lpcie_ioctls.h"

#define L502_DEVICE_NAME "L502"


typedef struct {
#ifdef _WIN32
    HANDLE file;
#else
    int file;
#endif
} t_pci_iface_data;

#define L502_PCI_IFACE_FILE(hnd) (((t_pci_iface_data*)hnd->iface_data)->file)

int32_t l502_port_fpga_reg_write(t_x502_hnd hnd, uint32_t reg, uint32_t val);
int32_t l502_port_fpga_reg_read(t_x502_hnd hnd, uint32_t reg, uint32_t *val);
int32_t l502_port_open(t_x502_hnd hnd, const t_x502_devrec *devrec);
int32_t l502_port_free_iface_data(void *intptr);
int32_t l502_port_close(t_x502_hnd hnd);
int32_t l502_port_stream_start(t_x502_hnd hnd, uint32_t ch, uint32_t single);
int32_t l502_port_stream_stop(t_x502_hnd hnd, uint32_t ch, uint32_t flags);
int32_t l502_port_stream_free(t_x502_hnd hnd, uint32_t ch, uint32_t flags);
int32_t l502_port_stream_read(t_x502_hnd hnd, uint32_t *buff, uint32_t size, uint32_t timeout);
int32_t l502_port_stream_write(t_x502_hnd hnd, const uint32_t *buff, uint32_t size,
                         uint32_t timeout);
int32_t l502_port_stream_set_params(t_x502_hnd hnd, t_lpcie_stream_ch_params *par);
int32_t l502_port_stream_rdy_size(t_x502_hnd hnd, uint32_t ch, uint32_t *rdy_size);
int32_t l502_port_renew_info(t_x502_hnd hnd);
int32_t l502_port_get_drv_ver(t_x502_hnd hnd, uint32_t *ver);
int32_t l502_port_cycle_load_start(t_x502_hnd hnd, uint32_t ch, uint32_t size, uint32_t min_irq_step);
int32_t l502_port_cycle_setup(t_x502_hnd hnd, uint32_t ch, uint32_t evt);
int32_t l502_port_cycle_stop(t_x502_hnd hnd, uint32_t ch, uint32_t evt);
int32_t l502_port_cycle_check_setup(t_x502_hnd hnd, uint32_t ch, uint32_t *done);



int32_t l502_iface_flash_rd(t_x502_hnd hnd, uint32_t addr, uint8_t* data, uint32_t size);
int32_t l502_iface_flash_wr(t_x502_hnd hnd, uint32_t addr, const uint8_t* data, uint32_t size);
int32_t l502_iface_flash_erase(t_x502_hnd hnd, uint32_t addr, uint32_t size);
int32_t l502_iface_flash_set_prot(t_x502_hnd hnd, uint32_t prot, const uint8_t* prot_data, uint32_t size);

int32_t l502_iface_bf_mem_block_rd(t_x502_hnd hnd, uint32_t addr, uint32_t *block, uint32_t size);
int32_t l502_iface_bf_mem_block_wr(t_x502_hnd hnd, uint32_t addr, const uint32_t *block, uint32_t size);
int32_t l502_iface_bf_firm_load(t_x502_hnd hnd, const char *filename);

int32_t l502_devlist_gen(t_x502_devrec *info, void *iface_data);

#endif // L502API_PRIVATE_H

