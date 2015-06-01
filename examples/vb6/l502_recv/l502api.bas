Attribute VB_Name = "l502api"
' модуль для объявления констант, структур и функций  для управления модулем L502 от Л Кард

'Максимальное количество логических каналов в таблице
Const L502_LTABLE_MAX_CH_CNT = 256
'Количество диапазонов для измерения напряжений
Const L502_ADC_RANGE_CNT = 6
'Максимальное значение для аппаратного усреднения по логическому каналу
Const L502_LCH_AVG_SIZE_MAX = 128
'Максимальное значения делителя частоты АЦП
Const L502_ADC_FREQ_DIV_MAX = 1024& * 1024&
'Максимальное значение делителя частоты синхронного цифрового ввода
Const L502_DIN_FREQ_DIV_MAX = 1024& * 1024&

'Максимальное значение межкадровой задержки для АЦП
Const L502_ADC_INTERFRAME_DELAY_MAX = &H1FFFFF

'Таймаут по умолчанию для выполнения команды к BlackFin
Const L502_BF_CMD_DEFAULT_TOUT = 500
 
'Код АЦП, соответствующий максимальному значению шкалы
Const L502_ADC_SCALE_CODE_MAX = 6000000
'Код ЦАП, соответствующий максимальному значению шкалы
Const L502_DAC_SCALE_CODE_MAX = 30000

'Максимальное количество символов в строке с названием устройства
Const L502_DEVNAME_SIZE = 32
'Максимальное количество символов в строке с серийным номером
Const L502_SERIAL_SIZE = 32

'Максимально возможное значение внешней опорной частоты
Const L502_EXT_REF_FREQ_MAX = 2000000
'Размер пользовательской области Flash-памяти
Const L502_FLASH_USER_SIZE = &H100000

'Cтандартный таймаут на выполнение запроса к BlackFin в мс
Const L502_BF_REQ_TOUT = 500

'Диапазон ЦАП в вольтах
Const L502_DAC_RANGE = 5#

'Количество каналов ЦАП
Const L502_DAC_CH_CNT = 2
 
'слово в потоке, означающее, что произошло переполнение
Const L502_STREAM_IN_MSG_OVERFLOW = &H1010000




