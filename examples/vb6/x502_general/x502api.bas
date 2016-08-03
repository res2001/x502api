Attribute VB_Name = "x502api"
' модуль для объявления констант, структур и функций, общих для модулей L502 и E502 от Л Кард

'Максимальное количество логических каналов в таблице
Public Const X502_LTABLE_MAX_CH_CNT = 256
'Количество диапазонов для измерения напряжений
Public Const X502_ADC_RANGE_CNT = 6

'Количество каналов АЦП в режиме с общей землей
Public Const X502_ADC_COMM_CH_CNT = 32

'Количество каналов АЦП в дифференциальном режиме
Public Const X502_ADC_DIFF_CH_CNT = 16

'Максимальное значение для аппаратного усреднения по логическому каналу
Public Const X502_LCH_AVG_SIZE_MAX = 128
'Максимальное значения делителя частоты АЦП
Public Const X502_ADC_FREQ_DIV_MAX = 1024& * 1024&
'Максимальное значение делителя частоты синхронного цифрового ввода
Public Const X502_DIN_FREQ_DIV_MAX = 1024& * 1024&

'Минимальное значение делителя частоты синхронного вывода
Public Const X502_OUT_FREQ_DIV_MIN = 2
'Максимальное значение делителя частоты синхронного вывода
Public Const X502_OUT_FREQ_DIV_MAX = 1024
'Значение делителя частоты вывода по умолчанию (которое также всегда
'используется в L502 с версией прошивки ПЛИС ниже 0.5)
Public Const X502_OUT_FREQ_DIV_DEFAULT = 2

'Максимальное значение межкадровой задержки для АЦП
Public Const X502_ADC_INTERFRAME_DELAY_MAX = &H1FFFFF

'Таймаут по умолчанию для выполнения команды к BlackFin
Public Const X502_BF_CMD_DEFAULT_TOUT = 500
 
'Код АЦП, соответствующий максимальному значению шкалы
Public Const X502_ADC_SCALE_CODE_MAX = 6000000
'Код ЦАП, соответствующий максимальному значению шкалы
Public Const X502_DAC_SCALE_CODE_MAX = 30000

'Максимальное количество символов в строке с названием устройства
Public Const X502_DEVNAME_SIZE = 32
'Максимальное количество символов в строке с серийным номером
Public Const X502_SERIAL_SIZE = 32
'Максимальное количество символов в строке с описанием подключения
Public Const X502_LOCATION_STR_SIZE = 64
'Размер MAC-адреса для Ethernet интерфейса
Public Const X502_MAC_ADDR_SIZE = 6
'Размер строки с описанием экземпляра устройства
Public Const X502_INSTANCE_NAME_SIZE = 64
'Максимальный размер строки с паролем на настройки
Public Const X502_PASSWORD_SIZE = 32

'Максимально возможное значение внешней опорной частоты
Public Const X502_EXT_REF_FREQ_MAX = 1500000
'Размер пользовательской области Flash-памяти
Public Const X502_FLASH_USER_SIZE = &H100000

'Cтандартный таймаут на выполнение запроса к BlackFin в мс
Public Const X502_BF_REQ_TOUT = 500

'Диапазон ЦАП в вольтах
Public Const X502_DAC_RANGE = 5#

'Количество каналов ЦАП
Public Const X502_DAC_CH_CNT = 2
 
'Количество цифровых выходов у модуля
Public Const X502_DOUT_LINES_CNT = 16
'слово в потоке, означающее, что произошло переполнение
Public Const X502_STREAM_IN_MSG_OVERFLOW = &H1010000

'Значение поля сигнатуры в записи о устройстве #t_x502_devrec.
Public Const X502_DEVREC_SIGN = &H4C524543

