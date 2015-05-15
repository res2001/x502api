#ifndef E502_LPC_CMD_DEFS_H
#define E502_LPC_CMD_DEFS_H


typedef enum {
    E502_CM4_ERR_OK                     = 0,
    E502_CM4_ERR_FPGA_NSTATUS_TOUT      = -1001, /**< При загрузке ПЛИС не удалось дождаться сигнала перехода в режим загрузки */
    E502_CM4_ERR_FPGA_CONF_DONE_TOUT    = -1002, /**< При загрузке ПЛИС не удалось дождаться сигнала завершения загрузки */
    E502_CM4_ERR_FPGA_FW_NOT_PRESENT    = -1003, /**< Не обнаружена прошивка ПЛИС во flash-памяти модуля */
    E502_CM4_ERR_FPGA_REG_NACK          = -1004, /**< Обращение к регистру ПЛИС вернуло ответ NACK */
    E502_CM4_ERR_FPGA_REG_ERROR         = -1005, /**< Обращение к регистру ПЛИС вернуло ответ ERROR */
    E502_CM4_ERR_FPGA_REG_WT_TOUT       = -1006, /**< Не удалось дожлаться ответ на обращение к регистру ПЛИС */
    E502_CM4_ERR_TEST_INVALID_NUM       = -1007, /**< Неподдерживаемый номер теста */
    E502_CM4_ERR_TEST_VALUE_MISMATH     = -1008, /**< Несовпадение ожидаемых значений при проходе теста */
    E502_CM4_ERR_TEST_NOT_RUNNING       = -1009, /**< Тест не запущен */
    E502_CM4_ERR_TEST_ALREADY_RUNNING   = -1010, /**< Tест уже запщен */
    E502_CM4_ERR_BF_LDR_FILE_SIZE       = -1011, /**< Не удалось найти конец файла прошивки BlackFin */
    E502_CM4_ERR_LDR_FILE_FORMAT        = -1012, /**< Неверный формат файла прошивки BlackFin */
    /** Используются возможность LDR-файла, недоступные при записи прошивки
        BlackFin по HDMA */
    E502_CM4_ERR_LDR_FILE_UNSUP_FEATURE       = -1013,
    /** Неверный стартовый адрес программы в прошивке BlackFin */
    E502_CM4_ERR_LDR_FILE_UNSUP_STARTUP_ADDR  = -1014,
    /** Истек таймаут выполнения запроса на чтения/запись памяти BlackFin */
    E502_CM4_ERR_BF_REQ_TIMEOUT               = -1015,
    /** Команда для BlackFin все еще находится в процессе обработки */
    E502_CM4_ERR_BF_CMD_IN_PROGRESS           = -1016,
    /** Истекло время выполнения управляющей команды процессором BlackFin */
    E502_CM4_ERR_BF_CMD_TIMEOUT               = -1017,
    /** Возвращено недостаточно данных в ответ на команду к BlackFin */
    E502_CM4_ERR_BF_CMD_RETURN_INSUF_DATA     = -1018,
    /** Истек таймаут ожидания готовности процессора BlackFin к записи прошивки */
    E502_CM4_ERR_BF_LOAD_RDY_TOUT             = -1019,
    /** Попытка выполнить операцию для которой нужен сигнальный процессор при
        отсутствии сигнального процессора в модуле */
    E502_CM4_ERR_BF_NOT_PRESENT               = -1020,
    /** Неверный адрес памяти BlackFin при записи или чтении по HDMA */
    E502_CM4_ERR_BF_INVALID_ADDR              = -1021,
    /** Неверный размер данных, передаваемых с управляющей командой в BlackFin */
    E502_CM4_ERR_BF_INVALID_CMD_DATA_SIZE     = -1022,
    E502_CM4_ERR_UNKNOWN_CMD                  = -1023, /**< Неподдерживаемый код команды */
    E502_CM4_ERR_INVALID_CMD_PARAMS           = -1024, /**< Неверные параметры переданной команды */
    E502_CM4_ERR_FIRM_BUF_OVERFLOW            = -1025, /**< Переполнение буфера для приема прошивки */
    E502_CM4_ERR_CMD_SIGNATURE                = -1026, /**< Неверный признак начала команды */
    E502_CM4_ERR_INVALID_CMD_DATA_SIZE        = -1027, /**< Неверное количество данных в команде */
    E502_CM4_ERR_FLASH_PROT_CODE              = -1028, /**< Неверный код настройки защиты Flash-памяти */
    E502_CM4_ERR_FLASH_OP                     = -1029, /**< Ошибка выполнения операции с Flash-памятью */
    E502_CM4_ERR_FLASH_DATA_COMPARE           = -1030, /**< Ошибка сравнения записанных данных во Flash-память */
    E502_CM4_ERR_INVALID_PASSWORD             = -1031, /**< Неверный пароль для изменения сетевых настроек */
    E502_CM4_ERR_FPGA_NOT_LOADED              = -1032, /**< ПЛИС не был загружен */
    E502_CM4_ERR_FLASH_SET_PROT_BITS          = -1033, /**< Не удалось изменить занчения битов защиты Flash-памяти */
    E502_CM4_ERR_FPGA_FW_INVALID_TEMP_RANGE   = -1034, /**< Загруженная прошивка ПЛИС предназначена для другого темп. исполнения */
    E502_CM4_ERR_M0_STREAM_START_REQ          = -1035, /**< Нет ответа на запрос запуска потока от ядра Cortex-M0 */
    E502_CM4_ERR_M0_STREAM_STOP_REQ           = -1036, /**< Нет ответа на запрос останова потока от ядра Cortex-M0 */
    E502_CM4_ERR_OUT_STREAM_RUNNING           = -1037, /**< Уже запущен вывод в потоковом режиме */
    E502_CM4_ERR_OUT_NO_CYCLE_BUF             = -1038, /**< Нет свободного буфера для циклического режима. Не произошла смена страниц */
    E502_CM4_ERR_OUT_CYCLE_BUF_SIZE           = -1039, /**< Задан слишком большой размер циклического буфера */
    E502_CM4_ERR_OUT_CYCLE_NOT_LOADED         = -1040, /**< Не был полностью загружен циклический буфер перед сменой */
} t_e502_cm4_errs;

