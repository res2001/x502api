#ifndef X502API_PRIVATE_H
#define X502API_PRIVATE_H


#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "x502api.h"
#include "osspec.h"
#include "l502_bf_cmd_defs.h"
#include "x502_fpga_regs.h"
#include <stdio.h>


struct st_x502_devrec_inptr {
    const void *iface;
    void *iface_data;
};


#define X502_SIGN 0xA55A0502


#define X502_CHECK_HND(hnd) ((hnd!=NULL) ? (hnd)->sign == X502_SIGN ? X502_ERR_OK \
    : X502_ERR_INVALID_HANDLE : X502_ERR_INVALID_HANDLE)

#define X502_CHECK_HND_OPEND(hnd) ((hnd!=NULL) ? (hnd)->sign == X502_SIGN ? \
            (((hnd)->flags & PRIV_FLAGS_OPENED) ? X502_ERR_OK : X502_ERR_DEVICE_NOT_OPENED) \
        : X502_ERR_INVALID_HANDLE : X502_ERR_INVALID_HANDLE)




/* на сколько секунд данных будет рассчитан внутренний буфер */
#define X502_DMA_IN_BUF_FOR_SEC  4
/* максимальное кол-во прерываний в секунду */
#define X502_DMA_IN_MAX_IRQ_PER_SEC  20

#define X502_DMA_OUT_BUF_SIZE  3*3*1024*1024
#define X502_DMA_OUT_IRQ_STEP  (3*1024*1024/64)



#define X502_MUTEX_CFG_LOCK_TOUT 1000


#define X502_STREAM_CH_CNT 2


#ifdef _WIN32
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #define SLEEP_MS(ms) usleep(ms*1000)
#endif




/** параметры канала потока данных */
typedef struct {
    uint32_t buf_size; /** размер временного буфера */
    uint32_t step;     /** через сколько переданных отсчетов будет генерироваться прерывание */
} t_x502_stream_ch_params;



typedef int32_t (*t_x502_iface_free_devinfo_data)(void* devinfo_data);

typedef int32_t (*t_x502_iface_open)(t_x502_hnd hnd, const t_x502_devrec *devinfo);
typedef int32_t (*t_x502_iface_close)(t_x502_hnd hnd);
typedef int32_t (*t_x502_iface_fpga_reg_read)(t_x502_hnd hnd, uint16_t addr, uint32_t *val);
typedef int32_t (*t_x502_iface_fpga_reg_write)(t_x502_hnd hnd, uint16_t addr, uint32_t val);

typedef int32_t (*t_x502_iface_stream_cfg)(t_x502_hnd hnd, uint32_t ch, t_x502_stream_ch_params *pars);
typedef int32_t (*t_x502_iface_stream_start)(t_x502_hnd hnd, uint32_t ch, uint32_t signle);
typedef int32_t (*t_x502_iface_stream_stop)(t_x502_hnd hnd, uint32_t ch);
typedef int32_t (*t_x502_iface_stream_free)(t_x502_hnd hnd, uint32_t ch);
typedef int32_t (*t_x502_iface_stream_running)(t_x502_hnd hnd, uint32_t ch, int32_t* running);
typedef int32_t (*t_x502_iface_stream_read)(t_x502_hnd hnd, uint32_t *buf, uint32_t size, uint32_t tout);
typedef int32_t (*t_x502_iface_stream_write)(t_x502_hnd hnd, const uint32_t *buf, uint32_t size, uint32_t tout);
typedef int32_t (*t_x502_iface_stream_get_rdy_cnt)(t_x502_hnd hnd, uint32_t ch, uint32_t *rdy_cnt);

typedef int32_t (*t_x502_iface_bf_mem_block_rd)(t_x502_hnd hnd, uint32_t addr, uint32_t *block, uint32_t size);
typedef int32_t (*t_x502_iface_bf_mem_block_wr)(t_x502_hnd hnd, uint32_t addr, const uint32_t *block, uint32_t size);
typedef int32_t (*t_x502_iface_bf_firm_load)(t_x502_hnd hnd, FILE* f);

typedef int32_t (*t_x502_iface_flash_rd)(t_x502_hnd hnd, uint32_t addr, uint8_t* data, uint32_t size);
typedef int32_t (*t_x502_iface_flash_wr)(t_x502_hnd hnd, uint32_t addr, const uint8_t* data, uint32_t size);
typedef int32_t (*t_x502_iface_flash_erase)(t_x502_hnd hnd, uint32_t addr, uint32_t size);
typedef int32_t (*t_x502_iface_flash_set_prot)(t_x502_hnd hnd, uint32_t flags,
                                               const uint8_t* prot_data, uint32_t size);