'Коды ошибок библиотеки
Public Enum t_lpcie_errs
    'Функция выполнена без ошибок
    X502_ERR_OK = 0
    'В функцию передан недействительный описатель модуля
    X502_ERR_INVALID_HANDLE = -1
    'Ошибка выделения памяти
    X502_ERR_MEMORY_ALLOC = -2
    'Попытка открыть уже открытое устройство
    X502_ERR_ALREADY_OPENED = -3
    'Устройство с заданными параметрами не найдено в системе
    X502_ERR_DEVICE_NOT_FOUND = -4
    'Доступ к устройству запрещен (Как правило из-за того, что устройство уже открыто в другой программе)
    X502_ERR_DEVICE_ACCESS_DENIED = -5
    'Ошибка открытия устройства
    X502_ERR_DEVICE_OPEN = -6
    'В функцию передан недействительный указатель
    X502_ERR_INVALID_POINTER = -7
    'Функция не может быть выполнена при запущенном потоке сбора данных
    X502_ERR_STREAM_IS_RUNNING = -8
    'Ошибка чтения данных синхронного ввода
    X502_ERR_RECV = -9
    'Ошибка записи данных для синхронного вывода
    X502_ERR_SEND = -10
    'Произошло переполнение внутреннего буфера для потока синхронного ввода
    X502_ERR_STREAM_OVERFLOW = -11
    'Неизвестное сообщение в потоке синхронного ввода
    X502_ERR_UNSUP_STREAM_MSG = -12
    'Ошибка создания системного мьютекса
    X502_ERR_MUTEX_CREATE = -13
    'Неверный описатель мьютекса
    X502_ERR_MUTEX_INVALID_HANDLE = -14
    'Истекло время ожидания освобождения мьютекса
    X502_ERR_MUTEX_LOCK_TOUT = -15
    'Ошибка освобождения мьютекса
    X502_ERR_MUTEX_RELEASE = -16
    'Недостаточно системных ресурсов
    X502_ERR_INSUFFICIENT_SYSTEM_RESOURCES = -17
    'Данная возможность еще не реализована
    X502_ERR_NOT_IMPLEMENTED = -18
    'Недостаточный размер массива
    X502_ERR_INSUFFICIENT_ARRAY_SIZE = -19
    'Ошибка чтения регистра FPGA
    X502_ERR_FPGA_REG_READ = -20
    'Ошибка записи регистра FPGA
    X502_ERR_FPGA_REG_WRITE = -21
    'Сбор данных уже остановлен
    X502_ERR_STREAM_IS_NOT_RUNNING = -22
    'Ошибка освобождения интерфейса
    X502_ERR_INTERFACE_RELEASE = -23
    'Ошибка запуска потока
    X502_ERR_THREAD_START = -24
    'Ошибка останова потока
    X502_ERR_THREAD_STOP = -25
    'Устройство было отключено
    X502_ERR_DEVICE_DISCONNECTED = -26
    'Неверный размер ответа на управляющий запрос
    X502_ERR_IOCTL_INVALID_RESP_SIZE = -27
    'Неверный тип устройства
    X502_ERR_INVALID_DEVICE = -28
    'Недействительная запись о устройстве
    X502_ERR_INVALID_DEVICE_RECORD = -29
    'Неверный описатель конфигурации модуля
    X502_ERR_INVALID_CONFIG_HANDLE = -30
    'Связь с устройством закрыта или не была установлена
    X502_ERR_DEVICE_NOT_OPENED = -31
    'Данная операция не доступна для текущего интерфейса связи с устройством
    X502_ERR_INVALID_OP_FOR_IFACE = -32
    'Не загружен ПЛИС модуля
    X502_ERR_FPGA_NOT_LOADED = -33
    'Неверная конфигурация USB-устройства
    X502_ERR_INVALID_USB_CONFIGURATION = -34
    'Неверный описатель контекста поиска устройств в сети
    X502_ERR_INVALID_SVC_BROWSE_HANDLE = -35
    'Неверный описатель записи о сервисе
    X502_ERR_INVALID_SVC_RECORD_HANDLE = -36
    'Не запущена программа обнаружения устройств в локальной сети
    X502_ERR_DNSSD_NOT_RUNNING = -37
    'Ошибка при обращении к программе обнаружения устройств в локальной сети
    X502_ERR_DNSSD_COMMUNICATION = -38
    'Превышен таймаут запроса параметров автообнаруженного сетевого устройства
    X502_ERR_SVC_RESOLVE_TIMEOUT = -39
    'Ошибка в кодировке имени экземпляра устройства
    X502_ERR_INSTANCE_NAME_ENCODING = -40
    'Экземпляры модулей не совпадают
    X502_ERR_INSTANCE_MISMATCH = -41
    'Возможность не поддерживается текущей версией прошивки устройства
    X502_ERR_NOT_SUP_BY_FIRMWARE = -42
    'Возможность не поддерживается текущей версией драйвера устройства
    X502_ERR_NOT_SUP_BY_DRIVER = -43
    'Превышено время ожидания установления циклического сигнала на вывод
    X502_ERR_OUT_CYCLE_SETUP_TOUT = -44
    'Неизвестный код поддерживаемой возможности
    X502_ERR_UNKNOWN_FEATURE_CODE = -45


    'Задан неверный размер логической таблицы
    X502_ERR_INVALID_LTABLE_SIZE = -102
    'Задан неверный номер логического канала
    X502_ERR_INVALID_LCH_NUMBER = -103
    'Неверно задано значение диапазона АЦП
    X502_ERR_INVALID_LCH_RANGE = -104
    'Неверно задан режим измерения для логического канала
    X502_ERR_INVALID_LCH_MODE = -105
    'Неверно задан номер физического канала при настройке логического
    X502_ERR_INVALID_LCH_PHY_NUMBER = -106
    'Неверно задан размер усреднения для логического канала
    X502_ERR_INVALID_LCH_AVG_SIZE = -107
    'Неверно задан делитель частоты сбора данных АЦП
    X502_ERR_INVALID_ADC_FREQ_DIV = -108
    'Неверно задан делитель частоты синхронного ввода цифровых линий
    X502_ERR_INVALID_DIN_FREQ_DIV = -109
    'Неверно задан режим работы модуля X502
    X502_ERR_INVALID_MODE = -110
    'Неверный номер канала ЦАП
    X502_ERR_INVALID_DAC_CHANNEL = -111
    'Неверный код выбора опорной частоты синхронизации
    X502_ERR_INVALID_REF_FREQ = -112
    'Неверно задано значение межкадровой задержки
    X502_ERR_INVALID_INTERFRAME_DELAY = -113
    'Неверно задан режим синхронизации
    X502_ERR_INVALID_SYNC_MODE = -114
    'Неверно задан номер потока данных
    X502_ERR_INVALID_STREAM_CH = -115
    'Неверно задан делитель частоты синхронного вывода
    X502_ERR_INVALID_OUT_FREQ_DIV = -116

    'Ошибка захвата опорной частоты синхронизации
    X502_ERR_REF_FREQ_NOT_LOCKED = -131
    'Управляющий запрос к драйверу завершен с ошибкой
    X502_ERR_IOCTL_FAILD = -132
    'Истек таймаут ожидания завершения выполнения управляющего запроса к драйверу
    X502_ERR_IOCTL_TIMEOUT = -133
    'Ошибка получения информации о устройстве от драйвера
    X502_ERR_GET_INFO = -134
    'За время ожидания не было считано новое слово с цифровых линий
    X502_ERR_DIG_IN_NOT_RDY = -135
    'Принято недостаточно слов от модуля
    X502_ERR_RECV_INSUFFICIENT_WORDS = -136
    'Попытка выполнить операцию, требующую наличие ЦАП, при его отсутствии
    X502_ERR_DAC_NOT_PRESENT = -137
    'Передано недостаточно слов в модуль
    X502_ERR_SEND_INSUFFICIENT_WORDS = -138
    'Не пришло ответа на переданную команду
    X502_ERR_NO_CMD_RESPONSE = -139

    'Неверный номер канала в обрабатываемом потоке синхронного ввода
    X502_ERR_PROC_INVALID_CH_NUM = -140
    'Неверный код диапазона в обрабатываемом потоке синхронного ввода
    X502_ERR_PROC_INVALID_CH_RANGE = -141
    'Задан неверный адрес во Flash-памяти
    X502_ERR_FLASH_INVALID_ADDR = -142
    'Задан неверный размер блока данных при работе с Flash-памятью
    X502_ERR_FLASH_INVALID_SIZE = -143
    'Истек таймаут ожидания завершения записи во Flash-память
    X502_ERR_FLASH_WRITE_TOUT = -144
    'Истек таймаут ожидания завершения стирания блока Flash-памяти
    X502_ERR_FLASH_ERASE_TOUT = -145
    'Заданная область для стирания Flash-памяти нарушает границу блока в 4 Кбайт
    X502_ERR_FLASH_SECTOR_BOUNDARY = -146

    'Не удалось открыть сокет для соединения
    X502_ERR_SOCKET_OPEN = -147
    'Превышено время подключения
    X502_ERR_CONNECTION_TOUT = -148
    'Соединение закрыто другой устройством
    X502_ERR_CONNECTION_CLOSED_BY_DEV = -149
    'Не удалось установить заданный размер буфера сокета
    X502_ERR_SOCKET_SET_BUF_SIZE = -150
    'Соединение для передачи данных не установлено
    X502_ERR_NO_DATA_CONNECTION = -151
    'Не удалось дождаться сообщения о завершении потока
    X502_ERR_NO_STREAM_END_MSG = -152
    'Соединение было сброшено другой стороной
    X502_ERR_CONNECTION_RESET = -153
    'Не удалось найти хост с указанным адресом
    X502_ERR_HOST_UNREACHABLE = -154
    'Ошибка установления TCP-соединения
    X502_ERR_TCP_CONNECTION_ERROR = -155

    'Не удалось открыть файл прошивки BlackFin
    X502_ERR_LDR_FILE_OPEN = -180
    'Ошибка чтения из фала прошивки BlackFin
    X502_ERR_LDR_FILE_READ = -181
    'Неверный формат файла прошивки BlackFin
    X502_ERR_LDR_FILE_FORMAT = -182
    'Используются возможность LDR-файла, недоступные при записи прошивки BlackFin по HDMA
    X502_ERR_LDR_FILE_UNSUP_FEATURE = -183
    'Неверный стартовый адрес программы в прошивке BlackFin
    X502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR = -184
    'Истек таймаут выполнения запроса на чтения/запись памяти BlackFin
    X502_ERR_BF_REQ_TIMEOUT = -185
    'Команда для BlackFin все еще находится в процессе обработки
    X502_ERR_BF_CMD_IN_PROGRESS = -186
    'Истекло время выполнения управляющей команды процессором BlackFin
    X502_ERR_BF_CMD_TIMEOUT = -187
    'Возвращено недостаточно данных в ответ на команду к BlackFin
    X502_ERR_BF_CMD_RETURN_INSUF_DATA = -188
    'Истек таймаут ожидания готовности процессора BlackFin к записи прошивки
    X502_ERR_BF_LOAD_RDY_TOUT = -189
    'Попытка выполнить операцию для которой нужен сигнальный процессор при отсутствии сигнального процессора в модуле
    X502_ERR_BF_NOT_PRESENT = -190
    'Неверный адрес памяти BlackFin при записи или чтении по HDMA
    X502_ERR_BF_INVALID_ADDR = -191
    'Неверный размер данных, передаваемых с управляющей командой в BlackFin
    X502_ERR_BF_INVALID_CMD_DATA_SIZE = -192