typedef enum {
    E502_STREAM_CH_IN = 0,
    E502_STREAM_CH_OUT = 1
} t_e502_stream_ch;


typedef enum {
    E502_IFACE_USB = 0,
    E502_IFACE_TCP = 1
} t_e502_ifaces;





typedef enum {
    E502_CM4_CMD_GET_MODULE_NAME     = 11,
    E502_CM4_CMD_GET_USB_SPEED       = 6,

    E502_CM4_CMD_BOOT                = 0x0F,
    E502_CM4_CMD_FPGA_REG_READ       = 0x10,
    E502_CM4_CMD_FPGA_REG_WRITE      = 0x11,
    E502_CM4_CMD_STREAM_START        = 0x12,
    E502_CM4_CMD_STREAM_STOP         = 0x13,
    E502_CM4_CMD_STREAM_SET_STEP     = 0x14,
    E502_CM4_CMD_STREAM_IS_RUNNING   = 0x15,
    E502_CM4_CMD_FIRM_BUF_WRITE      = 0x16,
    E502_CM4_CMD_FLASH_RD            = 0x17,
    E502_CM4_CMD_FLASH_WR            = 0x18,
    E502_CM4_CMD_FLASH_ERASE         = 0x19,
    E502_CM4_CMD_FLASH_SET_PROTECT   = 0x1A,
    E502_CM4_CMD_FLASH_RELOAD_INFO   = 0x1B,
    E502_CM4_CMD_ETH_CFG_SET         = 0x1C,
    E502_CM4_CMD_ETH_CFG_GET         = 0x1D,
    E502_CM4_CMD_BF_MEM_WRITE        = 0x20,
    E502_CM4_CMD_BF_MEM_READ         = 0x21,
    E502_CM4_CMD_BF_FIRM_LOAD        = 0x22,
    E502_CM4_CMD_DROP_DATA_CON       = 0x23,
    E502_CM4_CMD_RELOAD_FPGA         = 0x24,
    E502_CM4_CMD_GET_DEVFLAGS        = 0x25,

    E502_CM4_CMD_OUT_CYCLE_LOAD      = 0x26,
    E502_CM4_CMD_OUT_CYCLE_SETUP     = 0x27,
    E502_CM4_CMD_OUT_CYCLE_STOP      = 0x28,


    E502_CM4_CMD_TEST_START          = 0x40,
    E502_CM4_CMD_TEST_STOP           = 0x41,
    E502_CM4_CMD_TEST_GET_STATE      = 0x42,

    E502_CM4_CMD_GET_MODULE_INFO     = 0x80,
    E502_CM4_CMD_GET_MODULE_MODE     = 0x81,
    E502_CM4_CMD_GET_LAST_ERROR      = 0x82
} t_e502_cm4_cmd_codes;


typedef enum {
    E502_CM4_TEST_NONE=0,
    E502_CM4_TEST_SRAM_BUF_RING,
    E502_CM4_TEST_SRAM_SDRAM_RING_DMA,
    E502_CM4_TEST_USB_TX_CNTR,
    E502_CM4_TEST_USB_RING,
    E502_CM4_TEST_SPI_SLAVE,
    E502_CM4_TEST_SDRAM,
    E502_CM4_TEST_ETH_PHY_LOOPBACK
} t_test_number;


typedef struct {
    uint32_t test; /**< Номер выполняемого теста */
    uint32_t run; /**< Признак, запущен ли сейчас тест */
    uint32_t stage; /**< Этап выполнения теста */
    uint32_t cntr; /**< Счетчик - сколько раз прошел тест */
    int32_t err; /**< Код ошибки выполнения теста */
    uint32_t last_addr; /**< Последний используемый адрес */
    uint32_t last_wr; /**< Последнее записанное значение */
    uint32_t last_rd; /**< Последнее считанное значение */
} t_e502_cm4_test_state;


#endif // E502_LPC_CMD_DEFS_H
