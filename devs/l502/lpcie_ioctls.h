/***************************************************************************//**
 *  @file lpcie_ioctls.h
 *  Файл содержит определения управляющих запросов драйвера lpcie и типов данных,
 *  используемых для передачи параметров управляющих запросов
 *  @author Borisov Alexey <borisov@lcard.ru>
 *  @date   23.05.2012
 * ****************************************************************************/
#ifndef LPCIE_IOCTLS_H
#define LPCIE_IOCTLS_H

/** Проверка по версии дарайвера, поддерживается ли запрос LPCIE_IOCTL_CYCLE_CHECK_SETUP */
#define LPCIE_IOCTL_SUPPORT_CYCLE_CHECK_SETUP(ver)   (ver >= 0x01000900)

/** Варианты событий, по которым должно произойти переключение
    циклического сигнала */
typedef enum {
    LPCIE_CYCLE_SW_EVT_IMMIDIATLY    = 1, /**< сразу по получению команды */
    LPCIE_CYCLE_SW_EVT_END_OF_CYCLE  = 2  /**< по завершению текущего цикла */
} t_lpcie_cycle_sw_evt;


/** параметры для записи значения регистра */
typedef struct {
    uint32_t addr; /** Адрес регистра */
    uint32_t val; /** Значение регистра */
} t_lpcie_mem_rw;

/** настройки канала DMA, передаваемые вместе с LPCIE_IOCTL_STREAM_SET_PARAMS */
typedef struct {
    uint32_t ch;  /** канал DMA (ввод/вывод) */
    uint32_t res[2]; /** резерв */
    uint32_t buf_size; /** размер каждой страницы памяти в PC */
    uint32_t irq_step; /** через сколько переданных отсчетов будет
                           генерироваться прерывание */
    uint32_t res2[3];
} t_lpcie_stream_ch_params;

/** параметры для установки циклического сигнала */
typedef struct {
    uint32_t ch; /** канал DMA (доступно только на вывод) */
    uint32_t size; /** количество отсчетов в циклическом сигнале */
    uint32_t res[2];  /** резерв */
} t_lpcie_cycle_set_par;

/** параметры для остановки/смены циклического сигнала */
typedef struct {
    uint32_t ch;    /** канал DMA (доступно только на вывод) */
    uint32_t evt;   /** событие для смены сигнала из #t_lpcie_cycle_sw_evt */
    uint32_t res[2]; /** резерв */
} t_lpcie_cycle_evt_par;

/** параметры для запроса LPCIE_IOCTL_CYCLE_CHECK_SETUP */
typedef struct {
    uint32_t ch;  /** канал DMA (доступно только на вывод) */
    uint32_t done;  /** признак, завершена ли установка циклического сигнала */
} t_lpcie_cycle_check_setup_par;

/** параметры запроса для получения количества готовых для ввода или вывода
    отсчетов */
typedef struct {
    uint32_t ch;  /** канал DMA (ввод/вывод) */
    uint32_t rdy_size; /** Количество отсчетов доступных на ввод или вывод */
} t_lpcie_get_rdy_par;


#ifdef _WIN32
#define LPCIE_IOCTL_GET_DRV_VERSION \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED , FILE_ANY_ACCESS)


#define LPCIE_IOCTL_MEMFPGA_RD \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED , FILE_ANY_ACCESS)

#define LPCIE_IOCTL_MEMFPGA_WR \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED , FILE_ANY_ACCESS)

#define LPCIE_IOCTL_POWER_DONW \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_BUFFERED , FILE_ANY_ACCESS)
#define LPCIE_IOCTL_RELOAD_DEVINFO \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x821, METHOD_BUFFERED , FILE_ANY_ACCESS)



#define LPCIE_IOCTL_STREAM_SET_PARAMS \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x840, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LPCIE_IOCTL_STREAM_GET_PARAMS \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x841, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LPCIE_IOCTL_STREAM_GET_RDY_SIZE \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x842, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define LPCIE_IOCTL_STREAM_START \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x844, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LPCIE_IOCTL_STREAM_STOP \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x845, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LPCIE_IOCTL_STREAM_FREE \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x846, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LPCIE_IOCTL_STREAM_START_SINGLE \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x847, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LPCIE_IOCTL_CYCLE_LOAD \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x850, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LPCIE_IOCTL_CYCLE_SWITCH \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x851, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LPCIE_IOCTL_CYCLE_STOP \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x852, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LPCIE_IOCTL_CYCLE_CHECK_SETUP \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x853, METHOD_BUFFERED, FILE_ANY_ACCESS)

#else

#define LPCIE_IO_MAGIC 'L'

#define LPCIE_IOCTL_GET_DRV_VERSION _IOR(LPCIE_IO_MAGIC, 0x800, uint32_t)

#define LPCIE_IOCTL_MEMFPGA_RD  _IOWR(LPCIE_IO_MAGIC, 0x810, t_lpcie_mem_rw)
#define LPCIE_IOCTL_MEMFPGA_WR  _IOW(LPCIE_IO_MAGIC, 0x811, t_lpcie_mem_rw)

#define LPCIE_IOCTL_POWER_DONW     _IO(LPCIE_IO_MAGIC, 0x820)
#define LPCIE_IOCTL_RELOAD_DEVINFO _IO(LPCIE_IO_MAGIC, 0x821)


#define LPCIE_IOCTL_STREAM_SET_PARAMS _IOW(LPCIE_IO_MAGIC, 0x840, t_lpcie_stream_ch_params)
#define LPCIE_IOCTL_STREAM_GET_PARAMS _IOWR(LPCIE_IO_MAGIC, 0x841, t_lpcie_stream_ch_params)
#define LPCIE_IOCTL_STREAM_GET_RDY_SIZE _IOWR(LPCIE_IO_MAGIC, 0x842, t_lpcie_get_rdy_par)


#define LPCIE_IOCTL_STREAM_START        _IOW(LPCIE_IO_MAGIC, 0x844, uint32_t)
#define LPCIE_IOCTL_STREAM_STOP         _IOW(LPCIE_IO_MAGIC, 0x845, uint32_t)
#define LPCIE_IOCTL_STREAM_FREE         _IOW(LPCIE_IO_MAGIC, 0x846, uint32_t)
#define LPCIE_IOCTL_STREAM_START_SINGLE _IOW(LPCIE_IO_MAGIC, 0x847, uint32_t)

#define LPCIE_IOCTL_CYCLE_LOAD          _IOW(LPCIE_IO_MAGIC, 0x850, t_lpcie_cycle_set_par)
#define LPCIE_IOCTL_CYCLE_SWITCH        _IOW(LPCIE_IO_MAGIC, 0x851, t_lpcie_cycle_evt_par)
#define LPCIE_IOCTL_CYCLE_STOP          _IOW(LPCIE_IO_MAGIC, 0x852, t_lpcie_cycle_evt_par)
#define LPCIE_IOCTL_CYCLE_CHECK_SETUP   _IOWR(LPCIE_IO_MAGIC, 0x853, t_lpcie_cycle_check_setup_par)

#endif






#endif // LPCIE_IOCTLS_H