End Enum


'Интерфейс соединения с модулем
Public Enum t_x502_iface
    X502_IFACE_UNKNOWN = 0 'Неизвестный интерфейс
    X502_IFACE_USB = 1     'Устройство подключено по USB
    X502_IFACE_ETH = 2     'Устройство подключено по Ethernet через TCP/IP
    X502_IFACE_PCI = 3     'Устройство подключено по PCI/PCIe
End Enum

'Флаги, управляющие поиском присутствующих модулей
Public Enum t_x502_getdevs_flags
   ' Признак, что нужно вернуть серийные номера только тех устройств,  которые еще не открыты
    X502_GETDEVS_FLAGS_ONLY_NOT_OPENED = &H1&
End Enum


'Флаги для управления цифровыми выходами.
Public Enum t_x502_digout_word_flags
    X502_DIGOUT_WORD_DIS_H = &H20000      'Запрещение (перевод в третье состояние)  старшей половины цифровых выходов
    X502_DIGOUT_WORD_DIS_L = &H10000  'Запрещение младшей половины цифровых выходов
End Enum

'Константы для выбора опорной частоты
Public Enum t_x502_ref_freq
    X502_REF_FREQ_2000KHZ = 2000000   'Частота 2МГц
    X502_REF_FREQ_1500KHZ = 1500000  'Частота 1.5МГц
End Enum

'Диапазоны измерения для канала АЦП
Public Enum t_x502_adc_range
    X502_ADC_RANGE_10 = 0 'Диапазон +/-10V
    X502_ADC_RANGE_5 = 1  'Диапазон +/-5V
    X502_ADC_RANGE_2 = 2  'Диапазон +/-2V
    X502_ADC_RANGE_1 = 3  'Диапазон +/-1V
    X502_ADC_RANGE_05 = 4 'Диапазон +/-0.5V
    X502_ADC_RANGE_02 = 5  'Диапазон +/-0.2V
End Enum

'Режим измерения для логического канала
Public Enum t_x502_lch_mode
    X502_LCH_MODE_COMM = 0  'Измерение напряжения относительно общей земли
    X502_LCH_MODE_DIFF = 1 'Дифференциальное измерение напряжения
    X502_LCH_MODE_ZERO = 2  'Измерение собственного нуля
End Enum

'Режимы синхронизации.
Public Enum t_x502_sync_mode
    X502_SYNC_INTERNAL = 0        'Внутренний сигнал
    X502_SYNC_EXTERNAL_MASTER = 1 'От внешнего мастера по разъему межмодульной синхронизации
    X502_SYNC_DI_SYN1_RISE = 2    'По фронту сигнала DI_SYN1
    X502_SYNC_DI_SYN1_FALL = 3    'По фронту сигнала DI_SYN2
    X502_SYNC_DI_SYN2_RISE = 6    'По спаду сигнала DI_SYN1
    X502_SYNC_DI_SYN2_FALL = 7    'По спаду сигнала DI_SYN2
End Enum