Public Enum t_lpcie_errs
    L502_ERR_OK = 0
    'В функцию передан недействительный описатель модуля
    L502_ERR_INVALID_HANDLE = -1
    'Ошибка выделения памяти
    L502_ERR_MEMORY_ALLOC = -2
    'Попытка открыть уже открытое устройство
    L502_ERR_ALREADY_OPENED = -3
    'Устройство с заданными параметрами не найдено в системе
    L502_ERR_DEVICE_NOT_FOUND = -4
    'Доступ к устройству запрещен (Как правило из-за того, что устройство уже открыто в другой программе)
    L502_ERR_DEVICE_ACCESS_DENIED = -5
    'Ошибка открытия устройства
    L502_ERR_DEVICE_OPEN = -6
    'В функцию передан недействительный указатель
    L502_ERR_INVALID_POINTER = -7
    'Функция не может быть выполнена при запущенном потоке сбора данных
    L502_ERR_STREAM_IS_RUNNING = -8
    'Ошибка чтения данных синхронного ввода
    L502_ERR_RECV = -9
    'Ошибка записи данных для синхронного вывода
    L502_ERR_SEND = -10
    'Произошло переполнение внутреннего буфера для потока синхронного ввода
    L502_ERR_STREAM_OVERFLOW = -11
    'Неизвестное сообщение в потоке синхронного ввода
    L502_ERR_UNSUP_STREAM_MSG = -12
    'Ошибка создания системного мьютекса
    L502_ERR_MUTEX_CREATE = -13
    'Неверный описатель мьютекса
    L502_ERR_MUTEX_INVALID_HANDLE = -14
    'Истекло время ожидания освобождения мьютекса
    L502_ERR_MUTEX_LOCK_TOUT = -15
    'Ошибка освобождения мьютекса
    L502_ERR_MUTEX_RELEASE = -16
    'Недостаточно системных ресурсов
    L502_ERR_INSUFFICIENT_SYSTEM_RESOURCES = -17
    'Данная возможность еще не реализована
    L502_ERR_NOT_IMPLEMENTED = -18
    'Недостаточный размер массива
    L502_ERR_INSUFFICIENT_ARRAY_SIZE = -19
    'Ошибка чтения регистра FPGA
    L502_ERR_FPGA_REG_READ = -20
    'Ошибка записи регистра FPGA
    L502_ERR_FPGA_REG_WRITE = -21
    'Сбор данных уже остановлен
    L502_ERR_STREAM_IS_NOT_RUNNING = -22
    'Задан неверный размер логической таблицы
    L502_ERR_INVALID_LTABLE_SIZE = -102
    'Задан неверный номер логического канала
    L502_ERR_INVALID_LCH_NUMBER = -103
    'Неверно задано значение диапазона АЦП
    L502_ERR_INVALID_LCH_RANGE = -104
    'Неверно задан режим измерения для логического канала
    L502_ERR_INVALID_LCH_MODE = -105
    'Неверно задан номер физического канала при настройке логического
    L502_ERR_INVALID_LCH_PHY_NUMBER = -106
    'Неверно задан размер усреднения для логического канала
    L502_ERR_INVALID_LCH_AVG_SIZE = -107
    'Неверно задан делитель частоты сбора данных АЦП
    L502_ERR_INVALID_ADC_FREQ_DIV = -108
    'Неверно задан делитель частоты синхронного ввода цифровых линий
    L502_ERR_INVALID_DIN_FREQ_DIV = -108
    'Неверно задан режим работы модуля L502
    L502_ERR_INVALID_MODE = -109
    'Неверный номер канала ЦАП
    L502_ERR_INVALID_DAC_CHANNEL = -110
    'Неверный код выбора опорной частоты синхронизации
    L502_ERR_INVALID_REF_FREQ = -111
    'Неверно задано значение межкадровой задержки
    L502_ERR_INVALID_INTERFRAME_DELAY = -112
    'Неверно задан режим синхронизации
    L502_ERR_INVALID_SYNC_MODE = -113
    'Неверно задан номер канала DMA
    L502_ERR_INVALID_DMA_CH = -114

    'Ошибка захвата опорной частоты синхронизации
    L502_ERR_REF_FREQ_NOT_LOCKED = -131
    'Управляющий запрос к драйверу завершен с ошибкой
    L502_ERR_IOCTL_FAILD = -132
    'Истек таймаут ожидания завершения выполнения управляющего запроса к драйверу
    L502_ERR_IOCTL_TIMEOUT = -133
    'Ошибка получения информации о устройстве от драйвера
    L502_ERR_GET_INFO = -134
    'За время ожидания не было считано новое слово с цифровых линий
    L502_ERR_DIG_IN_NOT_RDY = -135
    'Принято недостаточно слов от модуля
    L502_ERR_RECV_INSUFFICIENT_WORDS = -136
    'Попытка выполнить операцию, требующую наличие ЦАП, при его отсутствии
    L502_ERR_DAC_NOT_PRESENT = -137
    'Неверный номер канала в обрабатываемом потоке синхронного ввода
    L502_ERR_PROC_INVALID_CH_NUM = -140
    'Неверный код диапазона в обрабатываемом потоке синхронного ввода
    L502_ERR_PROC_INVALID_CH_RANGE = -141
    'Задан неверный адрес во Flash-памяти
    L502_ERR_FLASH_INVALID_ADDR = -142
    'Задан неверный размер блока данных при работе с Flash-памятью
    L502_ERR_FLASH_INVALID_SIZE = -143
    'Истек таймаут ожидания завершения записи во Flash-память
    L502_ERR_FLASH_WRITE_TOUT = -144
    'Истек таймаут ожидания завершения стирания блока Flash-памяти
    L502_ERR_FLASH_ERASE_TOUT = -145
    'Заданная область для стирания Flash-памяти нарушает границу блока в 4 Кбайт
    L502_ERR_FLASH_SECTOR_BOUNDARY = -146
    'Не удалось открыть файл прошивки BlackFin
    L502_ERR_LDR_FILE_OPEN = -180
    'Ошибка чтения из фала прошивки BlackFin
    L502_ERR_LDR_FILE_READ = -181
    'Неверный формат файла прошивки BlackFin
    L502_ERR_LDR_FILE_FORMAT = -182
    'Используются возможность LDR-файла, недоступные при записи прошивки BlackFin по HDMA
    L502_ERR_LDR_FILE_UNSUP_FEATURE = -183
    'Неверный стартовый адрес программы в прошивке BlackFin
    L502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR = -184
    'Истек таймаут выполнения запроса на чтения/запись памяти BlackFin
    L502_ERR_BF_REQ_TIMEOUT = -185
    'Команда для BlackFin все еще находится в процессе обработки
    L502_ERR_BF_CMD_IN_PROGRESS = -186
    'Истекло время выполнения управляющей команды процессором BlackFin
    L502_ERR_BF_CMD_TIMEOUT = -187
    'Возвращено недостаточно данных в ответ на команду к BlackFin
    L502_ERR_BF_CMD_RETURN_INSUF_DATA = -188
    'Истек таймаут ожидания готовности процессора BlackFin к записи прошивки
    L502_ERR_BF_LOAD_RDY_TOUT = -189
    'Попытка выполнить операцию для которой нужен сигнальный процессор при
    'отсутствии сигнального процессора в модуле
    L502_ERR_BF_NOT_PRESENT = -190
    'Неверный адрес памяти BlackFin при записи или чтении по HDMA
    L502_ERR_BF_INVALID_ADDR = -191
    'Неверный размер данных, передаваемых с управляющей командой в BlackFin
    L502_ERR_BF_INVALID_CMD_DATA_SIZE = -192
