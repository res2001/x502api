unit l502api;
interface
uses Windows, SysUtils, x502api;



  //получение списка серийных номеров
  function L502_GetSerialList(out serials: array of string; flags: LongWord; out devcnt: LongWord) : LongInt; overload;
  function L502_GetSerialList(out serials: array of string; flags: LongWord) : LongInt; overload;
  //установить связь с модулем по серийному номеру
  function L502_Open(hnd: t_x502_hnd; serial: string): LongInt; stdcall;

  function L502_GetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord;  out devcnt: LongWord) : LongInt; overload;
  function L502_GetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord) : LongInt; overload;



{ ----------- типы и функции, оставленные для обратной совместимости ----------}
const
  // Максимальное количество логических каналов в таблице
  L502_LTABLE_MAX_CH_CNT      = 256;
  // Количество диапазонов для измерения напряжений
  L502_ADC_RANGE_CNT          = 6;
  // Максимальное значение для аппаратного усреднения по логическому каналу
  L502_LCH_AVG_SIZE_MAX       = 128;
  //Максимальное значения делителя частоты АЦП
  L502_ADC_FREQ_DIV_MAX       = (1024*1024);
  // Максимальное значение делителя частоты синхронного цифрового ввода
  L502_DIN_FREQ_DIV_MAX       = (1024*1024);
  // Максимальное значение межкадровой задержки для АЦП
  L502_ADC_INTERFRAME_DELAY_MAX  = $1FFFFF;
  // Таймаут по умолчанию для выполнения команды к BlackFin
  L502_BF_CMD_DEFAULT_TOUT     = 500;

  // Код АЦП, соответствующий максимальному значению шкалы
  L502_ADC_SCALE_CODE_MAX      = 6000000;
  // Код ЦАП, соответствующий максимальному значению шкалы
  L502_DAC_SCALE_CODE_MAX      = 30000;

  // Максимальное количество символов в строке с названием устройства
  L502_DEVNAME_SIZE            = X502_DEVNAME_SIZE;
  // Максимальное количество символов в строке с серийным номером
  L502_SERIAL_SIZE             = X502_SERIAL_SIZE;

  //Размер пользовательской области Flash-памяти
  L502_FLASH_USER_SIZE         = $100000;
  //Стандартный таймаут на выполнение запроса к BlackFin в мс
  L502_BF_REQ_TOUT             = 500;
  // Диапазон ЦАП в вольтах
  L502_DAC_RANGE               = 5;
  // Количество каналов ЦАП
  L502_DAC_CH_CNT              = 2;

  // слово в потоке, означающее, что произошло переполнение
  L502_STREAM_IN_MSG_OVERFLOW  = $01010000;

  // Максимально возможное значение внешней опорной частоты 
  L502_EXT_REF_FREQ_MAX        = 2000000;


  { ----------------- Коды ошибок библиотеки ---------------------------}
    // Функция выполнена без ошибок
    L502_ERR_OK                           = 0;
    // В функцию передан недействительный описатель модуля
    L502_ERR_INVALID_HANDLE               = -1;
    // Ошибка выделения памяти
    L502_ERR_MEMORY_ALLOC                 = -2;
    // Попытка открыть уже открытое устройство
    L502_ERR_ALREADY_OPENED               = -3;
    // Устройство с заданными параметрами не найдено в системе
    L502_ERR_DEVICE_NOT_FOUND             = -4;
    // Доступ к устройству запрещен (Как правило из-за того, что устройство уже открыто в другой программе)
    L502_ERR_DEVICE_ACCESS_DENIED         = -5;
    // Ошибка открытия устройства 
    L502_ERR_DEVICE_OPEN                  = -6;
    // В функцию передан недействительный указатель 
    L502_ERR_INVALID_POINTER              = -7;
    // Функция не может быть выполнена при запущенном потоке сбора данных 
    L502_ERR_STREAM_IS_RUNNING            = -8;
    // Ошибка чтения данных синхронного ввода 
    L502_ERR_RECV                         = -9;
    // Ошибка записи данных для синхронного вывода 
    L502_ERR_SEND                         = -10;
    // Произошло переполнение внутреннего буфера для потока синхронного ввода 
    L502_ERR_STREAM_OVERFLOW              = -11;
    // Неизвестное сообщение в потоке синхронного ввода 
    L502_ERR_UNSUP_STREAM_MSG             = -12;
    // Ошибка создания системного мьютекса 
    L502_ERR_MUTEX_CREATE                 = -13;
    // Неверный описатель мьютекса 
    L502_ERR_MUTEX_INVALID_HANDLE         = -14;
    // Истекло время ожидания освобождения мьютекса 
    L502_ERR_MUTEX_LOCK_TOUT              = -15;
    // Ошибка освобождения мьютекса 
    L502_ERR_MUTEX_RELEASE                = -16;
    // Недостаточно системных ресурсов 
    L502_ERR_INSUFFICIENT_SYSTEM_RESOURCES= -17;
    // Данная возможность еще не реализована 
    L502_ERR_NOT_IMPLEMENTED              = -18;
    // Недостаточный размер массива
    L502_ERR_INSUFFICIENT_ARRAY_SIZE      = -19;
    // Ошибка чтения регистра FPGA
    L502_ERR_FPGA_REG_READ                = -20;
    // Ошибка записи регистра FPGA
    L502_ERR_FPGA_REG_WRITE               = -21;
    // Сбор данных уже остановлен
    L502_ERR_STREAM_IS_NOT_RUNNING        = -22;
    // Задан неверный размер логической таблицы 
    L502_ERR_INVALID_LTABLE_SIZE          = -102;
    // Задан неверный номер логического канала 
    L502_ERR_INVALID_LCH_NUMBER           = -103;
    // Неверно задано значение диапазона АЦП 
    L502_ERR_INVALID_LCH_RANGE            = -104;
    // Неверно задан режим измерения для логического канала 
    L502_ERR_INVALID_LCH_MODE             = -105;
    // Неверно задан номер физического канала при настройке логического 
    L502_ERR_INVALID_LCH_PHY_NUMBER       = -106;
    // Неверно задан размер усреднения для логического канала 
    L502_ERR_INVALID_LCH_AVG_SIZE         = -107;
    // Неверно задан делитель частоты сбора данных АЦП 
    L502_ERR_INVALID_ADC_FREQ_DIV         = -108;
    // Неверно задан делитель частоты синхронного ввода цифровых линий 
    L502_ERR_INVALID_DIN_FREQ_DIV         = -108;
    // Неверно задан режим работы модуля L502 
    L502_ERR_INVALID_MODE                 = -109;
    // Неверный номер канала ЦАП 
    L502_ERR_INVALID_DAC_CHANNEL          = -110;
    // Неверный код выбора опорной частоты синхронизации 
    L502_ERR_INVALID_REF_FREQ             = -111;
    // Неверно задано значение межкадровой задержки 
    L502_ERR_INVALID_INTERFRAME_DELAY     = -112;
    // Неверно задан режим синхронизации 
    L502_ERR_INVALID_SYNC_MODE            = -113;
    // Неверно задан номер канала DMA 
    L502_ERR_INVALID_DMA_CH               = -114;
    // Ошибка захвата опорной частоты синхронизации
    L502_ERR_REF_FREQ_NOT_LOCKED          = -131;
    // Управляющий запрос к драйверу завершен с ошибкой 
    L502_ERR_IOCTL_FAILD                  = -132;
    // Истек таймаут ожидания завершения выполнения управляющего запроса к драйверу 
    L502_ERR_IOCTL_TIMEOUT                = -133;
    // Ошибка получения информации о устройстве от драйвера 
    L502_ERR_GET_INFO                     = -134;
    // За время ожидания не было считано новое слово с цифровых линий 
    L502_ERR_DIG_IN_NOT_RDY               = -135;
    // Принято недостаточно слов от модуля 
    L502_ERR_RECV_INSUFFICIENT_WORDS      = -136;
    // Попытка выполнить операцию, требующую наличие ЦАП, при его отсутствии 
    L502_ERR_DAC_NOT_PRESENT              = -137;
    // Неверный номер канала в обрабатываемом потоке синхронного ввода 
    L502_ERR_PROC_INVALID_CH_NUM          = -140;
    // Неверный код диапазона в обрабатываемом потоке синхронного ввода 
    L502_ERR_PROC_INVALID_CH_RANGE        = -141;
    // Задан неверный адрес во Flash-памяти 
    L502_ERR_FLASH_INVALID_ADDR           = -142;
    // Задан неверный размер блока данных при работе с Flash-памятью 
    L502_ERR_FLASH_INVALID_SIZE           = -143;
    // Истек таймаут ожидания завершения записи во Flash-память 
    L502_ERR_FLASH_WRITE_TOUT             = -144;
    // Истек таймаут ожидания завершения стирания блока Flash-памяти 
    L502_ERR_FLASH_ERASE_TOUT             = -145;
    // Заданная область для стирания Flash-памяти нарушает границу блока в 4 Кбайт 
    L502_ERR_FLASH_SECTOR_BOUNDARY        = -146;
    // Не удалось открыть файл прошивки BlackFin
    L502_ERR_LDR_FILE_OPEN                = -180;
    // Ошибка чтения из фала прошивки BlackFin 
    L502_ERR_LDR_FILE_READ                = -181;
    // Неверный формат файла прошивки BlackFin 
    L502_ERR_LDR_FILE_FORMAT              = -182;
    // Используются возможность LDR-файла, недоступные при записи прошивки BlackFin по HDMA 
    L502_ERR_LDR_FILE_UNSUP_FEATURE       = -183;
    // Неверный стартовый адрес программы в прошивке BlackFin 
    L502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR  = -184;
    // Истек таймаут выполнения запроса на чтения/запись памяти BlackFin 
    L502_ERR_BF_REQ_TIMEOUT               = -185;
    // Команда для BlackFin все еще находится в процессе обработки 
    L502_ERR_BF_CMD_IN_PROGRESS           = -186;
    // Истекло время выполнения управляющей команды процессором BlackFin 
    L502_ERR_BF_CMD_TIMEOUT               = -187;
    // Возвращено недостаточно данных в ответ на команду к BlackFin
    L502_ERR_BF_CMD_RETURN_INSUF_DATA     = -188;
    // Истек таймаут ожидания готовности процессора BlackFin к записи прошивки 
    L502_ERR_BF_LOAD_RDY_TOUT             = -189;
    // Попытка выполнить операцию для которой нужен сигнальный процессор при
    // отсутствии сигнального процессора в модуле
    L502_ERR_BF_NOT_PRESENT               = -190;
    // Неверный адрес памяти BlackFin при записи или чтении по HDMA 
    L502_ERR_BF_INVALID_ADDR              = -191;
    // Неверный размер данных, передаваемых с управляющей командой в BlackFin
    L502_ERR_BF_INVALID_CMD_DATA_SIZE     = -192;


    {-------- Флаги, управляющие поиском присутствующих модулей ------}
     // Признак, что нужно вернуть серийные номера только тех устройств,
     //   которые еще не открыты
    L502_GETDEVS_FLAGS_ONLY_NOT_OPENED = X502_GETDEVS_FLAGS_ONLY_NOT_OPENED;



    {-------- Флаги для управления цифровыми выходами. ---------------}
    //Запрещение (перевод в третье состояние) старшей половины цифровых выходов
    L502_DIGOUT_WORD_DIS_H = $00020000;
    //Запрещение младшей половины цифровых выходов
    L502_DIGOUT_WORD_DIS_L = $00010000;


    {-------- Константы для выбора опорной частоты -------------------}
    L502_REF_FREQ_2000KHZ  = 2000000; // частота 2МГц
    L502_REF_FREQ_1500KHZ  = 1500000; // частота 1.5МГц

    {-------- Диапазоны измерения для канала АЦП ----------------------}
    L502_ADC_RANGE_10 = 0; // диапазон +/-10V
    L502_ADC_RANGE_5  = 1; // диапазон +/-5V
    L502_ADC_RANGE_2  = 2; // диапазон +/-2V
    L502_ADC_RANGE_1  = 3; // диапазон +/-1V
    L502_ADC_RANGE_05 = 4; // диапазон +/-0.5V
    L502_ADC_RANGE_02 = 5; // диапазон +/-0.2V


    {-------- Режим измерения для логического канала ------------------}
    L502_LCH_MODE_COMM = 0; // Измерение напряжения относительно общей земли
    L502_LCH_MODE_DIFF = 1; // Дифференциальное измерение напряжения
    L502_LCH_MODE_ZERO = 2; // Измерение собственного нуля


    {-------- Режимы синхронизации ------------------------------------}
    L502_SYNC_INTERNAL        = 0; // внутренний сигнал
    L502_SYNC_EXTERNAL_MASTER = 1; // от внешнего мастера по разъему синхронизации
    L502_SYNC_DI_SYN1_RISE    = 2; // по фронту сигнала DI_SYN1
    L502_SYNC_DI_SYN2_RISE    = 3; // по фронту сигнала DI_SYN2
    L502_SYNC_DI_SYN1_FALL    = 6; // по спаду сигнала DI_SYN1
    L502_SYNC_DI_SYN2_FALL    = 7;  // по спаду сигнала DI_SYN2

    {-------- Флаги, управляющие обработкой принятых данных -------------------}
    L502_PROC_FLAGS_VOLT       = 1; // Признак, что нужно преобразовать значения
                                   // АЦП в вольты

    {-------- Флаги для обозначения синхронных потоков данных -----------------}
    L502_STREAM_ADC  = $01; //Поток данных от АЦП
    L502_STREAM_DIN  = $02; // Поток данных с цифровых входов
    L502_STREAM_DAC1 = $10; // Поток данных первого канала ЦАП
    L502_STREAM_DAC2 = $20; // Поток данных второго канала ЦАП
    L502_STREAM_DOUT = $40; // Поток данных на цифровые выводы
    // объединение всех флагов, обозначающих потоки данных на ввод
    L502_STREAM_ALL_IN = L502_STREAM_ADC or L502_STREAM_DIN;
    // объединение всех флагов, обозначающих потоки данных на вывод
    L502_STREAM_ALL_OUT = L502_STREAM_DAC1 or L502_STREAM_DAC2 or L502_STREAM_DOUT;


    {--- Константы, определяющие тип передаваемого отсчета из ПК в модуль -----}
    L502_STREAM_OUT_WORD_TYPE_DOUT   = $0; // Цифровой вывод
    L502_STREAM_OUT_WORD_TYPE_DAC1   = $40000000; // Код для 1-го канала ЦАП
    L502_STREAM_OUT_WORD_TYPE_DAC2   = $80000000; // Код для 2-го канала ЦАП

    {------------------ Режим работы модуля L502 ------------------------------}
    L502_MODE_FPGA  = 0; // Все потоки данных передаются через ПЛИС минуя
                         //   сигнальный процессор BlackFin
    L502_MODE_DSP   = 1; // Все потоки данных передаются через сигнальный
                         //   процессор, который должен быть загружен
                         //   прошивкой для обработки этих потоков
    L502_MODE_DEBUG = 2; // Отладочный режим

    {------------------- Номера каналов ЦАП -----------------------------------}
    L502_DAC_CH1 = 0;  //Первый канал ЦАП
    L502_DAC_CH2 = 1;  //Второй канал ЦАП

    {----------- Флаги, используемые при выводе данных на ЦАП -----------------}
    // Указывает, что значение задано в Вольтах и при выводе его нужно
    // перевести в коды ЦАП. Если флаг не указан, то считается, что значение
    // изначально в кодах
    L502_DAC_FLAGS_VOLT    = $0001;
    // Указывает, что нужно применить калибровочные коэффициенты перед
    //    выводом значения на ЦАП.
    L502_DAC_FLAGS_CALIBR  = $0002;

    {------------------ Номера каналов DMA ------------------------------------}
    L502_DMA_CH_IN  = 0; // Общий канал DMA на ввод
    L502_DMA_CH_OUT = 1; // Общий канал DMA на вывод

    {--- Цифровые линии, на которых можно включить подтягивающие резисторы ----}
    L502_PULLUPS_DI_H    = $01; // Старшая половина цифровых входов
    L502_PULLUPS_DI_L    = $02; // Младшая половина цифровых входов
    L502_PULLUPS_DI_SYN1 = $04; // Линия SYN1
    L502_PULLUPS_DI_SYN2 = $08; // Линия SYN2



    {--------------- Флаги, определяющие наличие опций в модуле ---------------}
    // Признак наличия двухканального канального ЦАП
    L502_DEVFLAGS_DAC_PRESENT         = X502_DEVFLAGS_DAC_PRESENT;
    // Признак наличия сигнального процессора BlackFin
    L502_DEVFLAGS_BF_PRESENT          = X502_DEVFLAGS_BF_PRESENT;
    // Признак наличия гальваноразвязки
    L502_DEVFLAGS_GAL_PRESENT         = X502_DEVFLAGS_GAL_PRESENT;
    // Признак, что во Flash-памяти присутствует информация о модуле
    L502_DEVFLAGS_FLASH_DATA_VALID    = $00010000;
    // Признак, что во Flash-памяти присутствуют действительные калибровочные
    //   коэффициенты АЦП
    L502_DEVFLAGS_FLASH_ADC_CALIBR_VALID = $00020000;
    // Признак, что во Flash-памяти присутствуют действительные калибровочные
    //    коэффициенты ЦАП
    L502_DEVFLAGS_FLASH_DAC_CALIBR_VALID = $00040000;

    {----------------Флаги для режима циклического вывода----------------------}
    // Флаг указывает, что останов или смена сигнала могут произойти без ожидания
    //    конца цикла предыдущего сигнала.
    L502_OUT_CYCLE_FLAGS_FORCE = $01;


