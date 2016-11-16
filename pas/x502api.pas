unit x502api;
interface
uses Windows, SysUtils;

const
  // Максимальное количество логических каналов в таблице
  X502_LTABLE_MAX_CH_CNT      = 256;
  // Количество диапазонов для измерения напряжений
  X502_ADC_RANGE_CNT          = 6;

  // Количество каналов АЦП в режиме с общей землей
  X502_ADC_COMM_CH_CNT        = 32;
  // Количество каналов АЦП в дифференциальном режиме
  X502_ADC_DIFF_CH_CNT        = 16;

  // Максимальное значение для аппаратного усреднения по логическому каналу
  X502_LCH_AVG_SIZE_MAX       = 128;
  //Максимальное значения делителя частоты АЦП
  X502_ADC_FREQ_DIV_MAX       = (1024*1024);
  // Максимальное значение делителя частоты синхронного цифрового ввода
  X502_DIN_FREQ_DIV_MAX       = (1024*1024);

  // Минимальное значение делителя частоты синхронного вывода
  X502_OUT_FREQ_DIV_MIN       = 2;
  // Максимальное значение делителя частоты синхронного вывода
  X502_OUT_FREQ_DIV_MAX       = 1024;
  // Значение делителя частоты вывода по умолчанию (которое также всегда
  // используется в L502 с версией прошивки ПЛИС ниже 0.5) 
  X502_OUT_FREQ_DIV_DEFAULT   = 2;

  // Максимальное значение межкадровой задержки для АЦП
  X502_ADC_INTERFRAME_DELAY_MAX = $1FFFFF;
  // Таймаут по умолчанию для выполнения команды к BlackFin
  X502_BF_CMD_DEFAULT_TOUT      = 500;

  // Код АЦП, соответствующий максимальному значению шкалы
  X502_ADC_SCALE_CODE_MAX       = 6000000;
  // Код ЦАП, соответствующий максимальному значению шкалы
  X502_DAC_SCALE_CODE_MAX       = 30000;

  // Максимальное количество символов в строке с названием устройства
  X502_DEVNAME_SIZE             = 32;
  // Максимальное количество символов в строке с серийным номером
  X502_SERIAL_SIZE              = 32;

  // Максимальное количество символов в строке с описанием подключения
  X502_LOCATION_STR_SIZE        = 64;
  // Размер MAC-адреса для Ethernet интерфейса
  X502_MAC_ADDR_SIZE            = 6;
  // Размер строки с описанием экземпляра устройства
  X502_INSTANCE_NAME_SIZE       = 64;
  // Максимальный размер строки с паролем на настройки
  X502_PASSWORD_SIZE            = 32;

  // Максимально возможное значение внешней опорной частоты
  X502_EXT_REF_FREQ_MAX         = 1500000;


  //Размер пользовательской области Flash-памяти
  X502_FLASH_USER_SIZE          = $100000;
  //Стандартный таймаут на выполнение запроса к BlackFin в мс
  X502_BF_REQ_TOUT              = 500;
  // Диапазон ЦАП в вольтах
  X502_DAC_RANGE                = 5;
  // Количество каналов ЦАП
  X502_DAC_CH_CNT               = 2;
  // Количество цифровых выходов у модуля
  X502_DOUT_LINES_CNT           = 16;

  // слово в потоке, означающее, что произошло переполнение
  X502_STREAM_IN_MSG_OVERFLOW   = $01010000;

  X502_DEVREC_SIGN              = $4C524543;

  { ----------------- Коды ошибок библиотеки ---------------------------}
    // Функция выполнена без ошибок
    X502_ERR_OK                           = 0;
    // В функцию передан недействительный описатель модуля
    X502_ERR_INVALID_HANDLE               = -1;
    // Ошибка выделения памяти
    X502_ERR_MEMORY_ALLOC                 = -2;
    // Попытка открыть уже открытое устройство
    X502_ERR_ALREADY_OPENED               = -3;
    // Устройство с заданными параметрами не найдено в системе
    X502_ERR_DEVICE_NOT_FOUND             = -4;
    // Доступ к устройству запрещен (Как правило из-за того, что устройство уже открыто в другой программе)
    X502_ERR_DEVICE_ACCESS_DENIED         = -5;
    // Ошибка открытия устройства
    X502_ERR_DEVICE_OPEN                  = -6;
    // В функцию передан недействительный указатель
    X502_ERR_INVALID_POINTER              = -7;
    // Функция не может быть выполнена при запущенном потоке сбора данных
    X502_ERR_STREAM_IS_RUNNING            = -8;
    // Ошибка чтения данных синхронного ввода
    X502_ERR_RECV                         = -9;
    // Ошибка записи данных для синхронного вывода
    X502_ERR_SEND                         = -10;
    // Произошло переполнение внутреннего буфера для потока синхронного ввода
    X502_ERR_STREAM_OVERFLOW              = -11;
    // Неизвестное сообщение в потоке синхронного ввода
    X502_ERR_UNSUP_STREAM_MSG             = -12;
    // Ошибка создания системного мьютекса
    X502_ERR_MUTEX_CREATE                 = -13;
    // Неверный описатель мьютекса
    X502_ERR_MUTEX_INVALID_HANDLE         = -14;
    // Истекло время ожидания освобождения мьютекса
    X502_ERR_MUTEX_LOCK_TOUT              = -15;
    // Ошибка освобождения мьютекса
    X502_ERR_MUTEX_RELEASE                = -16;
    // Недостаточно системных ресурсов
    X502_ERR_INSUFFICIENT_SYSTEM_RESOURCES= -17;
    // Данная возможность еще не реализована
    X502_ERR_NOT_IMPLEMENTED              = -18;
    // Недостаточный размер массива
    X502_ERR_INSUFFICIENT_ARRAY_SIZE      = -19;
    // Ошибка чтения регистра FPGA
    X502_ERR_FPGA_REG_READ                = -20;
    // Ошибка записи регистра FPGA
    X502_ERR_FPGA_REG_WRITE               = -21;
    // Сбор данных уже остановлен
    X502_ERR_STREAM_IS_NOT_RUNNING        = -22;
    // Ошибка освобождения интерфейса
    X502_ERR_INTERFACE_RELEASE            = -23;
    // Ошибка запуска потока
    X502_ERR_THREAD_START                 = -24;
    // Ошибка останова потока
    X502_ERR_THREAD_STOP                  = -25;
    // Устройство было отключено
    X502_ERR_DEVICE_DISCONNECTED          = -26;
    // Неверный размер ответа на управляющий запрос
    X502_ERR_IOCTL_INVALID_RESP_SIZE      = -27;
    // Неверный тип устройства
    X502_ERR_INVALID_DEVICE               = -28;
    // Недействительная запись о устройстве
    X502_ERR_INVALID_DEVICE_RECORD        = -29;
    // Неверный хендл конфигурации модуля
    X502_ERR_INVALID_CONFIG_HANDLE        = -30;
    // Связь с устройством закрыта или не была установлена
    X502_ERR_DEVICE_NOT_OPENED            = -31;
    // Данная операция не доступна для текущего интерфейса связи с устройством
    X502_ERR_INVALID_OP_FOR_IFACE         = -32;
    // Не загружен ПЛИС модуля
    X502_ERR_FPGA_NOT_LOADED              = -33;
    // Неверная конфигурация USB-устройства
    X502_ERR_INVALID_USB_CONFIGURATION    = -34;
    // Задан неверный размер логической таблицы
    X502_ERR_INVALID_LTABLE_SIZE          = -102;
    // Задан неверный номер логического канала
    X502_ERR_INVALID_LCH_NUMBER           = -103;
    // Неверно задано значение диапазона АЦП
    X502_ERR_INVALID_LCH_RANGE            = -104;
    // Неверно задан режим измерения для логического канала
    X502_ERR_INVALID_LCH_MODE             = -105;
    // Неверно задан номер физического канала при настройке логического
    X502_ERR_INVALID_LCH_PHY_NUMBER       = -106;
    // Неверно задан размер усреднения для логического канала
    X502_ERR_INVALID_LCH_AVG_SIZE         = -107;
    // Неверно задан делитель частоты сбора данных АЦП
    X502_ERR_INVALID_ADC_FREQ_DIV         = -108;
    // Неверно задан делитель частоты синхронного ввода цифровых линий
    X502_ERR_INVALID_DIN_FREQ_DIV         = -109;
    // Неверно задан режим работы модуля L502
    X502_ERR_INVALID_MODE                 = -110;
    // Неверный номер канала ЦАП
    X502_ERR_INVALID_DAC_CHANNEL          = -111;
    // Неверный код выбора опорной частоты синхронизации
    X502_ERR_INVALID_REF_FREQ             = -112;
    // Неверно задано значение межкадровой задержки
    X502_ERR_INVALID_INTERFRAME_DELAY     = -113;
    // Неверно задан режим синхронизации
    X502_ERR_INVALID_SYNC_MODE            = -114;
    // Неверно задан номер потока данных
    X502_ERR_INVALID_STREAM_CH            = -115;
    // Неверно задан делитель частоты синхронного вывода
    X502_ERR_INVALID_OUT_FREQ_DIV         = -116;
    // Ошибка захвата опорной частоты синхронизации
    X502_ERR_REF_FREQ_NOT_LOCKED          = -131;
    // Управляющий запрос к драйверу завершен с ошибкой
    X502_ERR_IOCTL_FAILD                  = -132;
    // Истек таймаут ожидания завершения выполнения управляющего запроса к драйверу
    X502_ERR_IOCTL_TIMEOUT                = -133;
    // Ошибка получения информации о устройстве от драйвера
    X502_ERR_GET_INFO                     = -134;
    // За время ожидания не было считано новое слово с цифровых линий
    X502_ERR_DIG_IN_NOT_RDY               = -135;
    // Принято недостаточно слов от модуля
    X502_ERR_RECV_INSUFFICIENT_WORDS      = -136;
    // Попытка выполнить операцию, требующую наличие ЦАП, при его отсутствии
    X502_ERR_DAC_NOT_PRESENT              = -137;
    // Неверный номер канала в обрабатываемом потоке синхронного ввода
    X502_ERR_PROC_INVALID_CH_NUM          = -140;
    // Неверный код диапазона в обрабатываемом потоке синхронного ввода
    X502_ERR_PROC_INVALID_CH_RANGE        = -141;
    // Задан неверный адрес во Flash-памяти
    X502_ERR_FLASH_INVALID_ADDR           = -142;
    // Задан неверный размер блока данных при работе с Flash-памятью
    X502_ERR_FLASH_INVALID_SIZE           = -143;
    // Истек таймаут ожидания завершения записи во Flash-память
    X502_ERR_FLASH_WRITE_TOUT             = -144;
    // Истек таймаут ожидания завершения стирания блока Flash-памяти
    X502_ERR_FLASH_ERASE_TOUT             = -145;
    // Заданная область для стирания Flash-памяти нарушает границу блока в 4 Кбайт
    X502_ERR_FLASH_SECTOR_BOUNDARY        = -146;
    // Не удалось открыть сокет для соединения
    X502_ERR_SOCKET_OPEN                  = -147;
    // Превышено время подключения
    X502_ERR_CONNECTION_TOUT              = -148;
    // Соединение закрыто другой устройством
    X502_ERR_CONNECTION_CLOSED_BY_DEV     = -149;
    // Не удалось установить заданный размер буфера сокета
    X502_ERR_SOCKET_SET_BUF_SIZE          = -150;
    // Соединение для передачи данных не установлено
    X502_ERR_NO_DATA_CONNECTION           = -151;
    // Не удалось дождаться сообщения о завершении потока
    X502_ERR_NO_STREAM_END_MSG            = -152;
    // Соединение было сброшено другой стороной
    X502_ERR_CONNECTION_RESET             = -153;
    // Не удалось найти хост с указанным адресом
    X502_ERR_HOST_UNREACHABLE             = -154;
    // Ошибка установления TCP-соединения
    X502_ERR_TCP_CONNECTION_ERROR         = -155;
    // Не удалось открыть файл прошивки BlackFin
    X502_ERR_LDR_FILE_OPEN                = -180;
    // Ошибка чтения из фала прошивки BlackFin
    X502_ERR_LDR_FILE_READ                = -181;
    // Неверный формат файла прошивки BlackFin
    X502_ERR_LDR_FILE_FORMAT              = -182;
    // Используются возможность LDR-файла, недоступные при записи прошивки BlackFin по HDMA
    X502_ERR_LDR_FILE_UNSUP_FEATURE       = -183;
    // Неверный стартовый адрес программы в прошивке BlackFin
    X502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR  = -184;
    // Истек таймаут выполнения запроса на чтения/запись памяти BlackFin
    X502_ERR_BF_REQ_TIMEOUT               = -185;
    // Команда для BlackFin все еще находится в процессе обработки
    X502_ERR_BF_CMD_IN_PROGRESS           = -186;
    // Истекло время выполнения управляющей команды процессором BlackFin
    X502_ERR_BF_CMD_TIMEOUT               = -187;
    // Возвращено недостаточно данных в ответ на команду к BlackFin
    X502_ERR_BF_CMD_RETURN_INSUF_DATA     = -188;
    // Истек таймаут ожидания готовности процессора BlackFin к записи прошивки
    X502_ERR_BF_LOAD_RDY_TOUT             = -189;
    // Попытка выполнить операцию для которой нужен сигнальный процессор при
    // отсутствии сигнального процессора в модуле
    X502_ERR_BF_NOT_PRESENT               = -190;
    // Неверный адрес памяти BlackFin при записи или чтении по HDMA
    X502_ERR_BF_INVALID_ADDR              = -191;
    // Неверный размер данных, передаваемых с управляющей командой в BlackFin
    X502_ERR_BF_INVALID_CMD_DATA_SIZE     = -192;

    {--------------- Интерфейс соединения с модулем ------------------}
    X502_IFACE_UNKNOWN = 0; // Неизвестный интерфейс
    X502_IFACE_USB     = 1; // Устройство подключено по USB
    X502_IFACE_ETH     = 2; // Устройство подключено по Ethernet через TCP/IP
    X502_IFACE_PCI     = 3; // Устройство подключено по PCI/PCIe

    {-------- Флаги, управляющие поиском присутствующих модулей ------}
     // Признак, что нужно вернуть серийные номера только тех устройств,
     //   которые еще не открыты
    X502_GETDEVS_FLAGS_ONLY_NOT_OPENED = 1;



    {-------- Флаги для управления цифровыми выходами. ---------------}
    //Запрещение (перевод в третье состояние) старшей половины цифровых выходов
    X502_DIGOUT_WORD_DIS_H = $00020000;
    //Запрещение младшей половины цифровых выходов
    X502_DIGOUT_WORD_DIS_L = $00010000;


    {-------- Константы для выбора опорной частоты -------------------}
    X502_REF_FREQ_2000KHZ  = 2000000; // частота 2МГц
    X502_REF_FREQ_1500KHZ  = 1500000; // частота 1.5МГц

    {-------- Диапазоны измерения для канала АЦП ----------------------}
    X502_ADC_RANGE_10 = 0; // диапазон +/-10V
    X502_ADC_RANGE_5  = 1; // диапазон +/-5V
    X502_ADC_RANGE_2  = 2; // диапазон +/-2V
    X502_ADC_RANGE_1  = 3; // диапазон +/-1V
    X502_ADC_RANGE_05 = 4; // диапазон +/-0.5V
    X502_ADC_RANGE_02 = 5; // диапазон +/-0.2V


    {-------- Режим измерения для логического канала ------------------}
    X502_LCH_MODE_COMM = 0; // Измерение напряжения относительно общей земли
    X502_LCH_MODE_DIFF = 1; // Дифференциальное измерение напряжения
    X502_LCH_MODE_ZERO = 2; // Измерение собственного нуля


    {-------- Режимы синхронизации ------------------------------------}
    X502_SYNC_INTERNAL        = 0; // внутренний сигнал
    X502_SYNC_EXTERNAL_MASTER = 1; // от внешнего мастера по разъему синхронизации
    X502_SYNC_DI_SYN1_RISE    = 2; // по фронту сигнала DI_SYN1
    X502_SYNC_DI_SYN2_RISE    = 3; // по фронту сигнала DI_SYN2
    X502_SYNC_DI_SYN1_FALL    = 6; // по спаду сигнала DI_SYN1
    X502_SYNC_DI_SYN2_FALL    = 7;  // по спаду сигнала DI_SYN2

    {-------- Флаги, управляющие обработкой принятых данных -------------------}
    X502_PROC_FLAGS_VOLT            = $00000001; // Признак, что нужно преобразовать значения
                                                 // АЦП в вольты
    X502_PROC_FLAGS_DONT_CHECK_CH   = $00010000; // Признак, что не нужно проверять совпадение номеров каналов

    {-------- Флаги для обозначения синхронных потоков данных -----------------}
    X502_STREAM_ADC  = $01; //Поток данных от АЦП
    X502_STREAM_DIN  = $02; // Поток данных с цифровых входов
    X502_STREAM_DAC1 = $10; // Поток данных первого канала ЦАП
    X502_STREAM_DAC2 = $20; // Поток данных второго канала ЦАП
    X502_STREAM_DOUT = $40; // Поток данных на цифровые выводы
    // объединение всех флагов, обозначающих потоки данных на ввод
    X502_STREAM_ALL_IN = X502_STREAM_ADC or X502_STREAM_DIN;
    // объединение всех флагов, обозначающих потоки данных на вывод
    X502_STREAM_ALL_OUT = X502_STREAM_DAC1 or X502_STREAM_DAC2 or X502_STREAM_DOUT;


    {--- Константы, определяющие тип передаваемого отсчета из ПК в модуль -----}
    X502_STREAM_OUT_WORD_TYPE_DOUT   = $0; // Цифровой вывод
    X502_STREAM_OUT_WORD_TYPE_DAC1   = $40000000; // Код для 1-го канала ЦАП
    X502_STREAM_OUT_WORD_TYPE_DAC2   = $80000000; // Код для 2-го канала ЦАП

    {------------------ Режим работы модуля L502 ------------------------------}
    X502_MODE_FPGA  = 0; // Все потоки данных передаются через ПЛИС минуя
                         //   сигнальный процессор BlackFin
    X502_MODE_DSP   = 1; // Все потоки данных передаются через сигнальный
                         //   процессор, который должен быть загружен
                         //   прошивкой для обработки этих потоков
    X502_MODE_DEBUG = 2; // Отладочный режим

    {------------------- Номера каналов ЦАП -----------------------------------}
    X502_DAC_CH1 = 0;  //Первый канал ЦАП
    X502_DAC_CH2 = 1;  //Второй канал ЦАП

    {----------- Флаги, используемые при выводе данных на ЦАП -----------------}
    // Указывает, что значение задано в Вольтах и при выводе его нужно
    // перевести в коды ЦАП. Если флаг не указан, то считается, что значение
    // изначально в кодах
    X502_DAC_FLAGS_VOLT    = $0001;
    // Указывает, что нужно применить калибровочные коэффициенты перед
    //    выводом значения на ЦАП.
    X502_DAC_FLAGS_CALIBR  = $0002;

    {---------- Номера каналов для передачи потоков данных --------------------}
    X502_STREAM_CH_IN  = 0; // Общий канал на ввод
    X502_STREAM_CH_OUT = 1; // Общий канал на вывод

    {--- Цифровые линии, на которых можно включить подтягивающие резисторы ----}
    X502_PULLUPS_DI_H      = $01; // Старшая половина цифровых входов (только для L502)
    X502_PULLUPS_DI_L      = $02; // Младшая половина цифровых входов (только для L502)
    X502_PULLUPS_DI_SYN1   = $04; // Линия SYN1
    X502_PULLUPS_DI_SYN2   = $08; // Линия SYN2
    X502_PULLDOWN_CONV_IN  = $10; // Подтяжка к 0 линии межмодульной
                                  //   синхронизации CONV_IN (только для E502)
    X502_PULLDOWN_START_IN = $20; // Подтяжка к 0 линии межмодульной
                                  //   синхронизации START_IN (только для E502)



    {--------------- Флаги, определяющие наличие опций в модуле ---------------}
    // Признак наличия двухканального канального ЦАП
    X502_DEVFLAGS_DAC_PRESENT           = $00000001;
    // Признак наличия гальваноразвязки
    X502_DEVFLAGS_GAL_PRESENT           = $00000002;
    // Признак наличия сигнального процессора BlackFin
    X502_DEVFLAGS_BF_PRESENT            = $00000004;

    // Признак, что устройство поддерживает интерфейс USB
    X502_DEVFLAGS_IFACE_SUPPORT_USB     = $00000100;
    // Признак, что устройство поддерживает Ethernet
    X502_DEVFLAGS_IFACE_SUPPORT_ETH     = $00000200;
    // Признак, что устройство поддерживает интерфейс PCI/PCI-Express
    X502_DEVFLAGS_IFACE_SUPPORT_PCI     = $00000400;

    // Признак, что устройство выполнено в индустриалном исполнении
    X502_DEVFLAGS_INDUSTRIAL            = $00008000;

    // Признак, что во Flash-памяти присутствует информация о модуле
    X502_DEVFLAGS_FLASH_DATA_VALID      = $00010000;
    // Признак, что во Flash-памяти присутствуют действительные калибровочные коэффициенты АЦП
    X502_DEVFLAGS_FLASH_ADC_CALIBR_VALID = $00020000;
    // Признак, что во Flash-памяти присутствуют действительные калибровочные коэффициенты ЦАП
    X502_DEVFLAGS_FLASH_DAC_CALIBR_VALID = $00040000;

    // Признак, что присутствует прошивка ПЛИС и она успешно была загружена
    X502_DEVFLAGS_FPGA_LOADED            = $00800000;
    // Признак, что устройство уже открыто (действителен только внутри t_x502_devrec)
    X502_DEVFLAGS_DEVREC_OPENED          = $01000000;


    { Тип содержимого строки с расположением устройства }
    // В поле расположения устройства не содержится информации
    X502_LOCATION_TYPE_NONE             = 0;
    // В поле расположения устройства содержится строка с адресом устройства
    X502_LOCATION_TYPE_ADDR             = 1;
    // В поле ресположения устройства содержится строка с именем экземпляра
    X502_LOCATION_TYPE_INSTANCE_NAME    = 2;

    {----------------Флаги для режима циклического вывода----------------------}
    // Флаг указывает, что останов или смена сигнала могут произойти без ожидания
    //    конца цикла предыдущего сигнала.
    X502_OUT_CYCLE_FLAGS_FORCE = $01;

    // Флаг указывает, что функция должна дождаться полной загрузки сигнала
    // и установки сигнала на вывод (для X502_OutCycleSetup()) или завершения
    // генерации циклического сигнала (для X502_OutCycleStop())
    X502_OUT_CYCLE_FLAGS_WAIT_DONE = $02;

	
	{ ----- Коды возможностей модуля, которые могут поддерживаться или нет 
	        в зависимости от типа модуля, версий прошивок и т.п. -------------}
    // Поддержка установки делителя частоты вывода, отличного от #X502_OUT_FREQ_DIV_DEFAULT
    X502_FEATURE_OUT_FREQ_DIV       = 1;
    // Возможность чтения флагов состояния вывода с помощью X502_OutGetStatusFlags() 
    X502_FEATURE_OUT_STATUS_FLAGS   = 2;

	{ --------------- Флаги состояния для синхронного вывода ------------------}
    // Флаг указывает, что в настоящее время буфер в модуле на передачу пуст 
    X502_OUT_STATUS_FLAG_BUF_IS_EMPTY = $01;
    // Флаг указывает, что было опустошение буфера на вывод с начала старта синхронного
    //    ввода-вывода или с момента последнего чтения статуса с помощью
    //    X502_OutGetStatusFlags() (в зависимости от того, что было последним) 
    X502_OUT_STATUS_FLAG_BUF_WAS_EMPTY = $02;