End Enum


'Флаги, управляющие поиском присутствующих модулей
Public Enum t_l502_getdevs_flags
   ' Признак, что нужно вернуть серийные номера только тех устройств,  которые еще не открыты
    L502_GETDEVS_FLAGS_ONLY_NOT_OPENED = &H1&
End Enum


'Флаги для управления цифровыми выходами.
Public Enum t_l502_digout_word_flags
    L502_DIGOUT_WORD_DIS_H = &H20000      'Запрещение (перевод в третье состояние)  старшей половины цифровых выходов
    L502_DIGOUT_WORD_DIS_L = &H10000  'Запрещение младшей половины цифровых выходов
End Enum

'Константы для выбора опорной частоты
Public Enum t_l502_ref_freq
    L502_REF_FREQ_2000KHZ = 2000000   'Частота 2МГц
    L502_REF_FREQ_1500KHZ = 1500000  'Частота 1.5МГц
End Enum

'Диапазоны измерения для канала АЦП
Public Enum t_l502_adc_range
    L502_ADC_RANGE_10 = 0 'Диапазон +/-10V
    L502_ADC_RANGE_5 = 1  'Диапазон +/-5V
    L502_ADC_RANGE_2 = 2  'Диапазон +/-2V
    L502_ADC_RANGE_1 = 3  'Диапазон +/-1V
    L502_ADC_RANGE_05 = 4 'Диапазон +/-0.5V
    L502_ADC_RANGE_02 = 5  'Диапазон +/-0.2V
End Enum

'Режим измерения для логического канала
Public Enum t_l502_lch_mode
    L502_LCH_MODE_COMM = 0  'Измерение напряжения относительно общей земли
    L502_LCH_MODE_DIFF = 1 'Дифференциальное измерение напряжения
    L502_LCH_MODE_ZERO = 2  'Измерение собственного нуля
End Enum

'Режимы синхронизации.
Public Enum t_l502_sync_mode
    L502_SYNC_INTERNAL = 0        'Внутренний сигнал
    L502_SYNC_EXTERNAL_MASTER = 1 'От внешнего мастера по разъему синхронизации
    L502_SYNC_DI_SYN1_RISE = 2    'По фронту сигнала DI_SYN1
    L502_SYNC_DI_SYN2_RISE = 3    'По фронту сигнала DI_SYN2
    L502_SYNC_DI_SYN1_FALL = 6    'По спаду сигнала DI_SYN1
    L502_SYNC_DI_SYN2_FALL = 7     'По спаду сигнала DI_SYN2
End Enum

'Флаги, управляющие обработкой принятых данных
Public Enum t_l502_proc_flags
    'Признак, что нужно преобразовать значения АЦП в вольты
    L502_PROC_FLAGS_VOLT = &H1&
    ' Признак, что не нужно проверять совпадение номеров каналов
    '   в принятых данных с каналами из логической таблицы.
    '   Может использоваться при нестандартной прошивке BlackFin
    '    при передаче в ПК не всех данных.
    L502_PROC_FLAGS_DONT_CHECK_CH = &H10000
End Enum

'Флаги для обозначения синхронных потоков данных
Public Enum t_l502_streams
    L502_STREAM_ADC = &H1&  'Поток данных от АЦП
    L502_STREAM_DIN = &H2&   'Поток данных с цифровых входов
    L502_STREAM_DAC1 = &H10& 'Поток данных первого канала ЦАП
    L502_STREAM_DAC2 = &H20& 'Поток данных второго канала ЦАП
    L502_STREAM_DOUT = &H40& 'Поток данных на цифровые выводы
    'Объединение всех флагов, обозначающих потоки данных на ввод
    L502_STREAM_ALL_IN = L502_STREAM_ADC Or L502_STREAM_DIN
    'Объединение всех флагов, обозначающих потоки данных на вывод
    L502_STREAM_ALL_OUT = L502_STREAM_DAC1 Or L502_STREAM_DAC2 Or L502_STREAM_DOUT
End Enum

'Константы, определяющие тип передаваемого отсчета из ПК в модуль
Public Enum t_stream_out_wrd_type
    L502_STREAM_OUT_WORD_TYPE_DOUT = &H0&   'Цифровой вывод
    L502_STREAM_OUT_WORD_TYPE_DAC1 = &H40000000    'Код для 1-го канала ЦАП
    L502_STREAM_OUT_WORD_TYPE_DAC2 = &H80000000    'Код для 2-го канала ЦАП
End Enum