type
  // описатель модуля - указатель на непрозрачную структуру
  t_l502_hnd = t_x502_hnd;


  { калибровочные коэффициенты диапазона }
  t_l502_cbr_coef = record
    offs: Double;  // смещение нуля
    k : Double; // коэффициент шкалы
  end;


  { Калибровочные коэффициенты модуля. }
  t_l502_cbr = record
    //Калибровочные коэффициенты АЦП
    adc: array[0..L502_ADC_RANGE_CNT-1] of t_l502_cbr_coef;
    res1: array[0..63] of LongWord;
    // Калибровочные коэффициенты ЦАП
    dac: array[0..L502_DAC_CH_CNT-1] of t_l502_cbr_coef;
    res2: array[0..19] of LongWord;
  end;

  { Информация о модуле L502.}
  t_l502_info = record
    name: array[0..L502_DEVNAME_SIZE-1] of AnsiChar; // Название устройства ("L502")
    serial: array[0..L502_SERIAL_SIZE-1] of AnsiChar; // Серийный номер
    devflags: LongWord; // Флаги описывающие наличие в модуле определенных опций
    fpga_ver : Word; // Версия ПЛИС (старший байт - мажорная, младший - минорная)
    plda_ver : Byte;  // Версия ПЛИС, управляющего аналоговой частью
    res : array[0..120] of Byte; //резерв
    cbr : t_l502_cbr; // Заводские калибровочные коэффициенты (из Flash-памяти)
  end;

  function L502_Create(): t_l502_hnd; stdcall;
  function L502_Free(hnd: t_l502_hnd): LongInt; stdcall;

  //разорвать связь с модулем
  function L502_Close(hnd: t_l502_hnd): LongInt; stdcall;
  //получение информации о модуле
  function L502_GetDevInfo(hnd: t_l502_hnd; out info: t_l502_info) : LongInt; stdcall;

  // Передача установленных настроек в модуль
  function L502_Configure(hnd: t_l502_hnd; flags: LongWord): LongInt; stdcall;
  // Установка параметров логического канала
  function L502_SetLChannel(hnd: t_l502_hnd; lch, phy_ch, mode, range, avg: LongWord): LongInt; stdcall;
  // Установка количества логических каналов
  function L502_SetLChannelCount(hnd: t_l502_hnd; lch_cnt : LongWord): LongInt; stdcall;
  // Получение количества логических каналов
  function L502_GetLChannelCount(hnd: t_l502_hnd; out lch_cnt: LongWord): LongInt; stdcall;
  // Установка делителя частоты сбора для АЦП
  function L502_SetAdcFreqDivider(hnd: t_l502_hnd; adc_freq_div : LongWord): LongInt; stdcall;
  // Установка значения межкадровой задержки для АЦП
  function L502_SetAdcInterframeDelay(hnd: t_l502_hnd; delay : LongWord): LongInt; stdcall;
  // Установка делителя частоты синхронного ввода с цифровых линий
  function L502_SetDinFreqDivider(hnd: t_l502_hnd; din_freq_div: LongWord): LongInt; stdcall;
  // Установка частоты сбора АЦП
  function L502_SetAdcFreq(hnd: t_l502_hnd; var f_acq, f_frame: Double): LongInt; stdcall;
  // Установка частоты синхронного ввода с цифровых входов
  function L502_SetDinFreq(hnd: t_l502_hnd; var f_din: Double): LongInt; stdcall;
  // Получить текущие значения частот сбора АЦП
  function L502_GetAdcFreq(hnd: t_l502_hnd; out f_acq, f_frame: Double): LongInt; stdcall;
  // Установка значения опорной частоты синхронизации.
  function L502_SetRefFreq(hnd: t_l502_hnd; freq: LongWord): LongInt; stdcall;
  // Установка режима генерации частоты синхронизации.
  function L502_SetSyncMode(hnd: t_l502_hnd; sync_mode: LongWord): LongInt; stdcall;
  // Установка режима запуска частоты синхронизации.
  function L502_SetSyncStartMode(hnd: t_l502_hnd; sync_start_mode: LongWord): LongInt; stdcall;
  // Установить режим работы модуля
  function L502_SetMode(hnd: t_l502_hnd; mode: LongWord): LongInt; stdcall;
  // Получение текущего режима работы модуля.
  function L502_GetMode(hnd: t_l502_hnd; out mode: LongWord): LongInt; stdcall;
  // Установить коэффициенты для калибровки значений АЦП.
  function L502_SetAdcCoef(hnd: t_l502_hnd; range: LongWord; k, offs: Double): LongInt; stdcall;
  // Получение текущих калибровочных коэффициентов АЦП.
  function L502_GetAdcCoef(hnd: t_l502_hnd; range: LongWord; out k, offs: Double): LongInt; stdcall;


  {----------------------- Функции асинхронного ввода-вывода ------------------}
  // Асинхронный вывод данных на канал ЦАП.
  function L502_AsyncOutDac(hnd: t_l502_hnd; ch: LongWord; data: Double; flags: LongWord): LongInt; stdcall;
  // Асинхронный вывод данных на цифровые выходы.
  function L502_AsyncOutDig(hnd: t_l502_hnd; val, msk: LongWord): LongInt; stdcall;
  // Асинхронный ввод значений с цифровых входов.
  function L502_AsyncInDig(hnd: t_l502_hnd; out din: LongWord): LongInt; stdcall;
  // Асинхронный ввод одного кадра АЦП.
  function L502_AsyncGetAdcFrame(hnd: t_l502_hnd; flags: LongWord; tout: LongWord; out data: array of Double): LongInt; stdcall;


  {-------------- Функции для работы с синхронным потоковым вводом-выводом ----}
  // Разрешение синхронных потоков на ввод/вывод.
  function L502_StreamsEnable(hnd: t_l502_hnd; streams: LongWord): LongInt; stdcall;
  // Запрещение синхронных потоков на ввод/вывод.
  function L502_StreamsDisable(hnd: t_l502_hnd; streams: LongWord): LongInt; stdcall;
  // Запуск синхронных потоков ввода/вывода.
  function L502_StreamsStart(hnd: t_l502_hnd): LongInt; stdcall;
  // Останов синхронных потоков ввода/вывода.
  function L502_StreamsStop(hnd: t_l502_hnd): LongInt; stdcall;
  // Проверка, запущен ли поток данных
  function L502_IsRunning(hnd: t_l502_hnd): LongInt; stdcall;


  // Чтение данных АЦП и цивровых входов из модуля.
  function L502_Recv(hnd: t_l502_hnd; out buf : array of LongWord; size: LongWord; tout : LongWord): LongInt; stdcall;
  // Передача потоковых данных ЦАП и цифровых выходов в модуль.
  function L502_Send(hnd: t_l502_hnd; const buf : array of LongWord; size: LongWord; tout: LongWord): LongInt; stdcall;
  // Обработка принятых отсчетов АЦП от модуля.
  function L502_ProcessAdcData(hnd: t_l502_hnd; const src: array of LongWord;
                               out dest: array of Double; var size : LongWord;
                               flags : LongWord): LongInt; stdcall;
  // Обработка принятых от модуля данных.
 function L502_ProcessData(hnd: t_l502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord):LongInt; stdcall;
  // Обработка принятых от модуля данных ьзовательскими данными.
  function L502_ProcessDataWithUserExt(hnd: t_l502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord;
                     out usr_data: array of LongWord; var usr_data_size: LongWord):LongInt; stdcall;
  // Подготовка данных для вывода в модуль.
  function L502_PrepareData(hnd: t_l502_hnd; const dac1, dac2: array of Double;
                            const digout: array of LongWord; size, flags : LongWord;
                            out out_buf: array of LongWord):LongInt; stdcall;

  // Получить количество отсчетов в буфере потока на ввод.
  function L502_GetRecvReadyCount(hnd: t_l502_hnd; out rdy_cnt: LongWord):LongInt; stdcall;
  // Получить размер свободного места в буфере потока на вывод.
  function L502_GetSendReadyCount(hnd: t_l502_hnd; out rdy_cnt: LongWord):LongInt; stdcall;
  // Получить номер следующего ожидаемого логического канала АЦП для обработки.
  function L502_GetNextExpectedLchNum(hnd: t_l502_hnd; out lch: LongWord):LongInt; stdcall;
  // Начало подготовки вывода синхронных данных
  function L502_PreloadStart(hnd: t_l502_hnd): LongInt; stdcall;

  //Установка ранее загруженного циклического сигнала на вывод
  function L502_OutCycleLoadStart(hnd: t_l502_hnd; size: LongWord):LongInt; stdcall;
  //Установка ранее загруженного циклического сигнала на вывод
  function L502_OutCycleSetup(hnd: t_l502_hnd; flags: LongWord):LongInt; stdcall;
  // Останов вывода циклического сигнала
  function L502_OutCycleStop(hnd: t_l502_hnd; flags: LongWord):LongInt; stdcall;

  // Установка размера буфера в драйвере для синхронного ввода или вывода.
  function L502_SetDmaBufSize(hnd: t_l502_hnd;  dma_ch, size: LongWord): LongInt; stdcall;
  // Установка шага прерывания при передаче потока по DMA.
  function L502_SetDmaIrqStep(hnd: t_l502_hnd; dma_ch, step: LongWord): LongInt; stdcall;
  
  {------------ Функции для работы с сигнальным процессором -------------------}
  function L502_BfCheckFirmwareIsLoaded(hnd: t_l502_hnd; out version: LongWord): LongInt; stdcall;
  /// Загрузка прошивки сигнального процессора BlackFin.
  function L502_BfLoadFirmware(hnd: t_l502_hnd; filename: string): LongInt; stdcall;
  /// Чтение блока данных из памяти сигнального процессора.
  function L502_BfMemRead(hnd: t_l502_hnd; addr : LongWord; out regs: array of LongWord;
                                size: LongWord): LongInt; stdcall;
  /// Запись блока данных в память сигнального процессора.
  function L502_BfMemWrite(hnd: t_l502_hnd; addr : LongWord;
                           const regs: array of LongWord; size: LongWord): LongInt; stdcall;
  /// Передача управляющей команды сигнальному процессору.
  function L502_BfExecCmd(hnd: t_l502_hnd; cmd_code : Word; par : LongWord;
                const snd_data : array of LongWord; snd_size : LongWord;
                out rcv_data : array of LongWord; rcv_size : LongWord;
                tout: LongWord; out recvd_size: LongWord): LongInt; stdcall;

  {-------------------  Функции для работы с Flash-памятью модуля -------------}
  /// Чтение блока данных из Flash-памяти модуля.
  function L502_FlashRead(hnd: t_l502_hnd; addr: LongWord;
                          out data: array of Byte; size: LongWord): LongInt; stdcall;
  ///Запись блока данных во Flash-память модуля.
  function L502_FlashWrite(hnd: t_l502_hnd; addr: LongWord;
                           const data: array of Byte; size: LongWord): LongInt; stdcall;
  /// Стирание блока во Flash-памяти.
  function L502_FlashErase(hnd: t_l502_hnd; addr: LongWord; size: LongWord): LongInt; stdcall;
  /// Разрешение записи в пользовательскую область Flash-памяти.
  function L502_FlashWriteEnable(hnd: LongWord): LongInt; stdcall;
  /// Запрет записи в пользовательскую область Flash-памяти.
  function L502_FlashWriteDisable(hnd: t_l502_hnd): LongInt; stdcall;

  {-----------------  Дополнительные вспомогательные функции ------------------}
  //Получить версию библиотеки.
  function L502_GetDllVersion() : LongWord; stdcall;
  // Получить версию драйвера устройства
  function L502_GetDriverVersion(hnd: t_l502_hnd; out ver: LongWord): LongInt; stdcall;
  // Получение строки об ошибке
  function L502_GetErrorString(err: LongInt) : string; stdcall;
  // Моргание светодиодом на передней панели.
  function L502_LedBlink(hnd: t_l502_hnd): LongInt; stdcall;
  // Установка подтягивающих резисторов на входных линиях.
  function L502_SetDigInPullup(hnd: t_l502_hnd; pullups : LongWord): LongInt; stdcall;