'Флаги, управляющие обработкой принятых данных
Public Enum t_x502_proc_flags
    'Признак, что нужно преобразовать значения АЦП в вольты
    X502_PROC_FLAGS_VOLT = &H1&
    ' Признак, что не нужно проверять совпадение номеров каналов
    '   в принятых данных с каналами из логической таблицы.
    '   Может использоваться при нестандартной прошивке BlackFin
    '    при передаче в ПК не всех данных.
    X502_PROC_FLAGS_DONT_CHECK_CH = &H10000
End Enum

'Флаги для обозначения синхронных потоков данных
Public Enum t_x502_streams
    X502_STREAM_ADC = &H1&  'Поток данных от АЦП
    X502_STREAM_DIN = &H2&   'Поток данных с цифровых входов
    X502_STREAM_DAC1 = &H10& 'Поток данных первого канала ЦАП
    X502_STREAM_DAC2 = &H20& 'Поток данных второго канала ЦАП
    X502_STREAM_DOUT = &H40& 'Поток данных на цифровые выводы
    'Объединение всех флагов, обозначающих потоки данных на ввод
    X502_STREAM_ALL_IN = X502_STREAM_ADC Or X502_STREAM_DIN
    'Объединение всех флагов, обозначающих потоки данных на вывод
    X502_STREAM_ALL_OUT = X502_STREAM_DAC1 Or X502_STREAM_DAC2 Or X502_STREAM_DOUT
End Enum

'Константы, определяющие тип передаваемого отсчета из ПК в модуль
Public Enum t_x502_stream_out_wrd_type
    X502_STREAM_OUT_WORD_TYPE_DOUT = &H0&          'Цифровой вывод
    X502_STREAM_OUT_WORD_TYPE_DAC1 = &H40000000    'Код для 1-го канала ЦАП
    X502_STREAM_OUT_WORD_TYPE_DAC2 = &H80000000    'Код для 2-го канала ЦАП
End Enum

'Режим работы модуля
Public Enum t_x502_mode
    X502_MODE_FPGA = 0  'Все потоки данных передаются через ПЛИС минуя
                        'сигнальный процессор BlackFin
    X502_MODE_DSP = 1   'Все потоки данных передаются через сигнальный
                        'процессор, который должен быть загружен
                        'прошивкой для обработки этих потоков
    X502_MODE_DEBUG = 2  'Отладочный режим
End Enum

'Номера каналов ЦАП.
Public Enum t_x502_dac_ch
   X502_DAC_CH1 = 0 'Первый канал ЦАП
   X502_DAC_CH2 = 1 'Второй канал ЦАП
End Enum

'Флаги, используемые при выводе данных на ЦАП.
 Public Enum t_x502_dacout_flags
    'Указывает, что значение задано в Вольтах и при выводе его нужно
    '    перевести его в коды ЦАП. Если флаг не указан, то считается, что значение
    '    изначально в кодах
    X502_DAC_FLAGS_VOLT = &H1&
    'Указывает, что нужно применить калибровочные коэффициенты перед
    '    выводом значения на ЦАП.
    X502_DAC_FLAGS_CALIBR = &H2&
End Enum

'Номера каналов для передачи потоков данных
Public Enum t_x502_stream_ch
    X502_STREAM_CH_IN = 0  'Общий канал на ввод
    X502_STREAM_CH_OUT = 1 'Общий канал на вывод
End Enum

'Цифровые линии, на которых можно включить подтягивающие резисторы
Public Enum t_x502_pullups
    X502_PULLUPS_DI_H = &H1        'Старшая половина цифровых входов (только для L502)
    X502_PULLUPS_DI_L = &H2        'Младшая половина цифровых входов (только для L502)
    X502_PULLUPS_DI_SYN1 = &H4     'Линия SYN1
    X502_PULLUPS_DI_SYN2 = &H8     'Линия SYN2
    X502_PULLDOWN_CONV_IN = &H10   'Подтяжка к 0 линии межмодульной
                                   ' синхронизации CONV_IN (только для E502)
    X502_PULLDOWN_START_IN = &H20  'Подтяжка к 0 линии межмодульной
                                   ' синхронизации START_IN (только для E502)
End Enum

'Флаги, определяющие наличие опций в модуле
Public Enum t_x502_dev_flags
    'Признак наличия двухканального канального ЦАП
    X502_DEVFLAGS_DAC_PRESENT = &H1&
    'Признак наличия гальваноразвязки
    X502_DEVFLAGS_GAL_PRESENT = &H2&
    'Признак наличия сигнального процессора BlackFin
    X502_DEVFLAGS_BF_PRESENT = &H4&

    'Признак, что устройство поддерживает интерфейс USB
    X502_DEVFLAGS_IFACE_SUPPORT_USB = &H100&
    'Признак, что устройство поддерживает Ethernet
    X502_DEVFLAGS_IFACE_SUPPORT_ETH = &H200&
    'Признак, что устройство поддерживает интерфейс PCI/PCI-Express
    X502_DEVFLAGS_IFACE_SUPPORT_PCI = &H400&

    'Признак, что устройство выполнено в индустриалном исполнении
    X502_DEVFLAGS_INDUSTRIAL = &H8000&

    'Признак, что во Flash-памяти присутствует информация о модуле
    X502_DEVFLAGS_FLASH_DATA_VALID = &H10000
    'Признак, что во Flash-памяти присутствуют действительные калибровочные коэффициенты АЦП
    X502_DEVFLAGS_FLASH_ADC_CALIBR_VALID = &H20000
    'Признак, что во Flash-памяти присутствуют действительные калибровочные коэффициенты ЦАП
    X502_DEVFLAGS_FLASH_DAC_CALIBR_VALID = &H40000

    'Признак, что присутствует прошивка ПЛИС и она успешно была загружена
    X502_DEVFLAGS_FPGA_LOADED = &H800000
    'Признак, что устройство уже открыто (действителен только внутри t_x502_devrec)
    X502_DEVFLAGS_DEVREC_OPENED = &H1000000
End Enum

Public Enum t_x502_location_type
    'В поле расположения устройства не содержится информации
    X502_LOCATION_TYPE_NONE = 0
    'В поле расположения устройства содержится строка с адресом устройства
    X502_LOCATION_TYPE_ADDR = 1
    'В поле расположения устройства содержится строка с именем экземпляра
    X502_LOCATION_TYPE_INSTANCE_NAME = 2
End Enum

