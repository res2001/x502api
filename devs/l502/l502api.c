#include "l502api.h"
#include "l502api_private.h"
#include "l502_fpga_regs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* минимальный размер внутреннего буфера */
#define L502_DMA_IN_BUF_SIZE_MIN 16*1024


#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hmod, DWORD reason, LPVOID resvd) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif

static int32_t f_iface_free_devinfo_ptr(t_x502_devrec_inptr *devinfo_ptr);
static int32_t f_iface_stream_cfg(t_x502_hnd hnd, uint32_t ch, t_x502_stream_ch_params *params);
static int32_t f_iface_cycle_load_start(t_x502_hnd hnd, uint32_t size);
static int32_t f_iface_cycle_setup(t_x502_hnd hnd, uint32_t flags);
static int32_t f_iface_cycle_stop(t_x502_hnd hnd, uint32_t flags);
static int32_t f_iface_cycle_check_setup(t_x502_hnd hnd, uint32_t *done);


static const t_x502_dev_iface f_pcie_iface = {
    L502_REGS_HARD_ID,
    L502_DMA_IN_BUF_SIZE_MIN,
    0,
    L502_BF_REQ_DATA_SIZE_MAX,
    4, //flash rd size
    1, //flash wr size
    f_iface_free_devinfo_ptr,
    l502_port_open,
    l502_port_close,
    l502_port_fpga_reg_read,
    l502_port_fpga_reg_write,
    f_iface_stream_cfg,
    l502_port_stream_start,
    l502_port_stream_stop,
    l502_port_stream_free,
    NULL,
    l502_port_stream_read,
    l502_port_stream_write,
    l502_port_stream_rdy_size,
    l502_iface_bf_mem_block_rd,
    l502_iface_bf_mem_block_wr,
    l502_iface_bf_firm_load,
    l502_iface_flash_rd,
    l502_iface_flash_wr,
    l502_iface_flash_erase,
    l502_iface_flash_set_prot,
    l502_port_renew_info,
    f_iface_cycle_load_start,
    f_iface_cycle_setup,
    f_iface_cycle_stop,
    f_iface_cycle_check_setup,
    NULL,
    NULL
};


X502_EXPORT(int32_t) L502_GetDriverVersion(t_x502_hnd hnd, uint32_t* ver) {
    int32_t err = X502_CHECK_HND(hnd);
    if (err == X502_ERR_OK)
        err = l502_port_get_drv_ver(hnd, ver);
    return err;
}

int32_t l502_devlist_gen(t_x502_devrec *info, void *iface_data) {
    int32_t err = X502_ERR_OK;
    t_x502_devrec_inptr *devinfo_ptr = calloc(1, sizeof(t_x502_devrec_inptr));
    if (devinfo_ptr == NULL) {
        err = X502_ERR_MEMORY_ALLOC;
    } else {
        devinfo_ptr->iface_data = iface_data;
        devinfo_ptr->iface = &f_pcie_iface;        
        info->iface = X502_IFACE_PCI;
        info->flags = X502_DEVFLAGS_IFACE_SUPPORT_PCI | X502_DEVFLAGS_FPGA_LOADED;
        info->internal = devinfo_ptr;
    }
    return err;
}

X502_EXPORT(int32_t) L502_GetSerialList(char serials[][X502_SERIAL_SIZE], uint32_t size,
                                        uint32_t flags, uint32_t *devcnt) {
    return X502_GetSerialList(serials, size, flags, devcnt, L502_DEVICE_NAME, L502_GetDevRecordsList);
}

X502_EXPORT(int32_t) L502_Open(t_x502_hnd hnd, const char *serial) {
    return X502_Open(hnd, serial, L502_DEVICE_NAME, L502_GetDevRecordsList);
}


static int32_t f_iface_free_devinfo_ptr(t_x502_devrec_inptr *devinfo_ptr) {
    l502_port_free_iface_data(devinfo_ptr->iface_data);
    free(devinfo_ptr);
    return X502_ERR_OK;
}

static int32_t f_iface_stream_cfg(t_x502_hnd hnd, uint32_t ch, t_x502_stream_ch_params *params) {
    t_lpcie_stream_ch_params lpcie_ch_params;
    memset(&lpcie_ch_params, 0, sizeof(lpcie_ch_params));
    lpcie_ch_params.ch = ch;
    lpcie_ch_params.irq_step = params->step;
    lpcie_ch_params.buf_size = params->buf_size;
    return l502_port_stream_set_params(hnd, &lpcie_ch_params);
}

static int32_t f_iface_cycle_load_start(t_x502_hnd hnd, uint32_t size) {
    return l502_port_cycle_load_start(hnd, L502_DMA_CHNUM_OUT, size);
}
static int32_t f_iface_cycle_setup(t_x502_hnd hnd, uint32_t flags) {
    return l502_port_cycle_setup(hnd, L502_DMA_CHNUM_OUT, (flags & X502_OUT_CYCLE_FLAGS_FORCE) ?
                                      LPCIE_CYCLE_SW_EVT_IMMIDIATLY : LPCIE_CYCLE_SW_EVT_END_OF_CYCLE);
}

static int32_t f_iface_cycle_stop(t_x502_hnd hnd, uint32_t flags) {
    return l502_port_cycle_stop(hnd, L502_DMA_CHNUM_OUT, (flags & X502_OUT_CYCLE_FLAGS_FORCE) ?
                           LPCIE_CYCLE_SW_EVT_IMMIDIATLY : LPCIE_CYCLE_SW_EVT_END_OF_CYCLE);
}

static int32_t f_iface_cycle_check_setup(t_x502_hnd hnd, uint32_t *done) {
    return X502_ERR_NOT_SUP_BY_DRIVER;
}
