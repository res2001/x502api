//---------------------------------------------------------------------------
#include "x502api.h"
#include "l502_bf_cmd_defs.h"

static const char* f_unknow_err = "Неизвестная ошибка.";

typedef struct
{
    int32_t err;
    const char* str;
}t_err_table;

static const t_err_table f_err_tbl[] = {
    { X502_ERR_OK,                      "Выполнено без ошибок" },
    { X502_ERR_INVALID_HANDLE,          "Недействительный описатель модуля"},
    { X502_ERR_MEMORY_ALLOC,            "Ошибка выделения памяти"},
    { X502_ERR_ALREADY_OPENED,          "Попытка открыть уже открытое устройство"},
    { X502_ERR_DEVICE_NOT_FOUND,        "Устройство с заданными параметрами не найдено в системе"},
    { X502_ERR_DEVICE_ACCESS_DENIED,    "Доступ к устройству запрещен"},
    { X502_ERR_DEVICE_OPEN,             "Ошибка открытия устройства"},
    { X502_ERR_INVALID_POINTER,         "В функцию передан недействительный указатель"},
    { X502_ERR_STREAM_IS_RUNNING,       "Функция не может быть выполнена при запущенном потоке сбора данных"},
    { X502_ERR_RECV,                    "Ошибка чтения данных синхронного ввода"},
    { X502_ERR_SEND,                    "Ошибка записи данных для синхронного вывода"},
    { X502_ERR_STREAM_OVERFLOW,         "Произошло переполнение внутреннего буфера для потока синхронного ввода"},
    { X502_ERR_UNSUP_STREAM_MSG,        "Неизвестное сообщение в потоке синхронного ввода"},
    { X502_ERR_MUTEX_CREATE,            "Ошибка создания системного мьютекса"},
    { X502_ERR_MUTEX_INVALID_HANDLE,    "Неверный описатель мьютекса"},
    { X502_ERR_MUTEX_LOCK_TOUT,         "Истекло время ожидания освобождния мьютекса"},
    { X502_ERR_MUTEX_RELEASE,           "Ошибка освобождения мьютекса"},
    { X502_ERR_INSUFFICIENT_SYSTEM_RESOURCES, "Недостаточно системных ресурсов"},
    { X502_ERR_NOT_IMPLEMENTED,         "Данная возможность еще не реализована"},
    { X502_ERR_INSUFFICIENT_ARRAY_SIZE, "Недостаточный размер массива"},
    { X502_ERR_FPGA_REG_READ,           "Ошибка чтения регистра FPGA"},
    { X502_ERR_FPGA_REG_WRITE,          "Ошибка записи регистра FPGA"},
    { X502_ERR_STREAM_IS_NOT_RUNNING,   "Синхронный сбор уже остановлен"},
    { X502_ERR_INTERFACE_RELEASE,       "Ошибка освобождения интерфейса"},
    { X502_ERR_THREAD_START,            "Ошибка запуска потока"},
    { X502_ERR_THREAD_STOP,             "Ошибка останова потока"},
    { X502_ERR_INVALID_LTABLE_SIZE,     "Задан неверный размер логической таблицы"},
    { X502_ERR_INVALID_LCH_NUMBER,      "Задан неверный номер логического канала"},
    { X502_ERR_INVALID_LCH_RANGE,       "Неверно задано значение диапазона АЦП"},
    { X502_ERR_INVALID_LCH_MODE,        "Неверно задан режим измерения для логического канала"},
    { X502_ERR_INVALID_LCH_PHY_NUMBER,  "Неверно задан номер физического канала при настройке логического"},
    { X502_ERR_INVALID_LCH_AVG_SIZE,    "Неверно задан размер усреднения для логического канала"},
    { X502_ERR_INVALID_ADC_FREQ_DIV,    "Неверно задан делитель частоты сбора данных АЦП"},
    { X502_ERR_INVALID_DIN_FREQ_DIV,    "Неверно задан делитель частоты синхронного ввода цифровых линий"},
    { X502_ERR_INVALID_MODE,            "Неверно задан режим работы платы"},
    { X502_ERR_INVALID_DAC_CHANNEL,     "Неверный номер канала ЦАП"},
    { X502_ERR_INVALID_REF_FREQ,        "Неверный код выбора опорной частоты синхронизации"},
    { X502_ERR_INVALID_INTERFRAME_DELAY,"Неверно задано значение межкадровой задержки"},
    { X502_ERR_INVALID_SYNC_MODE,       "Неверно задан режим синхронизации"},
    { X502_ERR_INVALID_STREAM_CH,       "Неверно задан номер потока данных"},
    { X502_ERR_INVALID_OUT_FREQ_DIV,    "Неверно задан делитель частоты синхронного вывода"},
    { X502_ERR_REF_FREQ_NOT_LOCKED,     "Ошибка захвата опорной частоты синхронизации"},
    { X502_ERR_IOCTL_FAILD,             "Управляющий запрос к драйверу завершен с ошибкой"},
    { X502_ERR_IOCTL_TIMEOUT,           "Истек таймаут ожидания завершения выполнения управляющего запроса к драйверу"},
    { X502_ERR_GET_INFO,                "Ошибка получения информации о устройстве от драйвера"},
    { X502_ERR_DIG_IN_NOT_RDY,          "За время ожидания не было считано новое слово с цифровых линий"},
    { X502_ERR_RECV_INSUFFICIENT_WORDS, "Принято недостаточно слов от модуля"},
    { X502_ERR_DAC_NOT_PRESENT,         "Попытка выполнить операцию, требующую наличие ЦАП, при его отсутствии"},
    { X502_ERR_PROC_INVALID_CH_NUM,     "Неверный номер канала в обрабатываемом потоке синхронного ввода"},
    { X502_ERR_PROC_INVALID_CH_RANGE,   "Неверный код диапазона в обрабатываемом потоке синхронного ввода"},
    { X502_ERR_FLASH_INVALID_ADDR,      "Задан неверный адрес во Flash-памяти"},
    { X502_ERR_FLASH_INVALID_SIZE,      "Задан неверный размер блока данных при работе с Flash-памятью"},
    { X502_ERR_FLASH_WRITE_TOUT,        "Истек таймаут ожидания завершения записи во Flash-память"},
    { X502_ERR_FLASH_ERASE_TOUT,        "Истек таймаут ожидания завершения стирания блока Flash-памяти"},
    { X502_ERR_FLASH_SECTOR_BOUNDARY,   "Заданная область для стирания Flash-памяти нарушает границу блока в 4 Кбайт"},
    { X502_ERR_LDR_FILE_OPEN,           "Не удалось открыть файл прошивки BlackFin"},
    { X502_ERR_LDR_FILE_READ,           "Ошибка чтения из фала прошивки BlackFin"},
    { X502_ERR_LDR_FILE_FORMAT,         "Неверный формат файла прошивки BlackFin"},
    { X502_ERR_LDR_FILE_UNSUP_FEATURE,  "Используются возможность LDR-файла, недоступные при записи прошивки BlackFin по HDMA"},
    { X502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR, "Неверный стартовый адрес программы в прошивке BlackFin"},
    { X502_ERR_BF_REQ_TIMEOUT,          "Истек таймаут выполнения запроса на чтения/запись памяти BlackFin"},
    { X502_ERR_BF_CMD_IN_PROGRESS,      "Команда для BlackFin все еще находится в процессе обработки"},
    { X502_ERR_BF_CMD_TIMEOUT,          "Истекло время выполнения управляющей команды процессором BlackFin"},
    { X502_ERR_BF_CMD_RETURN_INSUF_DATA,"Возвращено недостаточно данных в ответ на команду к BlackFin"},
    { X502_ERR_BF_LOAD_RDY_TOUT,        "Истек таймаут ожидания готовности процессора BlackFin к записи прошивки"},
    { X502_ERR_BF_NOT_PRESENT,          "Процессор BlackFin не присутствует на плате"},
    { X502_ERR_BF_INVALID_ADDR,         "Неверный адрес памяти BlackFin при записи или чтении по HDMA"},
    { X502_ERR_BF_INVALID_CMD_DATA_SIZE,"Неверный размер данных, передаваемых с управляющей командой в BlackFin"},

    { L502_BF_ERR_UNSUP_CMD,            "Ошибка BlackFin: неподдерживаемый  код команды"},
    { L502_BF_ERR_CMD_OVERRUN,          "Ошибка BlackFin: запрос на выполнение команды опережает завершение предыдущей"},
    { L502_BF_ERR_INVALID_CMD_PARAMS,   "Ошибка BlackFin: неверное значение параметров команды"},
    { L502_BF_ERR_INSUF_CMD_DATA,       "Ошибка BlackFin: недостаточное данных передано с командой"},
    { L502_BF_ERR_STREAM_RUNNING,       "Ошибка BlackFin: команда не допустима при запущеном сборе"},
    { L502_BF_ERR_STREAM_STOPPED,       "Ошибка BlackFin: команда допустима только при запущеном сборе"},
    { L502_BF_ERR_NO_TEST_IN_PROGR,     "Ошибка BlackFin: не выполняется ни одного теста"},
    { L502_BF_ERR_TEST_VALUE,           "Ошибка BlackFin: неверное значение при выполнении теста"}
};


LPCIE_EXPORT(const char*) X502_GetErrorString(int32_t err) {
    size_t i;
    const char* str = f_unknow_err;

    for (i=0; (i < sizeof(f_err_tbl)/sizeof(f_err_tbl[0])) && (str==f_unknow_err); i++) {
        if (f_err_tbl[i].err == err)
            str = f_err_tbl[i].str;
    }
    return str;
}