'Флаги для режима циклического вывода
Public Enum t_x502_out_cycle_flags
    'останов или смена сигнала могут произойти без ожидания конца цикла предыдущего сигнала
    X502_OUT_CYCLE_FLAGS_FORCE = &H1&
    'функция должна дождаться полной загрузки сигнала
    ' и установки сигнала на вывод (для X502_OutCycleSetup()) или завершения
    ' генерации циклического сигнала (для X502_OutCycleStop())
    X502_OUT_CYCLE_FLAGS_WAIT_DONE = &H2&
End Enum

'Коды возможностей модуля, которые могут поддерживаться или нет в зависимости от типа модуля, версий прошивок и т.п
Public Enum t_x502_features
    'Поддержка установки делителя частоты вывода, отличного от X502_OUT_FREQ_DIV_DEFAULT
    X502_FEATURE_OUT_FREQ_DIV = 1
    'Возможность чтения флагов состояния вывода с помощью X502_OutGetStatusFlags()
    X502_FEATURE_OUT_STATUS_FLAGS = 2
End Enum

'Флаги состояния для синхронного вывода
Public Enum t_x502_out_status_flags
    'Флаг указывает, что в настоящее время буфер в модуле на передачу пуст
    X502_OUT_STATUS_FLAG_BUF_IS_EMPTY = &H1&
    'Флаг указывает, что было опустошение буфера на вывод с начала старта синхронного
    ' ввода-вывода или с момента последнего чтения статуса с помощью
    ' X502_OutGetStatusFlags() (в зависимости от того, что было последним)
    X502_OUT_STATUS_FLAG_BUF_WAS_EMPTY = &H2&
End Enum

'--------------- Типы данных ---------------

'Запись о устройстве
Public Type t_x502_devrec
    sign As Long  'Признак действительной структуры
    DevName(0 To X502_DEVNAME_SIZE - 1) As Byte 'Название устройства
    serial(0 To X502_SERIAL_SIZE - 1) As Byte 'Серийный номер
    location(0 To X502_LOCATION_STR_SIZE - 1) As Byte 'Описание подключения (если есть)
    flags As Long 'Флаги из #t_x502_dev_flags, описывающие устройство
    iface As Byte 'Интерфейс, по которому подключено устройство
    location_type As Byte  'Определяет, что именно сохранено в поле location (одно значение из #t_x502_location_type)
    res(0 To 122 - 1) As Byte 'Резерв
    internal As Long 'Непрозрачный указатель на структуру с дополнительной информацией
End Type


' Калибровочные коэффициенты диапазона
Public Type t_x502_cbr_coef
    offs As Double 'смещение нуля
    k As Double 'коэффициент шкалы
End Type


'Калибровочные коэффициенты модуля
Public Type t_x502_cbr
    adc(0 To X502_ADC_RANGE_CNT - 1) As t_x502_cbr_coef 'Калибровочные коэффициенты АЦП
    res1(0 To 64 - 1) As Long 'Резерв
    dac(0 To X502_DAC_CH_CNT - 1) As t_x502_cbr_coef 'Калибровочные коэффициенты ЦАП
    res2(0 To 20 - 1) As Long
End Type

' Информация о модуле L502/E502
Public Type t_x502_info
    DevName(0 To X502_DEVNAME_SIZE - 1) As Byte 'Название устройства ("L502" или "E502")
    serial(0 To X502_SERIAL_SIZE - 1) As Byte 'Серийный номер
    devflags As Long 'Флаги из #t_x502_dev_flags, описывающие наличие в модуле определенных опций
    fpga_ver As Integer 'Версия ПЛИС (старший байт - мажорная, младший - минорная)
    plda_ver As Byte 'Версия ПЛИС, управляющего аналоговой частью
    board_rev As Byte 'Ревизия платы
    mcu_firmware_ver As Long 'Версия прошивки контроллера Cortex-M4. Действительна только для E502
    factory_mac(0 To X502_MAC_ADDR_SIZE - 1) As Byte 'Заводской MAC-адрес --- действителен только для
                                                     'устройств с Ethernet-интерфейсом */
    res(0 To 110 - 1) As Byte 'Резерв
    cbr As t_x502_cbr      'Заводские калибровочные коэффициенты (из Flash-памяти)
End Type


Public Declare Function X502_Create Lib "x502api" () As Long
Public Declare Function X502_Free Lib "x502api" (ByVal hnd As Long) As Long

Public Declare Function X502_OpenByDevRecord Lib "x502api" (ByVal hnd As Long, ByRef devrec As t_x502_devrec) As Long
Private Declare Function tmp_X502_FreeDevRecordList Lib "x502api" Alias "X502_FreeDevRecordList" (ByRef list As t_x502_devrec, ByVal size As Long) As Long


Public Declare Function X502_Close Lib "x502api" (ByVal hnd As Long) As Long
Public Declare Function X502_GetDevInfo Lib "x502api" (ByVal hnd As Long, ByRef info As t_x502_info) As Long