'Режим работы модуля L502
Public Enum t_l502_mode
    L502_MODE_FPGA = 0  'Все потоки данных передаются через ПЛИС минуя
                                                    'сигнальный процессор BlackFin
    L502_MODE_DSP = 1    'Все потоки данных передаются через сигнальный
                                                    'процессор, который должен быть загружен
                                                    'прошивкой для обработки этих потоков
    L502_MODE_DEBUG = 2  'Отладочный режим
End Enum

'Номера каналов ЦАП.
Public Enum t_l502_dac_ch
   L502_DAC_CH1 = 0 'Первый канал ЦАП
   L502_DAC_CH2 = 1  'Второй канал ЦАП
End Enum

'Флаги, используемые при выводе данных на ЦАП.
 Public Enum t_l502_dacout_flags
    'Указывает, что значение задано в Вольтах и при выводе его нужно
    '    перевести его в коды ЦАП. Если флаг не указан, то считается, что значение
    '    изначально в кодах
    L502_DAC_FLAGS_VOLT = &H1&
    'Указывает, что нужно применить калибровочные коэффициенты перед
    '    выводом значения на ЦАП.
    L502_DAC_FLAGS_CALIBR = &H2&
End Enum

'Номера каналов DMA
Public Enum t_l502_dma_ch
    L502_DMA_CH_IN = 0  'Общий канал DMA на ввод
    L502_DMA_CH_OUT = 1 'Общий канал DMA на вывод
End Enum

'Цифровые линии, на которых можно включить подтягивающие резисторы
Public Enum t_l502_pullups
    L502_PULLUPS_DI_H = &H1&     'Старшая половина цифровых входов
    L502_PULLUPS_DI_L = &H2&     'Младшая половина цифровых входов
    L502_PULLUPS_DI_SYN1 = &H4&  'Линия SYN1
    L502_PULLUPS_DI_SYN2 = &H8&  'Линия SYN2
End Enum

'Флаги, определяющие наличие опций в модуле
Public Enum t_l502_dev_flags
    'Признак наличия двухканального канального ЦАП
    L502_DEVFLAGS_DAC_PRESENT = &H1&
    'Признак наличия гальваноразвязки
    L502_DEVFLAGS_GAL_PRESENT = &H2&
    'Признак наличия сигнального процессора BlackFin
    L502_DEVFLAGS_BF_PRESENT = &H4&
    'Признак, что во Flash-памяти присутствует информация о модуле
    L502_DEVFLAGS_FLASH_DATA_VALID = &H10000
    'Признак, что во Flash-памяти присутствуют действительные калибровочные
    '   коэффициенты АЦП
    L502_DEVFLAGS_FLASH_ADC_CALIBR_VALID = &H20000
    'Признак, что во Flash-памяти присутствуют действительные калибровочные
    '  коэффициенты ЦАП
    L502_DEVFLAGS_FLASH_DAC_CALIBR_VALID = &H40000
End Enum

'Флаги для режима циклического вывода
Public Enum t_l502_out_cycle_flags
    L502_OUT_CYCLE_FLAGS_FORCE = &H1&
End Enum




' Калибровочные коэффициенты диапазона
Public Type t_l502_cbr_coef
    offs As Double 'смещение нуля
    k As Double 'коэффициент шкалы
End Type


'Калибровочные коэффициенты модуля
Public Type t_l502_cbr
    adc(0 To L502_ADC_RANGE_CNT - 1) As t_l502_cbr_coef 'Калибровочные коэффициенты АЦП
    res1(0 To 64 - 1) As Long 'Резерв
    dac(0 To L502_DAC_CH_CNT - 1) As t_l502_cbr_coef 'Калибровочные коэффициенты ЦАП
    res2(0 To 20 - 1) As Long
End Type


Public Type t_l502_info
    DevName(0 To L502_DEVNAME_SIZE - 1) As Byte
    Serial(0 To L502_SERIAL_SIZE - 1) As Byte 'Серийный номер
    devflags As Long 'Флаги из #t_l502_dev_flags, описывающие наличие в модуле определенных опций
    fpga_ver As Integer 'Версия ПЛИС (старший байт - мажорная, младший - минорная)
    plda_ver As Byte 'Версия ПЛИС, управляющего аналоговой частью
    board_rev As Byte 'Ревизия платы
    res(0 To 120 - 1) As Byte 'Резерв
    cbr As t_l502_cbr      'Заводские калибровочные коэффициенты (из Flash-памяти)
End Type



Private Declare Function tmp_L502_GetSerialList Lib "l502api" Alias "L502_GetSerialList" (ByVal serials As Long, ByVal size As Long, ByVal flags As Long, ByRef devcnt As Long) As Long