typedef int32_t (*t_x502_iface_reload_devinfo)(t_x502_hnd hnd);

typedef int32_t (*t_x502_iface_gen_ioctl)(t_x502_hnd hnd, uint32_t cmd_code, uint32_t param,
                                          const void* snd_data, uint32_t snd_size,
                                          void* rcv_data, uint32_t recv_size,
                                          uint32_t* recvd_size, uint32_t tout);




typedef struct {
    uint16_t id_reg_addr;
    uint16_t in_stream_buf_min;
    uint16_t ioctl_max_data_size;
    uint16_t bf_mem_block_size;
    uint16_t flash_rd_size;  /**< Максимальный размер чтения из flash-памяти за один запрос */
    uint16_t flash_wr_size;  /**< Максимальный размер записи во flash-память за один запрос */
    t_x502_iface_free_devinfo_data  free_devinfo_data;
    t_x502_iface_open               open;
    t_x502_iface_close              close;
    t_x502_iface_fpga_reg_read      fpga_reg_read;
    t_x502_iface_fpga_reg_write     fpga_reg_write;
    t_x502_iface_stream_cfg         stream_cfg;
    t_x502_iface_stream_start       stream_start;
    t_x502_iface_stream_stop        stream_stop;
    t_x502_iface_stream_free        stream_free;
    t_x502_iface_stream_running     stream_running;
    t_x502_iface_stream_read        stream_read;
    t_x502_iface_stream_write       stream_write;
    t_x502_iface_stream_get_rdy_cnt stream_get_rdy_cnt;
    t_x502_iface_bf_mem_block_rd    bf_mem_block_rd;
    t_x502_iface_bf_mem_block_wr    bf_mem_block_wr;
    t_x502_iface_bf_firm_load       bf_firm_load;
    t_x502_iface_flash_rd           flash_rd;
    t_x502_iface_flash_wr           flash_wr;
    t_x502_iface_flash_erase        flash_erase;
    t_x502_iface_flash_set_prot     flash_set_prot;
    t_x502_iface_reload_devinfo     reload_dev_info;
    t_x502_iface_gen_ioctl          gen_ioctl;
} t_x502_dev_iface;





typedef enum {
    STREAM_IN_WRD_ADC  = 0,
    STREAM_IN_WRD_DIN  = 1,
    STREAM_IN_WRD_MSG  = 2,
    STREAM_IN_WRD_USR  = 3
} t_stream_in_wrd_type;



typedef enum {
    PRIV_FLAGS_OPENED             = 0x0001,
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
    uint32_t out_freq_div;
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

    t_x502_stream_ch_params stream_pars[X502_STREAM_CH_CNT];

    uint32_t last_dout; /* последнее выданное значение на DIGOUT */
    uint32_t proc_adc_ch; /* ожидаемый логический канал для следующей ProcessData() */


    t_mutex mutex_cfg; /* мьютекс для доступа к полям параметров и состояния модуля */
    t_mutex mutex_bf; /* мьютекс для доступа к памяти сигнального процессора */

    uint32_t bf_ver; /* версия прошивки BlackFin, если есть */
    uint32_t bf_features; /* дополниельные возможности, поддерживаемые прошивкой */
} t_x502;


typedef int32_t (APIENTRY *t_x502_get_devinfo_list_cb)(t_x502_devrec* list, uint32_t size,
                                             uint32_t flags, uint32_t* devcnt);\




int x502_check_eeprom(t_x502_hnd hnd);

X502_EXPORT(int32_t) X502_FreeDevRecordList(t_x502_devrec *list, uint32_t size);
X502_EXPORT(int32_t) X502_Open(t_x502_hnd hnd, const char* serial,
                                const char *devname, t_x502_get_devinfo_list_cb get_list);


#define x502_bf_set_par(hnd, par, data, size) X502_BfExecCmd(hnd, L502_BF_CMD_CODE_SET_PARAM, \
                            par, data, size, NULL, 0, X502_BF_CMD_DEFAULT_TOUT, NULL)

#define x502_bf_get_par(hnd, par, data, size) X502_BfExecCmd(hnd, L502_BF_CMD_CODE_GET_PARAM, \
                            par, NULL, 0, data, size, X502_BF_CMD_DEFAULT_TOUT, NULL)


#endif // X502API_PRIVATE_H