implementation
  function L502_Create() : t_l502_hnd; stdcall; external 'l502api.dll';
  function L502_Free(hnd: t_l502_hnd) : LongInt; stdcall; external 'l502api.dll';
  function _get_serials( ser_arr: p_x502_serial_array; size:LongWord;
                         flags:LongWord; out devcnt: LongWord) : LongInt;
                         stdcall; external 'l502api.dll' name 'L502_GetSerialList';
  function _get_dev_records_list(out list; size:LongWord;
                          flags : LongWord;  out devcnt: LongWord) : LongInt;
                          stdcall; external 'l502api.dll' name 'L502_GetDevRecordsList';

  function _open(hnd: t_l502_hnd; serial: PAnsiChar) : LongInt; stdcall; external 'l502api.dll' name 'L502_Open';
  function L502_Close(hnd : t_l502_hnd) : LongInt; stdcall; external 'l502api.dll';
  function L502_GetDevInfo(hnd : t_l502_hnd; out info : t_l502_info) : LongInt; stdcall; external 'l502api.dll';

  function L502_Configure(hnd: t_l502_hnd; flags: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetLChannel(hnd: t_l502_hnd; lch, phy_ch, mode, range, avg: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetLChannelCount(hnd: t_l502_hnd; lch_cnt : LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_GetLChannelCount(hnd: t_l502_hnd; out lch_cnt: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetAdcFreqDivider(hnd: t_l502_hnd; adc_freq_div : LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetAdcInterframeDelay(hnd: t_l502_hnd; delay : LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetDinFreqDivider(hnd: t_l502_hnd; din_freq_div: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetAdcFreq(hnd: t_l502_hnd; var f_acq, f_frame: Double): LongInt; stdcall; external 'l502api.dll';
  function L502_SetDinFreq(hnd: t_l502_hnd; var f_din: Double): LongInt; stdcall; external 'l502api.dll';
  function L502_GetAdcFreq(hnd: t_l502_hnd; out f_acq, f_frame: Double): LongInt; stdcall; external 'l502api.dll';
  function L502_SetRefFreq(hnd: t_l502_hnd; freq: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetSyncMode(hnd: t_l502_hnd; sync_mode: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetSyncStartMode(hnd: t_l502_hnd; sync_start_mode: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetMode(hnd: t_l502_hnd; mode: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_GetMode(hnd: t_l502_hnd; out mode: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetAdcCoef(hnd: t_l502_hnd; range: LongWord; k, offs: Double): LongInt; stdcall; external 'l502api.dll';
  function L502_GetAdcCoef(hnd: t_l502_hnd; range: LongWord; out k, offs: Double): LongInt; stdcall; external 'l502api.dll';

  function L502_AsyncOutDac(hnd: t_l502_hnd; ch: LongWord; data: Double; flags: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_AsyncOutDig(hnd: t_l502_hnd; val, msk: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_AsyncInDig(hnd: t_l502_hnd; out din: LongWord): LongInt; stdcall; external 'l502api.dll';
  function _get_adc_frame(hnd: t_l502_hnd; flags: LongWord; tout: LongWord; out data): LongInt; stdcall; external 'l502api.dll' name 'L502_AsyncGetAdcFrame';

  function L502_StreamsEnable(hnd: t_l502_hnd; streams: LongWord): LongInt; stdcall;  external 'l502api.dll';
  function L502_StreamsDisable(hnd: t_l502_hnd; streams: LongWord): LongInt; stdcall;  external 'l502api.dll';
  function L502_StreamsStart(hnd: t_l502_hnd): LongInt; stdcall;  external 'l502api.dll';
  function L502_IsRunning(hnd: t_l502_hnd): LongInt; stdcall; external 'l502api.dll';
  function L502_StreamsStop(hnd: t_l502_hnd): LongInt; stdcall;  external 'l502api.dll';
  function _recv(hnd: t_l502_hnd; out buf; size: LongWord; tout : LongWord): LongInt; stdcall;  external 'l502api.dll' name 'L502_Recv';
  function _send(hnd: t_l502_hnd; const buf; size: LongWord; tout : LongWord): LongInt; stdcall;  external 'l502api.dll' name 'L502_Send';
  function _process_adc_data(hnd: t_l502_hnd; const src; out dest; var size : LongWord;
                               flags : LongWord): LongInt; stdcall;  external 'l502api.dll' name 'L502_ProcessAdcData';
  function _process_data(hnd: t_l502_hnd; const src; size: LongWord;
                     flags : LongWord; out adc_data; var adc_data_size : LongWord;
                     out din_data; var din_data_size: LongWord):LongInt; stdcall;  external 'l502api.dll' name 'L502_ProcessData';

  function _process_data_usr(hnd: t_l502_hnd; const src; size: LongWord;
                     flags : LongWord; out adc_data; var adc_data_size : LongWord;
                     out din_data; var din_data_size: LongWord;
                     out usr_data; var usr_data_size: LongWord):LongInt; stdcall;  external 'l502api.dll' name 'L502_ProcessDataWithUserExt';
  function _prepare_data(hnd: t_l502_hnd; const dac1, dac2; const digout; size, flags : LongWord;
                            out out_buf):LongInt; stdcall;  external 'l502api.dll' name 'L502_PrepareData';
  function L502_GetRecvReadyCount(hnd: t_l502_hnd; out rdy_cnt: LongWord):LongInt; stdcall; external 'l502api.dll';
  function L502_GetSendReadyCount(hnd: t_l502_hnd; out rdy_cnt: LongWord):LongInt; stdcall; external 'l502api.dll';
  function L502_GetNextExpectedLchNum(hnd: t_l502_hnd; out lch: LongWord):LongInt; stdcall; external 'l502api.dll';
  function L502_PreloadStart(hnd: t_l502_hnd): LongInt; stdcall; external 'l502api.dll';
  function L502_OutCycleLoadStart(hnd: t_l502_hnd; size: LongWord):LongInt; stdcall; external 'l502api.dll';
  function L502_OutCycleSetup(hnd: t_l502_hnd; flags: LongWord):LongInt; stdcall; external 'l502api.dll';
  function L502_OutCycleStop(hnd: t_l502_hnd; flags: LongWord):LongInt; stdcall; external 'l502api.dll';
  function L502_SetDmaBufSize(hnd: t_l502_hnd;  dma_ch, size: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_SetDmaIrqStep(hnd: t_l502_hnd; dma_ch, step: LongWord): LongInt; stdcall; external 'l502api.dll';

  function L502_BfCheckFirmwareIsLoaded(hnd: t_l502_hnd; out version: LongWord): LongInt; stdcall; external 'l502api.dll';
  function _bf_load_firm(hnd: t_l502_hnd; filename: PAnsiChar): LongInt; stdcall; external 'l502api.dll' name 'L502_BfLoadFirmware';
  function _bf_mem_read(hnd: t_l502_hnd; addr : LongWord; out regs; size: LongWord): LongInt; stdcall; external 'l502api.dll' name 'L502_BfMemRead';
  function _bf_mem_write(hnd: t_l502_hnd; addr : LongWord; const regs; size: LongWord): LongInt; stdcall; external 'l502api.dll' name 'L502_BfMemWrite';
  function _bf_exec_cmd(hnd: t_l502_hnd; cmd_code : Word; par : LongWord;
                const snd_data; snd_size : LongWord; out rcv_data; rcv_size : LongWord;
                tout: LongWord; out recved_size: LongWord): LongInt; stdcall; external 'l502api.dll' name 'L502_BfExecCmd';

  function _flash_read(hnd: t_l502_hnd; addr: LongWord; out data; size: LongWord): LongInt; stdcall; external 'l502api.dll' name 'L502_FlashRead';
  function _flash_write(hnd: t_l502_hnd; addr: LongWord; const data; size: LongWord): LongInt; stdcall; external 'l502api.dll' name 'L502_FlashWrite';
  function L502_FlashErase(hnd: t_l502_hnd; addr: LongWord; size: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_FlashWriteEnable(hnd: LongWord): LongInt; stdcall; external 'l502api.dll';
  function L502_FlashWriteDisable(hnd: t_l502_hnd): LongInt; stdcall; external 'l502api.dll';

  function L502_GetDllVersion() : LongWord; stdcall; external 'l502api.dll';
  function L502_GetDriverVersion(hnd: t_l502_hnd; out ver: LongWord): LongInt; stdcall; external 'l502api.dll';
  function _get_err_str(err : LongInt) : PAnsiChar; stdcall; external 'l502api.dll' name 'L502_GetErrorString';
  function L502_LedBlink(hnd: t_l502_hnd): LongInt; stdcall;  external 'l502api.dll';
  function L502_SetDigInPullup(hnd: t_l502_hnd; pullups : LongWord): LongInt; stdcall;  external 'l502api.dll';


  {
  function L502_GetSerialList(out serials: t_l502_serial_list; flags: LongWord) : LongInt; overload;
  var
    ser_arr : p_l502_serial_array;
    devcnt: LongWord;
    res, i : LongInt;
  begin
    // получаем количество устройтв
    res := _get_serials(nil, 0, flags, devcnt);
    if (res >= 0) and (devcnt>0) then
    begin
      // выделяем память под devcnt серийных номеров
      ser_arr:=GetMemory(devcnt*L502_SERIAL_SIZE);
      //получаем их список
      res:= _get_serials(ser_arr, devcnt, flags, PLongWord(nil)^);
      if (res > 0) then
      begin
        //устанавливаем размер динамического массива
        SetLength(serials, res);
        //сохраняем полученные номера в динамический массив строк
        for i:=0 to res-1 do
          serials[i] := string(ser_arr[i]);
      end;
      //освобождаем память, выделенную под временный массив
      FreeMemory(ser_arr);
    end;
    L502_GetSerialList:= res;
  end;
  }

  function L502_GetSerialList(out serials: array of string; flags: LongWord; out devcnt: LongWord) : LongInt; overload;
  var
    ser_arr : p_x502_serial_array;
    res, i : LongInt;
  begin
    if (Length(serials) > 0) then
    begin
      ser_arr:=GetMemory(Length(serials)*X502_SERIAL_SIZE);
      // получаем количество устройтв
      res := _get_serials(ser_arr, Length(serials), flags, devcnt);
      if res >= 0 then
      begin
        //сохраняем полученные номера в динамический массив строк
        for i:=0 to res-1 do
          serials[i] := string(ser_arr[i]);
      end;
      //освобождаем память, выделенную под временный массив
      FreeMemory(ser_arr);
    end
    else
    begin
      res:= _get_serials(nil, 0, flags, devcnt);
    end;
    L502_GetSerialList:=res;
  end;

  function L502_GetSerialList(out serials: array of string; flags: LongWord) : LongInt; overload;
  begin
    L502_GetSerialList:= L502_GetSerialList(serials, flags, PCardinal(nil)^);
  end;

  function L502_GetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord;  out devcnt: LongWord) : LongInt; overload;
  begin
    if (Length(list) > 0) then
    begin
      L502_GetDevRecordsList := _get_dev_records_list(list, Length(list), flags, devcnt);
    end
    else
    begin
      L502_GetDevRecordsList:= _get_dev_records_list(PCardinal(nil)^, 0, flags, devcnt);
    end;
  end;
  function L502_GetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord) : LongInt; overload;
  begin
    L502_GetDevRecordsList:= L502_GetDevRecordsList(list, flags, PCardinal(nil)^);
  end;

  function L502_Open(hnd: t_l502_hnd; serial: string) : LongInt;
  begin
    L502_Open:=_open(hnd, PAnsiChar(AnsiString(serial)));
  end;

  function L502_GetErrorString(err: LongInt) : string;
  begin
    L502_GetErrorString:= string(_get_err_str(err));
  end;

  function L502_AsyncGetAdcFrame(hnd: t_l502_hnd; flags: LongWord; tout: LongWord; out data: array of Double): LongInt; stdcall;
  var err: LongInt;
      lch_cnt: LongWord;
  begin
    err:= L502_GetLChannelCount(hnd, lch_cnt);
    if err=L502_ERR_OK then
    begin
      if LongWord(Length(data)) < lch_cnt then
        err:=L502_ERR_INSUFFICIENT_ARRAY_SIZE
      else
        err:=_get_adc_frame(hnd,flags,tout,data);
    end;
    L502_AsyncGetAdcFrame:=err;
  end;

  function L502_Recv(hnd: t_l502_hnd; out buf : array of LongWord; size: LongWord; tout : LongWord): LongInt; stdcall;
  begin
    if LongWord(Length(buf)) < size then
      L502_Recv:=L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_Recv:=_recv(hnd, buf, size, tout);
  end;

  function L502_Send(hnd: t_l502_hnd; const buf : array of LongWord; size: LongWord; tout: LongWord): LongInt; stdcall;
  begin
    if LongWord(Length(buf)) < size then
      L502_Send:=L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_Send:=_send(hnd,buf,size,tout);
  end;


 function L502_ProcessAdcData(hnd: t_l502_hnd; const src: array of LongWord;
                               out dest: array of Double; var size : LongWord;
                               flags : LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(src)) < size) or (LongWord(Length(dest)) < size) then
      L502_ProcessAdcData:=L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_ProcessAdcData:=_process_adc_data(hnd, src, dest, size, flags);
  end;

  function L502_ProcessData(hnd: t_l502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord):LongInt; stdcall;
  begin
    if (LongWord(Length(adc_data)) < adc_data_size) or (LongWord(Length(din_data)) < din_data_size)
            or (LongWord(Length(src)) < size) then
      L502_ProcessData:=L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_ProcessData:=_process_data(hnd, src, size, flags, adc_data, adc_data_size, din_data, din_data_size);
  end;

  function L502_ProcessDataWithUserExt(hnd: t_l502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord;
                     out usr_data: array of LongWord; var usr_data_size: LongWord):LongInt; stdcall;
  begin
    if (LongWord(Length(adc_data)) < adc_data_size) or (LongWord(Length(din_data)) < din_data_size)
            or (LongWord(Length(src)) < size) or (LongWord(Length(usr_data)) < usr_data_size) then
      L502_ProcessDataWithUserExt:=L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_ProcessDataWithUserExt:=_process_data_usr(hnd, src,size,flags,adc_data,
                                adc_data_size, din_data, din_data_size, usr_data, usr_data_size);
  end;

  function L502_PrepareData(hnd: t_l502_hnd; const dac1, dac2: array of Double;
                            const digout: array of LongWord; size, flags : LongWord;
                            out out_buf: array of LongWord):LongInt; stdcall;
  begin
    L502_PrepareData:=_prepare_data(hnd, dac1, dac2, digout, size, flags, out_buf);
  end;

  function L502_BfLoadFirmware(hnd: t_l502_hnd; filename: string): LongInt; stdcall;
  begin
    L502_BfLoadFirmware:=_bf_load_firm(hnd, PAnsiChar(AnsiString(filename)));
  end;

  function L502_BfMemRead(hnd: t_l502_hnd; addr : LongWord; out regs: array of LongWord;
                                size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(regs)) < size) then
      L502_BfMemRead := L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_BfMemRead:=_bf_mem_read(hnd, addr, regs, size);
  end;

  function L502_BfMemWrite(hnd: t_l502_hnd; addr : LongWord;
                           const regs: array of LongWord; size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(regs)) < size) then
      L502_BfMemWrite := L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_BfMemWrite:=_bf_mem_write(hnd, addr, regs, size);
  end;

  function L502_BfExecCmd(hnd: t_l502_hnd; cmd_code : Word; par : LongWord;
                const snd_data : array of LongWord; snd_size : LongWord;
                out rcv_data : array of LongWord; rcv_size : LongWord;
                tout: LongWord; out recvd_size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(snd_data)) < snd_size) or
       (LongWord(Length(rcv_data)) < rcv_size)  then
      L502_BfExecCmd := L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_BfExecCmd:=_bf_exec_cmd(hnd, cmd_code, par, snd_data, snd_size,
                rcv_data, rcv_size, tout, recvd_size);
  end;

  function L502_FlashRead(hnd: t_l502_hnd; addr: LongWord;
                          out data: array of Byte; size: LongWord): LongInt; stdcall;
  begin
     if (LongWord(Length(data)) < size) then
      L502_FlashRead := L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_FlashRead:=_bf_mem_write(hnd, addr, data, size);
  end;
  ///Запись блока данных во Flash-память модуля.
  function L502_FlashWrite(hnd: t_l502_hnd; addr: LongWord;
                           const data: array of Byte; size: LongWord): LongInt; stdcall;
  begin
     if (LongWord(Length(data)) < size) then
      L502_FlashWrite := L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_FlashWrite:=_bf_mem_write(hnd, addr, data, size);
  end;
end.
 