Public Declare Function X502_Configure Lib "x502api" (ByVal hnd As Long, ByVal flags As Long) As Long
Public Declare Function X502_SetLChannel Lib "x502api" (ByVal hnd As Long, ByVal lch As Long, ByVal phy_ch As Long, ByVal mode As Long, ByVal range As Long, ByVal avg As Long) As Long
'Установка количества логических каналов.
Public Declare Function X502_SetLChannelCount Lib "x502api" (ByVal hnd As Long, ByVal lch_cnt As Long) As Long
'Получение количества логических каналов.
Public Declare Function X502_GetLChannelCount Lib "x502api" (ByVal hnd As Long, ByRef lch_cnt As Long) As Long
'Установка делителя частоты сбора для АЦП
Public Declare Function X502_SetAdcFreqDivider Lib "x502api" (ByVal hnd As Long, ByVal adc_freq_div As Long) As Long
'Установка значения межкадровой задержки для АЦП.
Public Declare Function X502_SetAdcInterframeDelay Lib "x502api" (ByVal hnd As Long, ByVal delay As Long) As Long
'Установка делителя частоты синхронного ввода с цифровых линий.
Public Declare Function X502_SetDinFreqDivider Lib "x502api" (ByVal hnd As Long, ByVal din_freq_div As Long) As Long
'Установка делителя частоты синхронного вывода
Public Declare Function X502_SetOutFreqDivider Lib "x502api" (ByVal hnd As Long, ByVal out_freq_div As Long) As Long
'Установка частоты сбора АЦП.
Public Declare Function X502_SetAdcFreq Lib "x502api" (ByVal hnd As Long, ByRef f_acq As Double, ByRef f_frame As Double) As Long
'Установка частоты синхронного ввода с цифровых входов.
Public Declare Function X502_SetDinFreq Lib "x502api" (ByVal hnd As Long, ByRef f_din As Double) As Long
'Установка частоты синхронного вывода.
Public Declare Function X502_SetOutFreq Lib "x502api" (ByVal hnd As Long, ByRef f_dout As Double) As Long
'Получить текущие значения частот сбора АЦП
Public Declare Function X502_GetAdcFreq Lib "x502api" (ByVal hnd As Long, ByRef f_acq As Double, ByRef f_frame As Double) As Long
'Установка значения опорной частоты синхронизации.
Public Declare Function X502_SetRefFreq Lib "x502api" (ByVal hnd As Long, ByVal freq As Long) As Long
'Установка значения внешней опорной частоты синхронизации
Public Declare Function X502_SetExtRefFreqValue Lib "x502api" (ByVal hnd As Long, ByVal freq As Double) As Long
'Получение значения опорной частоты синхронизации
Public Declare Function X502_GetRefFreqValue Lib "x502api" (ByVal hnd As Long, ByRef freq As Double) As Long
'Установка режима генерации частоты синхронизации.
Public Declare Function X502_SetSyncMode Lib "x502api" (ByVal hnd As Long, ByVal sync_mode As Long) As Long
'Установка режима запуска частоты синхронизации
Public Declare Function X502_SetSyncStartMode Lib "x502api" (ByVal hnd As Long, ByVal sync_start_mode As Long) As Long
'Установить режим работы модуля.
Public Declare Function X502_SetMode Lib "x502api" (ByVal hnd As Long, ByVal mode As Long) As Long
'Получение текущего режима работы модуля.
Public Declare Function X502_GetMode Lib "x502api" (ByVal hnd As Long, ByRef mode As Long) As Long
'Установить коэффициенты для калибровки значений АЦП
Public Declare Function X502_SetAdcCoef Lib "x502api" (ByVal hnd As Long, ByVal range As Long, ByVal k As Double, ByVal offs As Double) As Long
'Получение текущих калибровочных коэффициентов АЦП.
Public Declare Function X502_GetAdcCoef Lib "x502api" (ByVal hnd As Long, ByVal range As Long, ByRef k As Double, ByRef offs As Double) As Long
'Установить коэффициенты для калибровки значений ЦАП.
Public Declare Function X502_SetDacCoef Lib "x502api" (ByVal hnd As Long, ByVal ch As Long, ByVal k As Double, ByVal offs As Double) As Long
'Получение текущих калибровочных коэффициентов ЦАП
Public Declare Function X502_GetDacCoef Lib "x502api" (ByVal hnd As Long, ByVal ch As Long, ByRef k As Double, ByRef offs As Double) As Long

'Асинхронный вывод данных на канал ЦАП.
Public Declare Function X502_AsyncOutDac Lib "x502api" (ByVal hnd As Long, ByVal ch As Long, ByVal data As Double, ByVal flags As Long) As Long
'Асинхронный вывод данных на цифровые выходы.
Public Declare Function X502_AsyncOutDig Lib "x502api" (ByVal hnd As Long, ByVal Val As Long, ByVal msk As Long) As Long
'Асинхронный ввод значений с цифровых входов.
Public Declare Function X502_AsyncInDig Lib "x502api" (ByVal hnd As Long, ByRef din As Long) As Long

Private Declare Function tmp_X502_AsyncGetAdcFrame Lib "x502api" Alias "X502_AsyncGetAdcFrame" (ByVal hnd As Long, ByVal flags As Long, ByVal tout As Long, ByRef data As Double) As Long

'Разрешение синхронных потоков на ввод/вывод.
Public Declare Function X502_StreamsEnable Lib "x502api" (ByVal hnd As Long, ByVal streams As Long) As Long
'Запрещение синхронных потоков на ввод/вывод.
Public Declare Function X502_StreamsDisable Lib "x502api" (ByVal hnd As Long, ByVal streams As Long) As Long
'Получить значение, какие синхронные потоки разрешены
Public Declare Function X502_GetEnabledStreams Lib "x502api" (ByVal hnd As Long, ByRef streams As Long) As Long
'Запуск синхронных потоков ввода/вывода.
Public Declare Function X502_StreamsStart Lib "x502api" (ByVal hnd As Long) As Long
'Останов синхронных потоков ввода/вывода.
Public Declare Function X502_StreamsStop Lib "x502api" (ByVal hnd As Long) As Long
'Проверка, запущен ли синхронный ввод/вывод
Public Declare Function X502_IsRunning Lib "x502api" (ByVal hnd As Long) As Long
'Чтение данных АЦП и цивровых входов из модуля.
Private Declare Function tmp_X502_Recv Lib "x502api" Alias "X502_Recv" (ByVal hnd As Long, ByRef buf As Long, ByVal size As Long, ByVal tout As Long) As Long
'Передача потоковых данных ЦАП и цифровых выходов в модуль.
Private Declare Function tmp_X502_Send Lib "x502api" Alias "X502_Send" (ByVal hnd As Long, ByRef buf As Long, ByVal size As Long, ByVal tout As Long) As Long
'Обработка принятых отсчетов АЦП от модуля.
Private Declare Function tmp_X502_ProcessAdcData Lib "x502api" Alias "X502_ProcessAdcData" (ByVal hnd As Long, ByRef src As Long, ByRef dest As Double, ByRef size As Long, ByVal flags As Long) As Long
'Обработка принятых от модуля данных.
Private Declare Function tmp_X502_ProcessData Lib "x502api" Alias "X502_ProcessData" (ByVal hnd As Long, ByRef src As Long, ByVal flags As Long, ByRef adc_data As Double, ByRef adc_data_size As Long, ByRef din_data As Long, ByRef din_data_size As Long) As Long
'Обработка принятых от модуля данных с пользовательскими данными.
Private Declare Function tmp_X502_ProcessDataWithUserExt Lib "x502api" Alias "X502_ProcessDataWithUserExt" (ByVal hnd As Long, ByRef src As Long, ByVal size As Long, ByVal flags As Long, ByRef adc_data As Double, ByRef adc_data_size As Long, ByRef din_data As Long, ByRef din_data_size As Long, ByRef usr_data As Long, ByRef usr_data_size As Long) As Long
'Подготовка данных для вывода в модуль.
Private Declare Function tmp_X502_PrepareData Lib "x502api" Alias "X502_PrepareData" (ByVal hnd As Long, ByRef dac1 As Double, ByRef dac2 As Double, ByRef digout As Long, ByVal size As Long, ByVal flags As Long, ByRef out_buf As Long) As Long