Public Declare Function L502_Create Lib "l502api" () As Long
Public Declare Function L502_Free Lib "l502api" (ByVal hnd As Long) As Long
Public Declare Function L502_Open Lib "l502api" (ByVal hnd As Long, ByVal Serial As String) As Long
Public Declare Function L502_Close Lib "l502api" (ByVal hnd As Long) As Long
Public Declare Function L502_GetDevInfo Lib "l502api" (ByVal hnd As Long, ByRef info As t_l502_info) As Long


Public Declare Function L502_Configure Lib "l502api" (ByVal hnd As Long, ByVal flags As Long) As Long
Public Declare Function L502_SetLChannel Lib "l502api" (ByVal hnd As Long, ByVal lch As Long, ByVal phy_ch As Long, ByVal mode As Long, ByVal range As Long, ByVal avg As Long) As Long
'Установка количества логических каналов.
Public Declare Function L502_SetLChannelCount Lib "l502api" (ByVal hnd As Long, ByVal lch_cnt As Long) As Long
'Получение количества логических каналов.
Public Declare Function L502_GetLChannelCount Lib "l502api" (ByVal hnd As Long, ByRef lch_cnt As Long) As Long
'Установка делителя частоты сбора для АЦП
Public Declare Function L502_SetAdcFreqDivider Lib "l502api" (ByVal hnd As Long, ByVal adc_freq_div As Long) As Long
'Установка значения межкадровой задержки для АЦП.
Public Declare Function L502_SetAdcInterframeDelay Lib "l502api" (ByVal hnd As Long, ByVal delay As Long) As Long
'Установка делителя частоты синхронного ввода с цифровых линий.
Public Declare Function L502_SetDinFreqDivider Lib "l502api" (ByVal hnd As Long, ByVal din_freq_div As Long) As Long
'Установка частоты сбора АЦП.
Public Declare Function L502_SetAdcFreq Lib "l502api" (ByVal hnd As Long, ByRef f_acq As Double, ByRef f_frame As Double) As Long
'Установка частоты синхронного ввода с цифровых входов.
Public Declare Function L502_SetDinFreq Lib "l502api" (ByVal hnd As Long, ByRef f_din As Double) As Long
'Получить текущие значения частот сбора АЦП
Public Declare Function L502_GetAdcFreq Lib "l502api" (ByVal hnd As Long, ByRef f_acq As Double, ByRef f_frame As Double) As Long
'Установка значения опорной частоты синхронизации.
Public Declare Function L502_SetRefFreq Lib "l502api" (ByVal hnd As Long, ByVal freq As Long) As Long
'Установка режима генерации частоты синхронизации.
Public Declare Function L502_SetSyncMode Lib "l502api" (ByVal hnd As Long, ByVal sync_mode As Long) As Long
'Установка режима запуска частоты синхронизации
Public Declare Function L502_SetSyncStartMode Lib "l502api" (ByVal hnd As Long, ByVal sync_start_mode As Long) As Long
'Установить режим работы модуля.
Public Declare Function L502_SetMode Lib "l502api" (ByVal hnd As Long, ByVal mode As Long) As Long
'Получение текущего режима работы модуля.
Public Declare Function L502_GetMode Lib "l502api" (ByVal hnd As Long, ByRef mode As Long) As Long
'Установить коэффициенты для калибровки значений АЦП
Public Declare Function L502_SetAdcCoef Lib "l502api" (ByVal hnd As Long, ByVal range As Long, ByVal k As Double, ByVal offs As Double) As Long
'Получение текущих калибровочных коэффициентов АЦП.
Public Declare Function L502_GetAdcCoef Lib "l502api" (ByVal hnd As Long, ByVal range As Long, ByRef k As Double, ByRef offs As Double) As Long
'Установить коэффициенты для калибровки значений ЦАП.
Public Declare Function L502_SetDacCoef Lib "l502api" (ByVal hnd As Long, ByVal ch As Long, ByVal k As Double, ByVal offs As Double) As Long
'Получение текущих калибровочных коэффициентов ЦАП
Public Declare Function L502_GetDacCoef Lib "l502api" (ByVal hnd As Long, ByVal ch As Long, ByRef k As Double, ByRef offs As Double) As Long

'Асинхронный вывод данных на канал ЦАП.
Public Declare Function L502_AsyncOutDac Lib "l502api" (ByVal hnd As Long, ByVal ch As Long, ByVal data As Double, ByVal flags As Long) As Long
'Асинхронный вывод данных на цифровые выходы.
Public Declare Function L502_AsyncOutDig Lib "l502api" (ByVal hnd As Long, ByVal val As Long, ByVal msk As Long) As Long
'Асинхронный ввод значений с цифровых входов.
Public Declare Function L502_AsyncInDig Lib "l502api" (ByVal hnd As Long, ByRef din As Long) As Long

