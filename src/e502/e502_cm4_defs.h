#ifndef E502_LPC_CMD_DEFS_H
#define E502_LPC_CMD_DEFS_H


typedef enum {
    E502_CM4_ERR_OK                     = 0,
    E502_CM4_ERR_FPGA_NSTATUS_TOUT      = -1001, /* при загрузке ПЛИС не дождались сигнала перехода в режим загрузки */
    E502_CM4_ERR_FPGA_CONF_DONE_TOUT    = -1002, /* при загрузке ПЛИС не дождались сигнала завершения загрузки */
    E502_CM4_ERR_FPGA_WF_NOT_PRESENT    = -1003, /* нет прошивки ПЛИС во flash-памяти */
    E502_CM4_ERR_FPGA_REG_NACK          = -1004, /* обращение к регистру ПЛИС вернуло NACK */
    E502_CM4_ERR_FPGA_REG_ERROR         = -1005, /* обращение к регистру ПЛИС вернуло ERROR */
    E502_CM4_ERR_FPGA_REG_WT_TOUT       = -1006, /* превышено ожидание завершения обращения к регистру ПЛИС */
    E502_CM4_ERR_TEST_INVALID_NUM       = -1007, /* неизвестный номер теста */
    E502_CM4_ERR_TEST_VALUE_MISMATH     = -1008, /* несовпадение ожидаемых значений при проходе теста */
    E502_CM4_ERR_TEST_NOT_RUNNING       = -1009, /* тест не запущен */
    E502_CM4_ERR_TEST_ALREADY_RUNNING   = -1010, /* тест уже запщен */
    E502_CM4_ERR_BF_LDR_FILE_SIZE       = -1011, /* не полный файл прошивки BlackFin */
    /** Неверный формат файла прошивки BlackFin */
    E502_CM4_ERR_LDR_FILE_FORMAT        = -1012,
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
    E502_CM4_ERR_BF_INVALID_CMD_DATA_SIZE     = -1022
} t_e502_cm4_errs;



typedef enum {
    E502_CM4_CMD_GET_MODULE_NAME     = 11,
    E502_CM4_CMD_GET_USB_SPEED       = 6,

    E502_CM4_CMD_FPGA_REG_READ       = 0x10,
    E502_CM4_CMD_FPGA_REG_WRITE      = 0x11,

    E502_CM4_CMD_TEST_START          = 0x40,
    E502_CM4_CMD_TEST_STOP           = 0x41,
    E502_CM4_CMD_TEST_GET_STATE      = 0x42,

    E502_CM4_CMD_GET_MODULE_INFO     = 0x80,
    E502_CM4_CMD_GET_MODULE_MODE     = 0x81,
    E502_CM4_CMD_GET_LAST_ERROR      = 0x82
} t_e502_cm4_cmd_codes;







typedef enum {
    E502_TEST_NONE=0,
    E502_TEST_SRAM_BUF_RING,
    E502_TEST_SRAM_SDRAM_RING_DMA,
    E502_TEST_USB_TX_CNTR,
    E502_TEST_USB_RING,
    E502_TEST_SPI_SLAVE
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