'Получить количество отсчетов в буфере потока на ввод.
Public Declare Function X502_GetRecvReadyCount Lib "x502api" (ByVal hnd As Long, ByRef rdy_cnt As Long) As Long
'Получить размер свободного места в буфере потока на вывод.
Public Declare Function X502_GetSendReadyCount Lib "x502api" (ByVal hnd As Long, ByRef rdy_cnt As Long) As Long
'Получить номер следующего ожидаемого логического канала АЦП для  обработки.
Public Declare Function X502_GetNextExpectedLchNum Lib "x502api" (ByVal hnd As Long, ByRef lch As Long) As Long
'Начало подготовки вывода синхронных данных
Public Declare Function X502_PreloadStart Lib "x502api" (ByVal hnd As Long) As Long
'Начало загрузки циклического сигнала на вывод
Public Declare Function X502_OutCycleLoadStart Lib "x502api" (ByVal hnd As Long, ByVal size As Long) As Long
'Установка ранее загруженного циклического сигнала на вывод
Public Declare Function X502_OutCycleSetup Lib "x502api" (ByVal hnd As Long, ByVal flags As Long) As Long
'Останов вывода циклического сигнала
Public Declare Function X502_OutCycleStop Lib "x502api" (ByVal hnd As Long, ByVal flags As Long) As Long
'Проверка, завершена ли установка или останов циклического сигнала
Public Declare Function X502_OutCycleCheckSetupDone Lib "x502api" (ByVal hnd As Long, ByRef done As Long) As Long
'Чтение флагов статуса вывода
Public Declare Function X502_OutGetStatusFlags Lib "x502api" (ByVal hnd As Long, ByRef status As Long) As Long
'Установка размера буфера в драйвере для синхронного ввода или вывода.
Public Declare Function X502_SetStreamBufSize Lib "x502api" (ByVal hnd As Long, ByVal dma_ch As Long, ByVal size As Long) As Long
'Установка шага прерывания при передаче потока по DMA.
Public Declare Function X502_SetStreamStep Lib "x502api" (ByVal hnd As Long, ByVal dma_ch As Long, ByVal step As Long) As Long

'Загрузка прошивки сигнального процессора BlackFin.
Public Declare Function X502_BfLoadFirmware Lib "x502api" (ByVal hnd As Long, ByVal filename As String) As Long
'Проверка, загружена ли прошивка BlackFIn.
Public Declare Function X502_BfCheckFirmwareIsLoaded Lib "x502api" (ByVal hnd As Long, ByRef version As Long) As Long
'Чтение блока данных из памяти сигнального процессора.
Private Declare Function tmp_X502_BfMemRead Lib "x502api" Alias "X502_BfMemRead" (ByVal hnd As Long, ByVal addr As Long, ByRef regs As Long, ByVal size As Long) As Long
'Запись блока данных в память сигнального процессора.
Private Declare Function tmp_X502_BfMemWrite Lib "x502api" Alias "X502_BfMemWrite" (ByVal hnd As Long, ByVal addr As Long, ByRef regs As Long, ByVal size As Long) As Long
'Передача управляющей команды сигнальному процессору.
Private Declare Function tmp_X502_BfExecCmd Lib "x502api" Alias "X502_BfExecCmd" (ByVal hnd As Long, ByVal cmd_code As Integer, ByVal par As Long, ByRef snd_data As Long, ByVal snd_size As Long, ByRef rcv_data As Long, ByVal rcv_size As Long, ByVal tout As Long, ByRef recvd_size As Long) As Long

'Чтение блока данных из Flash-памяти.
 Private Declare Function tmp_X502_FlashRead Lib "x502api" Alias "X502_FlashRead" (ByVal hnd As Long, ByVal addr As Long, ByRef data As Byte, ByVal size As Long) As Long
 'Запись блока данных во Flash-память модуля.
 Private Declare Function tmp_X502_FlashWrite Lib "x502api" Alias "X502_FlashWrite" (ByVal hnd As Long, ByVal addr As Long, ByRef data As Byte, ByVal size As Long) As Long
'Стирание блока во Flash-памяти.
 Public Declare Function X502_FlashErase Lib "x502api" (ByVal hnd As Long, ByVal addr As Long, ByVal size As Long) As Long
 'Разрешение записи в пользовательскую область Flash-памяти.
 Public Declare Function X502_FlashWriteEnable Lib "x502api" (ByVal hnd As Long) As Long
 'Запрет записи в пользовательскую область Flash-памяти.
 Public Declare Function X502_FlashWriteDisable Lib "x502api" (ByVal hnd As Long) As Long
 
 'Получить версию библиотеки.
Public Declare Function X502_GetLibraryVersion Lib "x502api" () As Long
'Моргание светодиодом на передней панели.
Public Declare Function X502_LedBlink Lib "x502api" (ByVal hnd As Long) As Long
'Установка подтягивающих резисторов на входных линиях.
Public Declare Function X502_SetDigInPullup Lib "x502api" (ByVal hnd As Long, ByVal pullups As Long) As Long
'Проверка поддержки модулем заданной возможности
Public Declare Function X502_CheckFeature Lib "x502api" (ByVal hnd As Long, ByVal feature As Long) As Long


Private Declare Function tmp_X502_GetErrorString Lib "x502api" Alias "X502_GetErrorString" (ByVal Error_Code As Long) As Long