Private Declare Function tmp_L502_AsyncGetAdcFrame Lib "l502api" Alias "L502_AsyncGetAdcFrame" (ByVal hnd As Long, ByVal flags As Long, ByVal tout As Long, ByRef data As Double) As Long

'Разрешение синхронных потоков на ввод/вывод.
Public Declare Function L502_StreamsEnable Lib "l502api" (ByVal hnd As Long, ByVal streams As Long) As Long
'Запрещение синхронных потоков на ввод/вывод.
Public Declare Function L502_StreamsDisable Lib "l502api" (ByVal hnd As Long, ByVal streams As Long) As Long
'Запуск синхронных потоков ввода/вывода.
Public Declare Function L502_StreamsStart Lib "l502api" (ByVal hnd As Long) As Long
'Останов синхронных потоков ввода/вывода.
Public Declare Function L502_StreamsStop Lib "l502api" (ByVal hnd As Long) As Long
'Проверка, запущен ли синхронный ввод/вывод
Public Declare Function L502_IsRunning Lib "l502api" (ByVal hnd As Long) As Long
'Чтение данных АЦП и цивровых входов из модуля.
Private Declare Function tmp_L502_Recv Lib "l502api" Alias "L502_Recv" (ByVal hnd As Long, ByRef buf As Long, ByVal size As Long, ByVal tout As Long) As Long
'Передача потоковых данных ЦАП и цифровых выходов в модуль.
Private Declare Function tmp_L502_Send Lib "l502api" Alias "L502_Send" (ByVal hnd As Long, ByRef buf As Long, ByVal size As Long, ByVal tout As Long) As Long
'Обработка принятых отсчетов АЦП от модуля.
Private Declare Function tmp_L502_ProcessAdcData Lib "l502api" Alias "L502_ProcessAdcData" (ByVal hnd As Long, ByRef src As Long, ByRef dest As Double, ByRef size As Long, ByVal flags As Long) As Long
'Обработка принятых от модуля данных.
Private Declare Function tmp_L502_ProcessData Lib "l502api" Alias "L502_ProcessData" (ByVal hnd As Long, ByRef src As Long, ByVal flags As Long, ByRef adc_data As Double, ByRef adc_data_size As Long, ByRef din_data As Long, ByRef din_data_size As Long) As Long
'Обработка принятых от модуля данных с пользовательскими данными.
Private Declare Function tmp_L502_ProcessDataWithUserExt Lib "l502api" Alias "L502_ProcessDataWithUserExt" (ByVal hnd As Long, ByRef src As Long, ByVal size As Long, ByVal flags As Long, ByRef adc_data As Double, ByRef adc_data_size As Long, ByRef din_data As Long, ByRef din_data_size As Long, ByRef usr_data As Long, ByRef usr_data_size As Long) As Long
'Подготовка данных для вывода в модуль.
Private Declare Function tmp_L502_PrepareData Lib "l502api" Alias "L502_PrepareData" (ByVal hnd As Long, ByRef dac1 As Double, ByRef dac2 As Double, ByRef digout As Long, ByVal size As Long, ByVal flags As Long, ByRef out_buf As Long) As Long

'Получить количество отсчетов в буфере потока на ввод.
Public Declare Function L502_GetRecvReadyCountLib Lib "l502api" (ByVal hnd As Long, ByRef rdy_cnt As Long) As Long
'Получить размер свободного места в буфере потока на вывод.
Public Declare Function L502_GetSendReadyCount Lib "l502api" (ByVal hnd As Long, ByRef rdy_cnt As Long) As Long
'Получить номер следующего ожидаемого логического канала АЦП для  обработки.
Public Declare Function L502_GetNextExpectedLchNum Lib "l502api" (ByVal hnd As Long, ByRef lch As Long) As Long
'Начало подготовки вывода синхронных данных
Public Declare Function L502_PreloadStart Lib "l502api" (ByVal hnd As Long) As Long
'Начало загрузки циклического сигнала на вывод
Public Declare Function L502_OutCycleLoadStart Lib "l502api" (ByVal hnd As Long, ByVal size As Long) As Long
'Установка ранее загруженного циклического сигнала на вывод
Public Declare Function L502_OutCycleSetup Lib "l502api" (ByVal hnd As Long, ByVal flags As Long) As Long
'Останов вывода циклического сигнала
Public Declare Function L502_OutCycleStop Lib "l502api" (ByVal hnd As Long, ByVal flags As Long) As Long
'Установка размера буфера в драйвере для синхронного ввода или вывода.
Public Declare Function L502_SetDmaBufSize Lib "l502api" (ByVal hnd As Long, ByVal dma_ch As Long, ByVal size As Long) As Long
'Установка шага прерывания при передаче потока по DMA.
Public Declare Function L502_SetDmaIrqStep Lib "l502api" (ByVal hnd As Long, ByVal dma_ch As Long, ByVal step As Long) As Long