type
  st_x502_devrec_inptr = record
  end;
  t_x502_devrec_inptr_hnd = ^st_x502_devrec_inptr;

  // Структура, описывающая устройство, по которой с ним можно установить соединение
  t_x502_devrec = record
    sign    : LongWord;  { Признак действительной структуры.
                       Если запись действительна (соответствует какому-либо устройству),
                       то должен быть равен #X502_DEVREC_SIGN) }
    devname : array [0..X502_DEVNAME_SIZE-1] of AnsiChar; // Название устройства
    serial  : array [0..X502_SERIAL_SIZE-1] of AnsiChar; // Серийный номер
    location: array [0..X502_LOCATION_STR_SIZE-1] of AnsiChar; // Описание подключения (если есть)
    flags   : LongWord; // Флаги из #t_x502_dev_flags, описывающие устройство
    iface   : Byte;    // Интерфейс, по которому подключено устройство
    location_type : Byte; // Определяет, что именно сохранено в поле location
    res     : array [0..122-1] of Byte;  // Резерв
    internal : t_x502_devrec_inptr_hnd; { Непрозрачный указатель на структуру с
                                          информацией об устройстве для возможности
                                          его открытия. }
  end;


  // описатель модуля - указатель на непрозрачную структуру
  t_x502_hnd = ^st_x502;
  st_x502 = record
  end;

  { калибровочные коэффициенты диапазона }
  t_x502_cbr_coef = record
    offs: Double;  // смещение нуля
    k : Double; // коэффициент шкалы
  end;


  { Калибровочные коэффициенты модуля. }
  t_x502_cbr = record
    //Калибровочные коэффициенты АЦП
    adc: array[0..X502_ADC_RANGE_CNT-1] of t_x502_cbr_coef;
    res1: array[0..63] of LongWord;
    // Калибровочные коэффициенты ЦАП
    dac: array[0..X502_DAC_CH_CNT-1] of t_x502_cbr_coef;
    res2: array[0..19] of LongWord;
  end;

  { Информация о модуле}
  t_x502_info = record
    name: array[0..X502_DEVNAME_SIZE-1] of AnsiChar; // Название устройства ("L502")
    serial: array[0..X502_SERIAL_SIZE-1] of AnsiChar; // Серийный номер
    devflags: LongWord; // Флаги описывающие наличие в модуле определенных опций
    fpga_ver : Word; // Версия ПЛИС (старший байт - мажорная, младший - минорная)
    plda_ver : Byte; // Версия ПЛИС, управляющего аналоговой частью
    board_rev: Byte; // Ревизия платы
    mcu_firmware_ver : LongWord; // Версия прошивки контроллера Cortex-M4. Действительна только для E502
    factory_mac : array[0..X502_MAC_ADDR_SIZE-1] of byte; { Заводской MAC-адрес --- действителен только для
                                                                устройств с Ethernet-интерфейсом }
    res : array[0..110-1] of Byte; //резерв
    cbr : t_x502_cbr; // Заводские калибровочные коэффициенты (из Flash-памяти)
  end;


  type t_x502_serial_array = array[0..0] of array[0..X502_SERIAL_SIZE-1] of AnsiChar;
  type p_x502_serial_array = ^t_x502_serial_array;
  type t_x502_devrec_array = array[0..0] of t_x502_devrec;
  type p_x502_devrec_array = ^t_x502_devrec_array;


  function X502_Create(): t_x502_hnd; stdcall;
  function X502_Free(hnd: t_x502_hnd): LongInt; stdcall;

  function X502_OpenByDevRecord(hnd: t_x502_hnd; var devrec : t_x502_devrec) : LongInt; stdcall;
  function X502_FreeDevRecordList(list : array of t_x502_devrec; size : LongWord) : LongInt; stdcall; overload;
  function X502_FreeDevRecordList(var rec : t_x502_devrec) : LongInt; stdcall; overload;

  //разорвать связь с модулем
  function X502_Close(hnd: t_x502_hnd): LongInt; stdcall;
  //получение информации о модуле
  function X502_GetDevInfo(hnd: t_x502_hnd; out info: t_x502_info) : LongInt; stdcall;

  // Передача установленных настроек в модуль
  function X502_Configure(hnd: t_x502_hnd; flags: LongWord): LongInt; stdcall;
  // Установка параметров логического канала
  function X502_SetLChannel(hnd: t_x502_hnd; lch, phy_ch, mode, range, avg: LongWord): LongInt; stdcall;
  // Установка количества логических каналов
  function X502_SetLChannelCount(hnd: t_x502_hnd; lch_cnt : LongWord): LongInt; stdcall;
  // Получение количества логических каналов
  function X502_GetLChannelCount(hnd: t_x502_hnd; out lch_cnt: LongWord): LongInt; stdcall;
  // Установка делителя частоты сбора для АЦП
  function X502_SetAdcFreqDivider(hnd: t_x502_hnd; adc_freq_div : LongWord): LongInt; stdcall;
  // Установка значения межкадровой задержки для АЦП
  function X502_SetAdcInterframeDelay(hnd: t_x502_hnd; delay : LongWord): LongInt; stdcall;
  // Установка делителя частоты синхронного ввода с цифровых линий
  function X502_SetDinFreqDivider(hnd: t_x502_hnd; din_freq_div: LongWord): LongInt; stdcall;
  // Установка делителя частоты синхронного вывода
  function X502_SetOutFreqDivider(hnd: t_x502_hnd; out_freq_div : LongWord): LongInt; stdcall;
  // Установка частоты сбора АЦП
  function X502_SetAdcFreq(hnd: t_x502_hnd; var f_acq, f_frame: Double): LongInt; stdcall;
  // Установка частоты синхронного ввода с цифровых входов
  function X502_SetDinFreq(hnd: t_x502_hnd; var f_din: Double): LongInt; stdcall;
  // Установка частоты синхронного вывода.
  function X502_SetOutFreq(hnd: t_x502_hnd; var f_dout: Double): LongInt; stdcall;
  // Получить текущие значения частот сбора АЦП
  function X502_GetAdcFreq(hnd: t_x502_hnd; out f_acq, f_frame: Double): LongInt; stdcall;
  // Установка значения опорной частоты синхронизации
  function X502_SetRefFreq(hnd: t_x502_hnd; freq: LongWord): LongInt; stdcall;
  // Установка значения внешней опорной частоты синхронизации
  function X502_SetExtRefFreqValue(hnd: t_x502_hnd; freq : Double): LongInt; stdcall;
  //Получение значения опорной частоты синхронизации
  function X502_GetRefFreqValue(hnd: t_x502_hnd; out freq : Double): LongInt; stdcall;

  // Установка режима генерации частоты синхронизации.
  function X502_SetSyncMode(hnd: t_x502_hnd; sync_mode: LongWord): LongInt; stdcall;
  // Установка режима запуска частоты синхронизации.
  function X502_SetSyncStartMode(hnd: t_x502_hnd; sync_start_mode: LongWord): LongInt; stdcall;
  // Установить режим работы модуля
  function X502_SetMode(hnd: t_x502_hnd; mode: LongWord): LongInt; stdcall;
  // Получение текущего режима работы модуля.
  function X502_GetMode(hnd: t_x502_hnd; out mode: LongWord): LongInt; stdcall;
  // Установить коэффициенты для калибровки значений АЦП.
  function X502_SetAdcCoef(hnd: t_x502_hnd; range: LongWord; k, offs: Double): LongInt; stdcall;
  // Получение текущих калибровочных коэффициентов АЦП.
  function X502_GetAdcCoef(hnd: t_x502_hnd; range: LongWord; out k, offs: Double): LongInt; stdcall;
  // Установить коэффициенты для калибровки значений ЦАП.
  function X502_SetDacCoef(hnd: t_x502_hnd; ch : LongWord; k, double: Double): LongInt; stdcall;
  // Получение текущих калибровочных коэффициентов ЦАП.
  function X502_GetDacCoef(hnd: t_x502_hnd; ch : LongWord; out k, double: Double): LongInt; stdcall;

  // Рассчет частоты сбора АЦП
  function X502_CalcAdcFreq(ref_freq: Double; lch_cnt: LongWord; var f_acq, f_frame: Double; out result_freq_div : LongWord; out  result_frame_delay: LongWord): LongInt; stdcall;
  // Рассчет частоты синхронного ввода с цифровых входов.
  function X502_CalcDinFreq(ref_freq: Double; var f_din : Double; out result_freq_div : LongWord): LongInt; stdcall;
  // Рассчет частоты синхронного вывода
  function X502_CalcOutFreq(ref_freq: Double; var f_dout : Double; out result_freq_div : LongWord): LongInt; stdcall;
  {----------------------- Функции асинхронного ввода-вывода ------------------}
  // Асинхронный вывод данных на канал ЦАП.
  function X502_AsyncOutDac(hnd: t_x502_hnd; ch: LongWord; data: Double; flags: LongWord): LongInt; stdcall;
  // Асинхронный вывод данных на цифровые выходы.
  function X502_AsyncOutDig(hnd: t_x502_hnd; val, msk: LongWord): LongInt; stdcall;
  // Асинхронный ввод значений с цифровых входов.
  function X502_AsyncInDig(hnd: t_x502_hnd; out din: LongWord): LongInt; stdcall;
  // Асинхронный ввод одного кадра АЦП.
  function X502_AsyncGetAdcFrame(hnd: t_x502_hnd; flags: LongWord; tout: LongWord; out data: array of Double): LongInt; stdcall;


  {-------------- Функции для работы с синхронным потоковым вводом-выводом ----}
  // Разрешение синхронных потоков на ввод/вывод.
  function X502_StreamsEnable(hnd: t_x502_hnd; streams: LongWord): LongInt; stdcall;
  // Запрещение синхронных потоков на ввод/вывод.
  function X502_StreamsDisable(hnd: t_x502_hnd; streams: LongWord): LongInt; stdcall;
  // Получить значение, какие синхронные потоки разрешены.
  function X502_GetEnabledStreams(hnd: t_x502_hnd; out streams: LongWord): LongInt; stdcall;
  // Запуск синхронных потоков ввода/вывода.
  function X502_StreamsStart(hnd: t_x502_hnd): LongInt; stdcall;
  // Останов синхронных потоков ввода/вывода.
  function X502_StreamsStop(hnd: t_x502_hnd): LongInt; stdcall;
  // Проверка, запущен ли поток данных
  function X502_IsRunning(hnd: t_x502_hnd): LongInt; stdcall;


  // Чтение данных АЦП и цивровых входов из модуля.
  function X502_Recv(hnd: t_x502_hnd; out buf : array of LongWord; size: LongWord; tout : LongWord): LongInt; stdcall;
  // Передача потоковых данных ЦАП и цифровых выходов в модуль.
  function X502_Send(hnd: t_x502_hnd; const buf : array of LongWord; size: LongWord; tout: LongWord): LongInt; stdcall;
  // Обработка принятых отсчетов АЦП от модуля.
  function X502_ProcessAdcData(hnd: t_x502_hnd; const src: array of LongWord;
                               out dest: array of Double; var size : LongWord;
                               flags : LongWord): LongInt; stdcall;
  // Обработка принятых от модуля данных.
 function X502_ProcessData(hnd: t_x502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord):LongInt; stdcall;
  // Обработка принятых от модуля данных ьзовательскими данными.
  function X502_ProcessDataWithUserExt(hnd: t_x502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord;
                     out usr_data: array of LongWord; var usr_data_size: LongWord):LongInt; stdcall;
  // Подготовка данных для вывода в модуль.
  function X502_PrepareData(hnd: t_x502_hnd; const dac1, dac2: array of Double;
                            const digout: array of LongWord; size, flags : LongWord;
                            out out_buf: array of LongWord):LongInt; stdcall;

  // Получить количество отсчетов в буфере потока на ввод.
  function X502_GetRecvReadyCount(hnd: t_x502_hnd; out rdy_cnt: LongWord):LongInt; stdcall;
  // Получить размер свободного места в буфере потока на вывод.
  function X502_GetSendReadyCount(hnd: t_x502_hnd; out rdy_cnt: LongWord):LongInt; stdcall;
  // Получить номер следующего ожидаемого логического канала АЦП для обработки.
  function X502_GetNextExpectedLchNum(hnd: t_x502_hnd; out lch: LongWord):LongInt; stdcall;
  // Начало подготовки вывода синхронных данных
  function X502_PreloadStart(hnd: t_x502_hnd): LongInt; stdcall;

  //Установка ранее загруженного циклического сигнала на вывод
  function X502_OutCycleLoadStart(hnd: t_x502_hnd; size: LongWord):LongInt; stdcall;
  //Установка ранее загруженного циклического сигнала на вывод
  function X502_OutCycleSetup(hnd: t_x502_hnd; flags: LongWord):LongInt; stdcall;
  // Останов вывода циклического сигнала
  function X502_OutCycleStop(hnd: t_x502_hnd; flags: LongWord):LongInt; stdcall;
  // Проверка, завершена ли установка или останов циклического сигнала
  function X502_OutCycleCheckSetupDone(hnd: t_x502_hnd; out done : LongBool):LongInt; stdcall;  
  // Чтение флагов статуса вывода
  function X502_OutGetStatusFlags(hnd: t_x502_hnd; out status : LongWord): LongInt; stdcall;

  // Установка размера буфера для синхронного ввода или вывода.
  function X502_SetStreamBufSize(hnd: t_x502_hnd;  dma_ch, size: LongWord): LongInt; stdcall;
  // Установка шага при передаче потока на ввод или вывод.
  function X502_SetStreamStep(hnd: t_x502_hnd; dma_ch, step: LongWord): LongInt; stdcall;

  {------------ Функции для работы с сигнальным процессором -------------------}

  // Загрузка прошивки сигнального процессора BlackFin.
  function X502_BfLoadFirmware(hnd: t_x502_hnd; filename: string): LongInt; stdcall;
  // Проверка, загружена ли прошивка BlackFIn
  function X502_BfCheckFirmwareIsLoaded(hnd: t_x502_hnd; out version: LongWord): LongInt; stdcall;
  /// Чтение блока данных из памяти сигнального процессора.
  function X502_BfMemRead(hnd: t_x502_hnd; addr : LongWord; out regs: array of LongWord;
                                size: LongWord): LongInt; stdcall;
  /// Запись блока данных в память сигнального процессора.
  function X502_BfMemWrite(hnd: t_x502_hnd; addr : LongWord;
                           const regs: array of LongWord; size: LongWord): LongInt; stdcall;
  /// Передача управляющей команды сигнальному процессору.
  function X502_BfExecCmd(hnd: t_x502_hnd; cmd_code : Word; par : LongWord;
                const snd_data : array of LongWord; snd_size : LongWord;
                out rcv_data : array of LongWord; rcv_size : LongWord;
                tout: LongWord; out recvd_size: LongWord): LongInt; stdcall;

  {-------------------  Функции для работы с Flash-памятью модуля -------------}
  /// Чтение блока данных из Flash-памяти модуля.
  function X502_FlashRead(hnd: t_x502_hnd; addr: LongWord;
                          out data: array of Byte; size: LongWord): LongInt; stdcall;
  ///Запись блока данных во Flash-память модуля.
  function X502_FlashWrite(hnd: t_x502_hnd; addr: LongWord;
                           const data: array of Byte; size: LongWord): LongInt; stdcall;
  /// Стирание блока во Flash-памяти.
  function X502_FlashErase(hnd: t_x502_hnd; addr: LongWord; size: LongWord): LongInt; stdcall;
  /// Разрешение записи в пользовательскую область Flash-памяти.
  function X502_FlashWriteEnable(hnd: t_x502_hnd): LongInt; stdcall;
  /// Запрет записи в пользовательскую область Flash-памяти.
  function X502_FlashWriteDisable(hnd: t_x502_hnd): LongInt; stdcall;

  {-----------------  Дополнительные вспомогательные функции ------------------}
  //Получить версию библиотеки.
  function X502_GetLibraryVersion() : LongWord; stdcall;
  // Получение строки об ошибке
  function X502_GetErrorString(err: LongInt) : string; stdcall;
  // Моргание светодиодом на передней панели.
  function X502_LedBlink(hnd: t_x502_hnd): LongInt; stdcall;
  // Установка подтягивающих резисторов на входных линиях.
  function X502_SetDigInPullup(hnd: t_x502_hnd; pullups : LongWord): LongInt; stdcall;
  // Проверка поддержки модулем заданной возможности
  function X502_CheckFeature(hnd: t_x502_hnd;  feature : LongWord) : LongInt; stdcall;








implementation
  function X502_Create() : t_x502_hnd; stdcall; external 'x502api.dll';
  function X502_Free(hnd: t_x502_hnd) : LongInt; stdcall; external 'x502api.dll';

  function X502_OpenByDevRecord(hnd: t_x502_hnd; var devrec : t_x502_devrec) : LongInt; stdcall; external 'x502api.dll';
  function _free_dev_record_list(var list ; size : LongWord) : LongInt; stdcall; external 'x502api.dll' name 'X502_FreeDevRecordList';
  function X502_Close(hnd : t_x502_hnd) : LongInt; stdcall; external 'x502api.dll';
  function X502_GetDevInfo(hnd : t_x502_hnd; out info : t_x502_info) : LongInt; stdcall; external 'x502api.dll';

  function X502_Configure(hnd: t_x502_hnd; flags: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetLChannel(hnd: t_x502_hnd; lch, phy_ch, mode, range, avg: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetLChannelCount(hnd: t_x502_hnd; lch_cnt : LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_GetLChannelCount(hnd: t_x502_hnd; out lch_cnt: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetAdcFreqDivider(hnd: t_x502_hnd; adc_freq_div : LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetAdcInterframeDelay(hnd: t_x502_hnd; delay : LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetDinFreqDivider(hnd: t_x502_hnd; din_freq_div: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetOutFreqDivider(hnd: t_x502_hnd; out_freq_div : LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetAdcFreq(hnd: t_x502_hnd; var f_acq, f_frame: Double): LongInt; stdcall; external 'x502api.dll';
  function X502_SetDinFreq(hnd: t_x502_hnd; var f_din: Double): LongInt; stdcall; external 'x502api.dll';
  function X502_SetOutFreq(hnd: t_x502_hnd; var f_dout: Double): LongInt; stdcall; external 'x502api.dll';
  function X502_GetAdcFreq(hnd: t_x502_hnd; out f_acq, f_frame: Double): LongInt; stdcall; external 'x502api.dll';
  function X502_SetRefFreq(hnd: t_x502_hnd; freq: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetExtRefFreqValue(hnd: t_x502_hnd; freq : Double): LongInt; stdcall; external 'x502api.dll';
  function X502_GetRefFreqValue(hnd: t_x502_hnd; out freq : Double): LongInt; stdcall; external 'x502api.dll';
  function X502_SetSyncMode(hnd: t_x502_hnd; sync_mode: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetSyncStartMode(hnd: t_x502_hnd; sync_start_mode: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetMode(hnd: t_x502_hnd; mode: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_GetMode(hnd: t_x502_hnd; out mode: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetAdcCoef(hnd: t_x502_hnd; range: LongWord; k, offs: Double): LongInt; stdcall; external 'x502api.dll';
  function X502_GetAdcCoef(hnd: t_x502_hnd; range: LongWord; out k, offs: Double): LongInt; stdcall; external 'x502api.dll';
  function X502_SetDacCoef(hnd: t_x502_hnd; ch : LongWord; k, double: Double): LongInt; stdcall; external 'x502api.dll';
  function X502_GetDacCoef(hnd: t_x502_hnd; ch : LongWord; out k, double: Double): LongInt; stdcall; external 'x502api.dll';
  function X502_CalcAdcFreq(ref_freq: Double; lch_cnt: LongWord; var f_acq, f_frame: Double; out result_freq_div : LongWord; out  result_frame_delay: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_CalcDinFreq(ref_freq: Double; var f_din : Double; out result_freq_div : LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_CalcOutFreq(ref_freq: Double; var f_dout : Double; out result_freq_div : LongWord): LongInt; stdcall; external 'x502api.dll';

  function X502_AsyncOutDac(hnd: t_x502_hnd; ch: LongWord; data: Double; flags: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_AsyncOutDig(hnd: t_x502_hnd; val, msk: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_AsyncInDig(hnd: t_x502_hnd; out din: LongWord): LongInt; stdcall; external 'x502api.dll';
  function _get_adc_frame(hnd: t_x502_hnd; flags: LongWord; tout: LongWord; out data): LongInt; stdcall; external 'x502api.dll' name 'X502_AsyncGetAdcFrame';

  function X502_StreamsEnable(hnd: t_x502_hnd; streams: LongWord): LongInt; stdcall;  external 'x502api.dll';
  function X502_StreamsDisable(hnd: t_x502_hnd; streams: LongWord): LongInt; stdcall;  external 'x502api.dll';
  function X502_GetEnabledStreams(hnd: t_x502_hnd; out streams: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_StreamsStart(hnd: t_x502_hnd): LongInt; stdcall;  external 'x502api.dll';
  function X502_IsRunning(hnd: t_x502_hnd): LongInt; stdcall; external 'x502api.dll';
  function X502_StreamsStop(hnd: t_x502_hnd): LongInt; stdcall;  external 'x502api.dll';
  function _recv(hnd: t_x502_hnd; out buf; size: LongWord; tout : LongWord): LongInt; stdcall;  external 'x502api.dll' name 'X502_Recv';
  function _send(hnd: t_x502_hnd; const buf; size: LongWord; tout : LongWord): LongInt; stdcall;  external 'x502api.dll' name 'X502_Send';
  function _process_adc_data(hnd: t_x502_hnd; const src; out dest; var size : LongWord;
                               flags : LongWord): LongInt; stdcall;  external 'x502api.dll' name 'X502_ProcessAdcData';
  function _process_data(hnd: t_x502_hnd; const src; size: LongWord;
                     flags : LongWord; out adc_data; var adc_data_size : LongWord;
                     out din_data; var din_data_size: LongWord):LongInt; stdcall;  external 'x502api.dll' name 'X502_ProcessData';

  function _process_data_usr(hnd: t_x502_hnd; const src; size: LongWord;
                     flags : LongWord; out adc_data; var adc_data_size : LongWord;
                     out din_data; var din_data_size: LongWord;
                     out usr_data; var usr_data_size: LongWord):LongInt; stdcall;  external 'x502api.dll' name 'X502_ProcessDataWithUserExt';
  function _prepare_data(hnd: t_x502_hnd; const dac1, dac2; const digout; size, flags : LongWord;
                            out out_buf):LongInt; stdcall;  external 'x502api.dll' name 'X502_PrepareData';
  function X502_GetRecvReadyCount(hnd: t_x502_hnd; out rdy_cnt: LongWord):LongInt; stdcall; external 'x502api.dll';
  function X502_GetSendReadyCount(hnd: t_x502_hnd; out rdy_cnt: LongWord):LongInt; stdcall; external 'x502api.dll';
  function X502_GetNextExpectedLchNum(hnd: t_x502_hnd; out lch: LongWord):LongInt; stdcall; external 'x502api.dll';
  function X502_PreloadStart(hnd: t_x502_hnd): LongInt; stdcall; external 'x502api.dll';
  function X502_OutCycleLoadStart(hnd: t_x502_hnd; size: LongWord):LongInt; stdcall; external 'x502api.dll';
  function X502_OutCycleSetup(hnd: t_x502_hnd; flags: LongWord):LongInt; stdcall; external 'x502api.dll';
  function X502_OutCycleStop(hnd: t_x502_hnd; flags: LongWord):LongInt; stdcall; external 'x502api.dll';
  function X502_OutCycleCheckSetupDone(hnd: t_x502_hnd; out done : LongBool):LongInt; stdcall; external 'x502api.dll';
  function X502_OutGetStatusFlags(hnd: t_x502_hnd; out status : LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetStreamBufSize(hnd: t_x502_hnd;  dma_ch, size: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_SetStreamStep(hnd: t_x502_hnd; dma_ch, step: LongWord): LongInt; stdcall; external 'x502api.dll';

  function X502_BfCheckFirmwareIsLoaded(hnd: t_x502_hnd; out version: LongWord): LongInt; stdcall; external 'x502api.dll';
  function _bf_load_firm(hnd: t_x502_hnd; filename: PAnsiChar): LongInt; stdcall; external 'x502api.dll' name 'X502_BfLoadFirmware';
  function _bf_mem_read(hnd: t_x502_hnd; addr : LongWord; out regs; size: LongWord): LongInt; stdcall; external 'x502api.dll' name 'X502_BfMemRead';
  function _bf_mem_write(hnd: t_x502_hnd; addr : LongWord; const regs; size: LongWord): LongInt; stdcall; external 'x502api.dll' name 'X502_BfMemWrite';
  function _bf_exec_cmd(hnd: t_x502_hnd; cmd_code : Word; par : LongWord;
                const snd_data; snd_size : LongWord; out rcv_data; rcv_size : LongWord;
                tout: LongWord; out recved_size: LongWord): LongInt; stdcall; external 'x502api.dll' name 'X502_BfExecCmd';

  function _flash_read(hnd: t_x502_hnd; addr: LongWord; out data; size: LongWord): LongInt; stdcall; external 'x502api.dll' name 'X502_FlashRead';
  function _flash_write(hnd: t_x502_hnd; addr: LongWord; const data; size: LongWord): LongInt; stdcall; external 'x502api.dll' name 'X502_FlashWrite';
  function X502_FlashErase(hnd: t_x502_hnd; addr: LongWord; size: LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_FlashWriteEnable(hnd: t_x502_hnd): LongInt; stdcall; external 'x502api.dll';
  function X502_FlashWriteDisable(hnd: t_x502_hnd): LongInt; stdcall; external 'x502api.dll';

  function X502_GetLibraryVersion() : LongWord; stdcall; external 'x502api.dll';
  function _get_err_str(err : LongInt) : PAnsiChar; stdcall; external 'x502api.dll' name 'X502_GetErrorString';
  function X502_LedBlink(hnd: t_x502_hnd): LongInt; stdcall;  external 'x502api.dll';
  function X502_SetDigInPullup(hnd: t_x502_hnd; pullups : LongWord): LongInt; stdcall; external 'x502api.dll';
  function X502_CheckFeature(hnd: t_x502_hnd;  feature : LongWord) : LongInt; stdcall; external 'x502api.dll';


  function X502_FreeDevRecordList(list : array of t_x502_devrec; size : LongWord) : LongInt; stdcall; overload;
  begin
    X502_FreeDevRecordList:=_free_dev_record_list(list, size);
  end;
  function X502_FreeDevRecordList(var rec : t_x502_devrec) : LongInt; stdcall; overload;
  begin
    X502_FreeDevRecordList:=_free_dev_record_list(rec, 1);
  end;

  function X502_GetErrorString(err: LongInt) : string;
  begin
    X502_GetErrorString:= string(_get_err_str(err));
  end;

  function X502_AsyncGetAdcFrame(hnd: t_x502_hnd; flags: LongWord; tout: LongWord; out data: array of Double): LongInt; stdcall;
  var err: LongInt;
      lch_cnt: LongWord;
  begin
    err := X502_GetLChannelCount(hnd, lch_cnt);
    if err = X502_ERR_OK then
    begin
      if LongWord(Length(data)) < lch_cnt then
        err := X502_ERR_INSUFFICIENT_ARRAY_SIZE
      else
        err := _get_adc_frame(hnd,flags,tout,data);
    end;
    X502_AsyncGetAdcFrame := err;
  end;

  function X502_Recv(hnd: t_x502_hnd; out buf : array of LongWord; size: LongWord; tout : LongWord): LongInt; stdcall;
  begin
    if LongWord(Length(buf)) < size then
      X502_Recv := X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_Recv := _recv(hnd, buf, size, tout);
  end;

  function X502_Send(hnd: t_x502_hnd; const buf : array of LongWord; size: LongWord; tout: LongWord): LongInt; stdcall;
  begin
    if LongWord(Length(buf)) < size then
      X502_Send:=X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_Send:=_send(hnd,buf,size,tout);
  end;


 function X502_ProcessAdcData(hnd: t_x502_hnd; const src: array of LongWord;
                               out dest: array of Double; var size : LongWord;
                               flags : LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(src)) < size) or (LongWord(Length(dest)) < size) then
      X502_ProcessAdcData:=X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_ProcessAdcData:=_process_adc_data(hnd, src, dest, size, flags);
  end;

  function X502_ProcessData(hnd: t_x502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord):LongInt; stdcall;
  begin
    if (LongWord(Length(adc_data)) < adc_data_size) or (LongWord(Length(din_data)) < din_data_size)
            or (LongWord(Length(src)) < size) then
      X502_ProcessData:=X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_ProcessData:=_process_data(hnd, src, size, flags, adc_data, adc_data_size, din_data, din_data_size);
  end;

  function X502_ProcessDataWithUserExt(hnd: t_x502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord;
                     out usr_data: array of LongWord; var usr_data_size: LongWord):LongInt; stdcall;
  begin
    if (LongWord(Length(adc_data)) < adc_data_size) or (LongWord(Length(din_data)) < din_data_size)
            or (LongWord(Length(src)) < size) or (LongWord(Length(usr_data)) < usr_data_size) then
      X502_ProcessDataWithUserExt:=X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_ProcessDataWithUserExt:=_process_data_usr(hnd, src,size,flags,adc_data,
                                adc_data_size, din_data, din_data_size, usr_data, usr_data_size);
  end;

  function X502_PrepareData(hnd: t_x502_hnd; const dac1, dac2: array of Double;
                            const digout: array of LongWord; size, flags : LongWord;
                            out out_buf: array of LongWord):LongInt; stdcall;
  begin
    X502_PrepareData:=_prepare_data(hnd, dac1, dac2, digout, size, flags, out_buf);
  end;

  function X502_BfLoadFirmware(hnd: t_x502_hnd; filename: string): LongInt; stdcall;
  begin
    X502_BfLoadFirmware:=_bf_load_firm(hnd, PAnsiChar(AnsiString(filename)));
  end;

  function X502_BfMemRead(hnd: t_x502_hnd; addr : LongWord; out regs: array of LongWord;
                                size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(regs)) < size) then
      X502_BfMemRead := X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_BfMemRead:=_bf_mem_read(hnd, addr, regs, size);
  end;

  function X502_BfMemWrite(hnd: t_x502_hnd; addr : LongWord;
                           const regs: array of LongWord; size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(regs)) < size) then
      X502_BfMemWrite := X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_BfMemWrite:=_bf_mem_write(hnd, addr, regs, size);
  end;

  function X502_BfExecCmd(hnd: t_x502_hnd; cmd_code : Word; par : LongWord;
                const snd_data : array of LongWord; snd_size : LongWord;
                out rcv_data : array of LongWord; rcv_size : LongWord;
                tout: LongWord; out recvd_size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(snd_data)) < snd_size) or
       (LongWord(Length(rcv_data)) < rcv_size)  then
      X502_BfExecCmd := X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_BfExecCmd:=_bf_exec_cmd(hnd, cmd_code, par, snd_data, snd_size,
                rcv_data, rcv_size, tout, recvd_size);
  end;

  function X502_FlashRead(hnd: t_x502_hnd; addr: LongWord;
                          out data: array of Byte; size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(data)) < size) then
      X502_FlashRead := X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_FlashRead:=_bf_mem_write(hnd, addr, data, size);
  end;
  ///Запись блока данных во Flash-память модуля.
  function X502_FlashWrite(hnd: t_x502_hnd; addr: LongWord;
                           const data: array of Byte; size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(data)) < size) then
      X502_FlashWrite := X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_FlashWrite:=_bf_mem_write(hnd, addr, data, size);
  end;
end.