Private Declare Function lstrlen Lib "kernel32" Alias "lstrlenA" (ByVal lpString As Long) As Long
Private Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (Destination As Any, Source As Any, ByVal Length As Long)
Private Declare Function GlobalAlloc Lib "kernel32" (ByVal wFlags As Long, ByVal dwBytes As Long) As Long
Private Declare Function GlobalFree Lib "kernel32" (ByVal hMem As Long) As Long




Public Function X502_FreeDevRecordList(ByRef list() As t_x502_devrec, ByVal size As Long) As Long
    X502_FreeDevRecordList = tmp_X502_FreeDevRecordList(list(0), size)
End Function

Public Function X502_FreeDevRecord(ByRef rec As t_x502_devrec) As Long
    Dim list(0 To 0) As t_x502_devrec
    list(0) = rec
    X502_FreeDevRecord = X502_FreeDevRecordList(list, 1)
End Function



Public Function X502_AsyncGetAdcFrame(ByVal hnd As Long, ByVal flags As Long, ByVal tout As Long, ByRef data() As Double) As Long
    X502_AsyncGetAdcFrame = tmp_X502_AsyncGetAdcFrame(hnd, flags, tout, data(0))
End Function

Public Function X502_Recv(ByVal hnd As Long, ByRef buf() As Long, ByVal size As Long, ByVal tout As Long) As Long
    X502_Recv = tmp_X502_Recv(hnd, buf(0), size, tout)
End Function

Public Function X502_Send(ByVal hnd As Long, ByRef buf() As Long, ByVal size As Long, ByVal tout As Long) As Long
    X502_Send = tmp_X502_Send(hnd, buf(0), size, tout)
End Function


Public Function X502_ProcessAdcData(ByVal hnd As Long, ByRef src() As Long, ByRef dest() As Double, ByRef size As Long, ByVal flags As Long) As Long
    X502_ProcessAdcData = tmp_X502_ProcessAdcData(hnd, src(0), dest(0), size, flags)
End Function

Public Function X502_ProcessData(ByVal hnd As Long, ByRef src() As Long, ByVal flags As Long, ByRef adc_data() As Double, ByRef adc_data_size As Long, ByRef din_data() As Long, ByRef din_data_size As Long) As Long
    X502_ProcessData = tmp_X502_ProcessData(hnd, src(0), flags, adc_data(0), adc_data_size, din_data(0), din_data_size)
End Function

Public Function X502_ProcessDataWithUserExt(ByVal hnd As Long, ByRef src() As Long, ByVal size As Long, ByVal flags As Long, ByRef adc_data() As Double, ByRef adc_data_size As Long, ByRef din_data() As Long, ByRef din_data_size As Long, ByRef usr_data() As Long, ByRef usr_data_size As Long) As Long
    X502_ProcessDataWithUserExt = tmp_X502_ProcessDataWithUserExt(hnd, src(0), size, flags, adc_data(0), adc_data_size, din_data(0), din_data_size, usr_data(0), usr_data_size)
End Function

Public Function X502_PrepareData(ByVal hnd As Long, ByRef dac1() As Double, ByRef dac2() As Double, ByRef digout() As Long, ByVal size As Long, ByVal flags As Long, ByRef out_buf() As Long) As Long
    X502_PrepareData = tmp_X502_PrepareData(hnd, dac1(0), dac2(0), digout(0), size, flags, out_buf(0))
End Function

Public Function X502_BfMemRead(ByVal hnd As Long, ByVal addr As Long, ByRef regs() As Long, ByVal size As Long) As Long
    X502_BfMemRead = tmp_X502_BfMemRead(hnd, addr, regs(0), size)
End Function

Public Function X502_BfMemWrite(ByVal hnd As Long, ByVal addr As Long, ByRef regs() As Long, ByVal size As Long) As Long
    X502_BfMemWrite = tmp_X502_BfMemWrite(hnd, addr, regs(0), size)
End Function

Public Function X502_BfExecCmd(ByVal hnd As Long, ByVal cmd_code As Integer, ByVal par As Long, ByRef snd_data() As Long, ByVal snd_size As Long, ByRef rcv_data() As Long, ByVal rcv_size As Long, ByVal tout As Long, ByRef recvd_size As Long) As Long
    X502_BfExecCmd = tmp_X502_BfExecCmd(hnd, cmd_code, par, snd_data(0), snd_size, rcv_data(0), rcv_size, tout, recvd_size)
End Function

Public Function X502_FlashRead(ByVal hnd As Long, ByVal addr As Long, ByRef data() As Byte, ByVal size As Long) As Long
    X502_FlashRead = tmp_X502_FlashRead(hnd, addr, data(0), size)
End Function
 
 Public Function X502_FlashWrite(ByVal hnd As Long, ByVal addr As Long, ByRef data() As Byte, ByVal size As Long) As Long
    X502_FlashWrite = tmp_X502_FlashWrite(hnd, addr, data(0), size)
 End Function


Public Function X502_StrConvert(ByRef str_arr() As Byte) As String
  Dim psCString As String
  Dim sReturn As String
  Dim iNullCharPos As Integer
  
  psCString = StrConv(str_arr, vbUnicode)

  iNullCharPos = InStr(psCString, vbNullChar)

  If iNullCharPos > 0 Then
    sReturn = Left(psCString, iNullCharPos - 1)
  Else
    sReturn = psCString
  End If

  X502_StrConvert = sReturn

End Function


Public Function X502_StrFromPtr(ByVal lpStr As Long) As String
    Dim bStr() As Byte
    Dim cChars As Long
    On Error Resume Next
    ' Get the number of characters in the buffer
    cChars = lstrlen(lpStr)
    If cChars Then
     ' Resize the byte array
     ReDim bStr(0 To cChars - 1) As Byte
     ' Grab the ANSI buffer
     Call CopyMemory(bStr(0), ByVal lpStr, cChars)
    End If
    ' Now convert to a VB Unicode string
    X502_StrFromPtr = StrConv(bStr, vbUnicode)
End Function

Public Function X502_GetErrorString(ByVal Error_Code As Long) As String
    Dim Ptr As Long
    Ptr = tmp_X502_GetErrorString(Error_Code)
    X502_GetErrorString = X502_StrFromPtr(Ptr)
End Function