'Загрузка прошивки сигнального процессора BlackFin.
Public Declare Function L502_BfLoadFirmware Lib "l502api" (ByVal hnd As Long, ByVal filename As String) As Long
'Проверка, загружена ли прошивка BlackFIn.
Public Declare Function L502_BfCheckFirmwareIsLoaded Lib "l502api" (ByVal hnd As Long, ByRef version As Long) As Long
'Чтение блока данных из памяти сигнального процессора.
Private Declare Function tmp_L502_BfMemRead Lib "l502api" Alias "L502_BfMemRead" (ByVal hnd As Long, ByVal addr As Long, ByRef regs As Long, ByVal size As Long) As Long
'Запись блока данных в память сигнального процессора.
Private Declare Function tmp_L502_BfMemWrite Lib "l502api" Alias "L502_BfMemWrite" (ByVal hnd As Long, ByVal addr As Long, ByRef regs As Long, ByVal size As Long) As Long
'Передача управляющей команды сигнальному процессору.
Private Declare Function tmp_L502_BfExecCmd Lib "l502api" Alias "L502_BfExecCmd" (ByVal hnd As Long, ByVal cmd_code As Integer, ByVal par As Long, ByRef snd_data As Long, ByVal snd_size As Long, ByRef rcv_data As Long, ByVal rcv_size As Long, ByVal tout As Long, ByRef recvd_size As Long) As Long

'Чтение блока данных из Flash-памяти.
 Private Declare Function tmp_L502_FlashRead Lib "l502api" Alias "L502_FlashRead" (ByVal hnd As Long, ByVal addr As Long, ByRef data As Byte, ByVal size As Long) As Long
 'Запись блока данных во Flash-память модуля.
 Private Declare Function tmp_L502_FlashWrite Lib "l502api" Alias "L502_FlashWrite" (ByVal hnd As Long, ByVal addr As Long, ByRef data As Byte, ByVal size As Long) As Long
'Стирание блока во Flash-памяти.
 Public Declare Function L502_FlashErase Lib "l502api" (ByVal hnd As Long, ByVal addr As Long, ByVal size As Long) As Long
 'Разрешение записи в пользовательскую область Flash-памяти.
 Public Declare Function L502_FlashWriteEnable Lib "l502api" (ByVal hnd As Long) As Long
 'Запрет записи в пользовательскую область Flash-памяти.
 Public Declare Function L502_FlashWriteDisable Lib "l502api" (ByVal hnd As Long) As Long
 
 'Получить версию библиотеки.
Public Declare Function L502_GetDllVersion Lib "l502api" () As Long
'Получить версию драйвера устройства.
Public Declare Function L502_GetDriverVersion Lib "l502api" (ByVal hnd As Long, ByRef ver As Long) As Long
'Моргание светодиодом на передней панели.
Public Declare Function L502_LedBlink Lib "l502api" (ByVal hnd As Long) As Long
'Установка подтягивающих резисторов на входных линиях.
Public Declare Function L502_SetDigInPullup Lib "l502api" (ByVal hnd As Long, ByVal pullups As Long) As Long


Private Declare Function tmp_L502_GetErrorString Lib "l502api" Alias "L502_GetErrorString" (ByVal Error_Code As Long) As Long


Private Declare Function lstrlen Lib "kernel32.dll" Alias "lstrlenA" (ByVal lpString As Long) As Long
Private Declare Sub CopyMemory Lib "kernel32.dll" Alias "RtlMoveMemory" (Destination As Any, Source As Any, ByVal Length As Long)
Private Declare Function GlobalAlloc Lib "kernel32" (ByVal wFlags As Long, ByVal dwBytes As Long) As Long
Private Declare Function GlobalFree Lib "kernel32" (ByVal hMem As Long) As Long


Public Function L502_AsyncGetAdcFrame(ByVal hnd As Long, ByVal flags As Long, ByVal tout As Long, ByRef data() As Double) As Long
    L502_AsyncGetAdcFrame = tmp_L502_AsyncGetAdcFrame(hnd, flags, tout, data(0))
End Function

Public Function L502_Recv(ByVal hnd As Long, ByRef buf() As Long, ByVal size As Long, ByVal tout As Long) As Long
    L502_Recv = tmp_L502_Recv(hnd, buf(0), size, tout)
End Function

Public Function L502_Send(ByVal hnd As Long, ByRef buf() As Long, ByVal size As Long, ByVal tout As Long) As Long
    L502_Send = tmp_L502_Send(hnd, buf(0), size, tout)
End Function


Public Function L502_ProcessAdcData(ByVal hnd As Long, ByRef src() As Long, ByRef dest() As Double, ByRef size As Long, ByVal flags As Long) As Long
    L502_ProcessAdcData = tmp_L502_ProcessAdcData(hnd, src(0), dest(0), size, flags)
