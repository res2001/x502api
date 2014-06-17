#ifndef X502API_PRIVATE_H
#define X502API_PRIVATE_H

#include "x502api.h"
#include "osspec.h"

typedef struct {
    const void *iface;
    void *iface_data;
} t_lpcie_devinfo_inptr;


#define X502_SIGN 0xA55A0502


#define X502_CHECK_HND(hnd) ((hnd) ? (hnd)->sign == X502_SIGN ? X502_ERR_OK \
    : X502_ERR_INVALID_HANDLE : X502_ERR_INVALID_HANDLE)





typedef int32_t (*t_x502_iface_open)(t_x502_hnd hnd, const t_lpcie_devinfo *devinfo);
typedef int32_t (*t_x502_iface_close)(t_x502_hnd hnd);
typedef int32_t (*t_x502_iface_fpga_reg_read)(t_x502_hnd hnd, uint16_t addr, uint32_t *val);
typedef int32_t (*t_x502_iface_fpga_reg_write)(t_x502_hnd hnd, uint16_t addr, uint32_t val);

typedef int32_t (*t_x502_iface_free_devinfo_data)(void* devinfo_data);

typedef struct {
    uint16_t id_reg_addr;
    t_x502_iface_open               open;
    t_x502_iface_close              close;
    t_x502_iface_fpga_reg_read      fpga_reg_read;
    t_x502_iface_fpga_reg_write     fpga_reg_write;
    t_x502_iface_free_devinfo_data  free_devinfo_data;
} t_x502_dev_iface;









typedef enum {
    _FLAGS_OPENED             = 0x0001,
    _FLAGS_PRELOAD_DONE       = 0x0002,
    _FLGAS_CYCLE_MODE         = 0x0004,
    _FLAGS_STREAM_RUN         = 0x0080
} t_x502_state_flags;




/** структура, описывающая параметры логического канала */
typedef struct {
    uint32_t ch; /** физический номер канала */
    uint32_t mode; /** режим работы канала из #t_l502_ch_mode */
    uint32_t range; /** диапазон измерения */
    uint32_t avg;  /** коэффициент усреднения */
} t_x502_lch;

typedef struct {
    t_x502_lch lch[X502_LTABLE_MAX_CH_CNT];
    uint32_t lch_cnt;
    uint32_t adc_freq_div;
    uint32_t adc_frame_delay;
    uint32_t din_freq_div;
    uint32_t sync_mode;
    uint32_t sync_start_mode;
    uint32_t ref_freq;
    uint32_t dac_freq_div;
} t_x502_settings;



typedef struct st_x502 {
    uint32_t sign; /* признак описателя L502/E502 */
    const t_x502_dev_iface *iface;
    void *iface_data;
    t_x502_state_flags  flags; /* флаги состояния платы */
    t_l502_streams streams; /* какие синхронные потоки разрешены */
    t_l502_mode mode; /* режим работы (через ПЛИС или DSP) */
    t_x502_info info;
    t_x502_settings     set; /* настройки платы */

    uint32_t last_dout; /* последнее выданное значение на DIGOUT */

    t_mutex mutex_cfg; /* мьютекс для доступа к полям параметров и состояния модуля */
} t_x502;


typedef int32_t (*t_x502_get_devinfo_list_cb)(t_lpcie_devinfo* list, uint32_t size,
                                             uint32_t flags, uint32_t* devcnt);\






LPCIE_EXPORT(int32_t) X502_FreeDevInfoList(t_lpcie_devinfo *list, uint32_t size);
LPCIE_EXPORT(int32_t) X502_Open(t_x502_hnd hnd, const char* serial,
                                const char *devname, t_x502_get_devinfo_list_cb get_list);



#endif // X502API_PRIVATE_H