End Function

Public Function L502_ProcessData(ByVal hnd As Long, ByRef src() As Long, ByVal flags As Long, ByRef adc_data() As Double, ByRef adc_data_size As Long, ByRef din_data() As Long, ByRef din_data_size As Long) As Long
    L502_ProcessData = tmp_L502_ProcessData(hnd, src(0), flags, adc_data(0), adc_data_size, din_data(0), din_data_size)
End Function

Public Function L502_ProcessDataWithUserExt(ByVal hnd As Long, ByRef src() As Long, ByVal size As Long, ByVal flags As Long, ByRef adc_data() As Double, ByRef adc_data_size As Long, ByRef din_data() As Long, ByRef din_data_size As Long, ByRef usr_data() As Long, ByRef usr_data_size As Long) As Long
    L502_ProcessDataWithUserExt = tmp_L502_ProcessDataWithUserExt(hnd, src(0), size, flags, adc_data(0), adc_data_size, din_data(0), din_data_size, usr_data(0), usr_data_size)
End Function

Public Function L502_PrepareData(ByVal hnd As Long, ByRef dac1() As Double, ByRef dac2() As Double, ByRef digout() As Long, ByVal size As Long, ByVal flags As Long, ByRef out_buf() As Long) As Long
    L502_PrepareData = tmp_L502_PrepareData(hnd, dac1(0), dac2(0), digout(0), size, flags, out_buf(0))
End Function

Public Function L502_BfMemRead(ByVal hnd As Long, ByVal addr As Long, ByRef regs() As Long, ByVal size As Long) As Long
    L502_BfMemRead = tmp_L502_BfMemRead(hnd, addr, regs(0), size)
End Function

Public Function L502_BfMemWrite(ByVal hnd As Long, ByVal addr As Long, ByRef regs() As Long, ByVal size As Long) As Long
    L502_BfMemWrite = tmp_L502_BfMemWrite(hnd, addr, regs(0), size)
End Function

Public Function L502_BfExecCmd(ByVal hnd As Long, ByVal cmd_code As Integer, ByVal par As Long, ByRef snd_data() As Long, ByVal snd_size As Long, ByRef rcv_data() As Long, ByVal rcv_size As Long, ByVal tout As Long, ByRef recvd_size As Long) As Long
    L502_BfExecCmd = tmp_L502_BfExecCmd(hnd, cmd_code, par, snd_data(0), snd_size, rcv_data(0), rcv_size, tout, recvd_size)
End Function

Public Function L502_FlashRead(ByVal hnd As Long, ByVal addr As Long, ByRef data() As Byte, ByVal size As Long) As Long
    L502_FlashRead = tmp_L502_FlashRead(hnd, addr, data(0), size)
End Function
 
 Public Function L502_FlashWrite(ByVal hnd As Long, ByVal addr As Long, ByRef data() As Byte, ByVal size As Long) As Long
    L502_FlashWrite = tmp_L502_FlashWrite(hnd, addr, data(0), size)
 End Function

Public Function StrFromPtr(ByVal lpStr As Long) As String
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
    StrFromPtr = StrConv(bStr, vbUnicode)
End Function

Public Function L502_GetErrorString(ByVal Error_Code As Long) As String
    Dim Ptr As Long
    Ptr = tmp_L502_GetErrorString(Error_Code)
    L502_GetErrorString = StrFromPtr(Ptr)
End Function

Public Function L502_GetSerialList(ByRef serials() As String, ByVal flags As Long) As Long
    Dim tmp As Long
    Dim cnt As Long
    Dim ret_cnt As Long
    Dim dummy As Long
    Dim err As Long
    
    ret_cnt = 0
    tmp = 0
    
    ret_cnt = tmp_L502_GetSerialList(tmp, 0, flags, cnt)
      If ret_cnt < 0 Then
        err = ret_cnt
        ReDim serials(0)
    Else
        If cnt = 0 Then
            ReDim serials(0)
       Else
            tmp = GlobalAlloc(0, L502_SERIAL_SIZE * cnt)
            If tmp = 0 Then
                err = L502_ERR_MEMORY_ALLOC
            Else
                ret_cnt = tmp_L502_GetSerialList(tmp, cnt, flags, dummy)
                If ret_cnt < 0 Then
                   err = ret_cnt
                   ReDim serials(0)
                Else
                    If ret_cnt = 0 Then
                        ReDim serials(0)
                    Else
                        ReDim serials(0 To ret_cnt - 1)
                         For i = 0 To ret_cnt - 1
                            serials(i) = StrFromPtr(tmp)
                         Next
                    End If
                End If
                GlobalFree (tmp)
             End If
       End If
    End If
    
    If err = L502_ERR_OK Then
        err = ret_cnt
    End If
    L502_GetSerialList = err
End Function

