unit l502api;
interface
uses Windows, SysUtils, x502api;



  //��������� ������ �������� �������
  function L502_GetSerialList(out serials: array of string; flags: LongWord; out devcnt: LongWord) : LongInt; overload;
  function L502_GetSerialList(out serials: array of string; flags: LongWord) : LongInt; overload;
  //���������� ����� � ������� �� ��������� ������
  function L502_Open(hnd: t_x502_hnd; serial: string): LongInt; stdcall;

  function L502_GetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord;  out devcnt: LongWord) : LongInt; overload;
  function L502_GetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord) : LongInt; overload;



{ ----------- ���� � �������, ����������� ��� �������� ������������� ----------}
const
  // ������������ ���������� ���������� ������� � �������
  L502_LTABLE_MAX_CH_CNT      = 256;
  // ���������� ���������� ��� ��������� ����������
  L502_ADC_RANGE_CNT          = 6;
  // ������������ �������� ��� ����������� ���������� �� ����������� ������
  L502_LCH_AVG_SIZE_MAX       = 128;
  //������������ �������� �������� ������� ���
  L502_ADC_FREQ_DIV_MAX       = (1024*1024);
  // ������������ �������� �������� ������� ����������� ��������� �����
  L502_DIN_FREQ_DIV_MAX       = (1024*1024);
  // ������������ �������� ����������� �������� ��� ���
  L502_ADC_INTERFRAME_DELAY_MAX  = $1FFFFF;
  // ������� �� ��������� ��� ���������� ������� � BlackFin
  L502_BF_CMD_DEFAULT_TOUT     = 500;

  // ��� ���, ��������������� ������������� �������� �����
  L502_ADC_SCALE_CODE_MAX      = 6000000;
  // ��� ���, ��������������� ������������� �������� �����
  L502_DAC_SCALE_CODE_MAX      = 30000;

  // ������������ ���������� �������� � ������ � ��������� ����������
  L502_DEVNAME_SIZE            = X502_DEVNAME_SIZE;
  // ������������ ���������� �������� � ������ � �������� �������
  L502_SERIAL_SIZE             = X502_SERIAL_SIZE;

  //������ ���������������� ������� Flash-������
  L502_FLASH_USER_SIZE         = $100000;
  //����������� ������� �� ���������� ������� � BlackFin � ��
  L502_BF_REQ_TOUT             = 500;
  // �������� ��� � �������
  L502_DAC_RANGE               = 5;
  // ���������� ������� ���
  L502_DAC_CH_CNT              = 2;

  // ����� � ������, ����������, ��� ��������� ������������
  L502_STREAM_IN_MSG_OVERFLOW  = $01010000;

  // ����������� ��������� �������� ������� ������� ������� 
  L502_EXT_REF_FREQ_MAX        = 2000000;


  { ----------------- ���� ������ ���������� ---------------------------}
    // ������� ��������� ��� ������
    L502_ERR_OK                           = 0;
    // � ������� ������� ���������������� ��������� ������
    L502_ERR_INVALID_HANDLE               = -1;
    // ������ ��������� ������
    L502_ERR_MEMORY_ALLOC                 = -2;
    // ������� ������� ��� �������� ����������
    L502_ERR_ALREADY_OPENED               = -3;
    // ���������� � ��������� ����������� �� ������� � �������
    L502_ERR_DEVICE_NOT_FOUND             = -4;
    // ������ � ���������� �������� (��� ������� ��-�� ����, ��� ���������� ��� ������� � ������ ���������)
    L502_ERR_DEVICE_ACCESS_DENIED         = -5;
    // ������ �������� ���������� 
    L502_ERR_DEVICE_OPEN                  = -6;
    // � ������� ������� ���������������� ��������� 
    L502_ERR_INVALID_POINTER              = -7;
    // ������� �� ����� ���� ��������� ��� ���������� ������ ����� ������ 
    L502_ERR_STREAM_IS_RUNNING            = -8;
    // ������ ������ ������ ����������� ����� 
    L502_ERR_RECV                         = -9;
    // ������ ������ ������ ��� ����������� ������ 
    L502_ERR_SEND                         = -10;
    // ��������� ������������ ����������� ������ ��� ������ ����������� ����� 
    L502_ERR_STREAM_OVERFLOW              = -11;
    // ����������� ��������� � ������ ����������� ����� 
    L502_ERR_UNSUP_STREAM_MSG             = -12;
    // ������ �������� ���������� �������� 
    L502_ERR_MUTEX_CREATE                 = -13;
    // �������� ��������� �������� 
    L502_ERR_MUTEX_INVALID_HANDLE         = -14;
    // ������� ����� �������� ������������ �������� 
    L502_ERR_MUTEX_LOCK_TOUT              = -15;
    // ������ ������������ �������� 
    L502_ERR_MUTEX_RELEASE                = -16;
    // ������������ ��������� �������� 
    L502_ERR_INSUFFICIENT_SYSTEM_RESOURCES= -17;
    // ������ ����������� ��� �� ����������� 
    L502_ERR_NOT_IMPLEMENTED              = -18;
    // ������������� ������ �������
    L502_ERR_INSUFFICIENT_ARRAY_SIZE      = -19;
    // ������ ������ �������� FPGA
    L502_ERR_FPGA_REG_READ                = -20;
    // ������ ������ �������� FPGA
    L502_ERR_FPGA_REG_WRITE               = -21;
    // ���� ������ ��� ����������
    L502_ERR_STREAM_IS_NOT_RUNNING        = -22;
    // ����� �������� ������ ���������� ������� 
    L502_ERR_INVALID_LTABLE_SIZE          = -102;
    // ����� �������� ����� ����������� ������ 
    L502_ERR_INVALID_LCH_NUMBER           = -103;
    // ������� ������ �������� ��������� ��� 
    L502_ERR_INVALID_LCH_RANGE            = -104;
    // ������� ����� ����� ��������� ��� ����������� ������ 
    L502_ERR_INVALID_LCH_MODE             = -105;
    // ������� ����� ����� ����������� ������ ��� ��������� ����������� 
    L502_ERR_INVALID_LCH_PHY_NUMBER       = -106;
    // ������� ����� ������ ���������� ��� ����������� ������ 
    L502_ERR_INVALID_LCH_AVG_SIZE         = -107;
    // ������� ����� �������� ������� ����� ������ ��� 
    L502_ERR_INVALID_ADC_FREQ_DIV         = -108;
    // ������� ����� �������� ������� ����������� ����� �������� ����� 
    L502_ERR_INVALID_DIN_FREQ_DIV         = -108;
    // ������� ����� ����� ������ ������ L502 
    L502_ERR_INVALID_MODE                 = -109;
    // �������� ����� ������ ��� 
    L502_ERR_INVALID_DAC_CHANNEL          = -110;
    // �������� ��� ������ ������� ������� ������������� 
    L502_ERR_INVALID_REF_FREQ             = -111;
    // ������� ������ �������� ����������� �������� 
    L502_ERR_INVALID_INTERFRAME_DELAY     = -112;
    // ������� ����� ����� ������������� 
    L502_ERR_INVALID_SYNC_MODE            = -113;
    // ������� ����� ����� ������ DMA 
    L502_ERR_INVALID_DMA_CH               = -114;
    // ������ ������� ������� ������� �������������
    L502_ERR_REF_FREQ_NOT_LOCKED          = -131;
    // ����������� ������ � �������� �������� � ������� 
    L502_ERR_IOCTL_FAILD                  = -132;
    // ����� ������� �������� ���������� ���������� ������������ ������� � �������� 
    L502_ERR_IOCTL_TIMEOUT                = -133;
    // ������ ��������� ���������� � ���������� �� �������� 
    L502_ERR_GET_INFO                     = -134;
    // �� ����� �������� �� ���� ������� ����� ����� � �������� ����� 
    L502_ERR_DIG_IN_NOT_RDY               = -135;
    // ������� ������������ ���� �� ������ 
    L502_ERR_RECV_INSUFFICIENT_WORDS      = -136;
    // ������� ��������� ��������, ��������� ������� ���, ��� ��� ���������� 
    L502_ERR_DAC_NOT_PRESENT              = -137;
    // �������� ����� ������ � �������������� ������ ����������� ����� 
    L502_ERR_PROC_INVALID_CH_NUM          = -140;
    // �������� ��� ��������� � �������������� ������ ����������� ����� 
    L502_ERR_PROC_INVALID_CH_RANGE        = -141;
    // ����� �������� ����� �� Flash-������ 
    L502_ERR_FLASH_INVALID_ADDR           = -142;
    // ����� �������� ������ ����� ������ ��� ������ � Flash-������� 
    L502_ERR_FLASH_INVALID_SIZE           = -143;
    // ����� ������� �������� ���������� ������ �� Flash-������ 
    L502_ERR_FLASH_WRITE_TOUT             = -144;
    // ����� ������� �������� ���������� �������� ����� Flash-������ 
    L502_ERR_FLASH_ERASE_TOUT             = -145;
    // �������� ������� ��� �������� Flash-������ �������� ������� ����� � 4 ����� 
    L502_ERR_FLASH_SECTOR_BOUNDARY        = -146;
    // �� ������� ������� ���� �������� BlackFin
    L502_ERR_LDR_FILE_OPEN                = -180;
    // ������ ������ �� ���� �������� BlackFin 
    L502_ERR_LDR_FILE_READ                = -181;
    // �������� ������ ����� �������� BlackFin 
    L502_ERR_LDR_FILE_FORMAT              = -182;
    // ������������ ����������� LDR-�����, ����������� ��� ������ �������� BlackFin �� HDMA 
    L502_ERR_LDR_FILE_UNSUP_FEATURE       = -183;
    // �������� ��������� ����� ��������� � �������� BlackFin 
    L502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR  = -184;
    // ����� ������� ���������� ������� �� ������/������ ������ BlackFin 
    L502_ERR_BF_REQ_TIMEOUT               = -185;
    // ������� ��� BlackFin ��� ��� ��������� � �������� ��������� 
    L502_ERR_BF_CMD_IN_PROGRESS           = -186;
    // ������� ����� ���������� ����������� ������� ����������� BlackFin 
    L502_ERR_BF_CMD_TIMEOUT               = -187;
    // ���������� ������������ ������ � ����� �� ������� � BlackFin
    L502_ERR_BF_CMD_RETURN_INSUF_DATA     = -188;
    // ����� ������� �������� ���������� ���������� BlackFin � ������ �������� 
    L502_ERR_BF_LOAD_RDY_TOUT             = -189;
    // ������� ��������� �������� ��� ������� ����� ���������� ��������� ���
    // ���������� ����������� ���������� � ������
    L502_ERR_BF_NOT_PRESENT               = -190;
    // �������� ����� ������ BlackFin ��� ������ ��� ������ �� HDMA 
    L502_ERR_BF_INVALID_ADDR              = -191;
    // �������� ������ ������, ������������ � ����������� �������� � BlackFin
    L502_ERR_BF_INVALID_CMD_DATA_SIZE     = -192;


    {-------- �����, ����������� ������� �������������� ������� ------}
     // �������, ��� ����� ������� �������� ������ ������ ��� ���������,
     //   ������� ��� �� �������
    L502_GETDEVS_FLAGS_ONLY_NOT_OPENED = X502_GETDEVS_FLAGS_ONLY_NOT_OPENED;



    {-------- ����� ��� ���������� ��������� ��������. ---------------}
    //���������� (������� � ������ ���������) ������� �������� �������� �������
    L502_DIGOUT_WORD_DIS_H = $00020000;
    //���������� ������� �������� �������� �������
    L502_DIGOUT_WORD_DIS_L = $00010000;


    {-------- ��������� ��� ������ ������� ������� -------------------}
    L502_REF_FREQ_2000KHZ  = 2000000; // ������� 2���
    L502_REF_FREQ_1500KHZ  = 1500000; // ������� 1.5���

    {-------- ��������� ��������� ��� ������ ��� ----------------------}
    L502_ADC_RANGE_10 = 0; // �������� +/-10V
    L502_ADC_RANGE_5  = 1; // �������� +/-5V
    L502_ADC_RANGE_2  = 2; // �������� +/-2V
    L502_ADC_RANGE_1  = 3; // �������� +/-1V
    L502_ADC_RANGE_05 = 4; // �������� +/-0.5V
    L502_ADC_RANGE_02 = 5; // �������� +/-0.2V


    {-------- ����� ��������� ��� ����������� ������ ------------------}
    L502_LCH_MODE_COMM = 0; // ��������� ���������� ������������ ����� �����
    L502_LCH_MODE_DIFF = 1; // ���������������� ��������� ����������
    L502_LCH_MODE_ZERO = 2; // ��������� ������������ ����


    {-------- ������ ������������� ------------------------------------}
    L502_SYNC_INTERNAL        = 0; // ���������� ������
    L502_SYNC_EXTERNAL_MASTER = 1; // �� �������� ������� �� ������� �������������
    L502_SYNC_DI_SYN1_RISE    = 2; // �� ������ ������� DI_SYN1
    L502_SYNC_DI_SYN2_RISE    = 3; // �� ������ ������� DI_SYN2
    L502_SYNC_DI_SYN1_FALL    = 6; // �� ����� ������� DI_SYN1
    L502_SYNC_DI_SYN2_FALL    = 7;  // �� ����� ������� DI_SYN2

    {-------- �����, ����������� ���������� �������� ������ -------------------}
    L502_PROC_FLAGS_VOLT       = 1; // �������, ��� ����� ������������� ��������
                                   // ��� � ������

    {-------- ����� ��� ����������� ���������� ������� ������ -----------------}
    L502_STREAM_ADC  = $01; //����� ������ �� ���
    L502_STREAM_DIN  = $02; // ����� ������ � �������� ������
    L502_STREAM_DAC1 = $10; // ����� ������ ������� ������ ���
    L502_STREAM_DAC2 = $20; // ����� ������ ������� ������ ���
    L502_STREAM_DOUT = $40; // ����� ������ �� �������� ������
    // ����������� ���� ������, ������������ ������ ������ �� ����
    L502_STREAM_ALL_IN = L502_STREAM_ADC or L502_STREAM_DIN;
    // ����������� ���� ������, ������������ ������ ������ �� �����
    L502_STREAM_ALL_OUT = L502_STREAM_DAC1 or L502_STREAM_DAC2 or L502_STREAM_DOUT;


    {--- ���������, ������������ ��� ������������� ������� �� �� � ������ -----}
    L502_STREAM_OUT_WORD_TYPE_DOUT   = $0; // �������� �����
    L502_STREAM_OUT_WORD_TYPE_DAC1   = $40000000; // ��� ��� 1-�� ������ ���
    L502_STREAM_OUT_WORD_TYPE_DAC2   = $80000000; // ��� ��� 2-�� ������ ���

    {------------------ ����� ������ ������ L502 ------------------------------}
    L502_MODE_FPGA  = 0; // ��� ������ ������ ���������� ����� ���� �����
                         //   ���������� ��������� BlackFin
    L502_MODE_DSP   = 1; // ��� ������ ������ ���������� ����� ����������
                         //   ���������, ������� ������ ���� ��������
                         //   ��������� ��� ��������� ���� �������
    L502_MODE_DEBUG = 2; // ���������� �����

    {------------------- ������ ������� ��� -----------------------------------}
    L502_DAC_CH1 = 0;  //������ ����� ���
    L502_DAC_CH2 = 1;  //������ ����� ���

    {----------- �����, ������������ ��� ������ ������ �� ��� -----------------}
    // ���������, ��� �������� ������ � ������� � ��� ������ ��� �����
    // ��������� � ���� ���. ���� ���� �� ������, �� ���������, ��� ��������
    // ���������� � �����
    L502_DAC_FLAGS_VOLT    = $0001;
    // ���������, ��� ����� ��������� ������������� ������������ �����
    //    ������� �������� �� ���.
    L502_DAC_FLAGS_CALIBR  = $0002;

    {------------------ ������ ������� DMA ------------------------------------}
    L502_DMA_CH_IN  = 0; // ����� ����� DMA �� ����
    L502_DMA_CH_OUT = 1; // ����� ����� DMA �� �����

    {--- �������� �����, �� ������� ����� �������� ������������� ��������� ----}
    L502_PULLUPS_DI_H    = $01; // ������� �������� �������� ������
    L502_PULLUPS_DI_L    = $02; // ������� �������� �������� ������
    L502_PULLUPS_DI_SYN1 = $04; // ����� SYN1
    L502_PULLUPS_DI_SYN2 = $08; // ����� SYN2



    {--------------- �����, ������������ ������� ����� � ������ ---------------}
    // ������� ������� �������������� ���������� ���
    L502_DEVFLAGS_DAC_PRESENT         = X502_DEVFLAGS_DAC_PRESENT;
    // ������� ������� ����������� ���������� BlackFin
    L502_DEVFLAGS_BF_PRESENT          = X502_DEVFLAGS_BF_PRESENT;
    // ������� ������� ����������������
    L502_DEVFLAGS_GAL_PRESENT         = X502_DEVFLAGS_GAL_PRESENT;
    // �������, ��� �� Flash-������ ������������ ���������� � ������
    L502_DEVFLAGS_FLASH_DATA_VALID    = $00010000;
    // �������, ��� �� Flash-������ ������������ �������������� �������������
    //   ������������ ���
    L502_DEVFLAGS_FLASH_ADC_CALIBR_VALID = $00020000;
    // �������, ��� �� Flash-������ ������������ �������������� �������������
    //    ������������ ���
    L502_DEVFLAGS_FLASH_DAC_CALIBR_VALID = $00040000;

    {----------------����� ��� ������ ������������ ������----------------------}
    // ���� ���������, ��� ������� ��� ����� ������� ����� ��������� ��� ��������
    //    ����� ����� ����������� �������.
    L502_OUT_CYCLE_FLAGS_FORCE = $01;


type
  // ��������� ������ - ��������� �� ������������ ���������
  t_l502_hnd = t_x502_hnd;


  { ������������� ������������ ��������� }
  t_l502_cbr_coef = record
    offs: Double;  // �������� ����
    k : Double; // ����������� �����
  end;


  { ������������� ������������ ������. }
  t_l502_cbr = record
    //������������� ������������ ���
    adc: array[0..L502_ADC_RANGE_CNT-1] of t_l502_cbr_coef;
    res1: array[0..63] of LongWord;
    // ������������� ������������ ���
    dac: array[0..L502_DAC_CH_CNT-1] of t_l502_cbr_coef;
    res2: array[0..19] of LongWord;
  end;

  { ���������� � ������ L502.}
  t_l502_info = record
    name: array[0..L502_DEVNAME_SIZE-1] of AnsiChar; // �������� ���������� ("L502")
    serial: array[0..L502_SERIAL_SIZE-1] of AnsiChar; // �������� �����
    devflags: LongWord; // ����� ����������� ������� � ������ ������������ �����
    fpga_ver : Word; // ������ ���� (������� ���� - ��������, ������� - ��������)
    plda_ver : Byte;  // ������ ����, ������������ ���������� ������
    res : array[0..120] of Byte; //������
    cbr : t_l502_cbr; // ��������� ������������� ������������ (�� Flash-������)
  end;

  function L502_Create(): t_l502_hnd; stdcall;
  function L502_Free(hnd: t_l502_hnd): LongInt; stdcall;

  //��������� ����� � �������
  function L502_Close(hnd: t_l502_hnd): LongInt; stdcall;
  //��������� ���������� � ������
  function L502_GetDevInfo(hnd: t_l502_hnd; out info: t_l502_info) : LongInt; stdcall;

  // �������� ������������� �������� � ������
  function L502_Configure(hnd: t_l502_hnd; flags: LongWord): LongInt; stdcall;
  // ��������� ���������� ����������� ������
  function L502_SetLChannel(hnd: t_l502_hnd; lch, phy_ch, mode, range, avg: LongWord): LongInt; stdcall;
  // ��������� ���������� ���������� �������
  function L502_SetLChannelCount(hnd: t_l502_hnd; lch_cnt : LongWord): LongInt; stdcall;
  // ��������� ���������� ���������� �������
  function L502_GetLChannelCount(hnd: t_l502_hnd; out lch_cnt: LongWord): LongInt; stdcall;
  // ��������� �������� ������� ����� ��� ���
  function L502_SetAdcFreqDivider(hnd: t_l502_hnd; adc_freq_div : LongWord): LongInt; stdcall;
  // ��������� �������� ����������� �������� ��� ���
  function L502_SetAdcInterframeDelay(hnd: t_l502_hnd; delay : LongWord): LongInt; stdcall;
  // ��������� �������� ������� ����������� ����� � �������� �����
  function L502_SetDinFreqDivider(hnd: t_l502_hnd; din_freq_div: LongWord): LongInt; stdcall;
  // ��������� ������� ����� ���
  function L502_SetAdcFreq(hnd: t_l502_hnd; var f_acq, f_frame: Double): LongInt; stdcall;
  // ��������� ������� ����������� ����� � �������� ������
  function L502_SetDinFreq(hnd: t_l502_hnd; var f_din: Double): LongInt; stdcall;
  // �������� ������� �������� ������ ����� ���
  function L502_GetAdcFreq(hnd: t_l502_hnd; out f_acq, f_frame: Double): LongInt; stdcall;
  // ��������� �������� ������� ������� �������������.
  function L502_SetRefFreq(hnd: t_l502_hnd; freq: LongWord): LongInt; stdcall;
  // ��������� ������ ��������� ������� �������������.
  function L502_SetSyncMode(hnd: t_l502_hnd; sync_mode: LongWord): LongInt; stdcall;
  // ��������� ������ ������� ������� �������������.
  function L502_SetSyncStartMode(hnd: t_l502_hnd; sync_start_mode: LongWord): LongInt; stdcall;
  // ���������� ����� ������ ������
  function L502_SetMode(hnd: t_l502_hnd; mode: LongWord): LongInt; stdcall;
  // ��������� �������� ������ ������ ������.
  function L502_GetMode(hnd: t_l502_hnd; out mode: LongWord): LongInt; stdcall;
  // ���������� ������������ ��� ���������� �������� ���.
  function L502_SetAdcCoef(hnd: t_l502_hnd; range: LongWord; k, offs: Double): LongInt; stdcall;
  // ��������� ������� ������������� ������������� ���.
  function L502_GetAdcCoef(hnd: t_l502_hnd; range: LongWord; out k, offs: Double): LongInt; stdcall;


  {----------------------- ������� ������������ �����-������ ------------------}
  // ����������� ����� ������ �� ����� ���.
  function L502_AsyncOutDac(hnd: t_l502_hnd; ch: LongWord; data: Double; flags: LongWord): LongInt; stdcall;
  // ����������� ����� ������ �� �������� ������.
  function L502_AsyncOutDig(hnd: t_l502_hnd; val, msk: LongWord): LongInt; stdcall;
  // ����������� ���� �������� � �������� ������.
  function L502_AsyncInDig(hnd: t_l502_hnd; out din: LongWord): LongInt; stdcall;
  // ����������� ���� ������ ����� ���.
  function L502_AsyncGetAdcFrame(hnd: t_l502_hnd; flags: LongWord; tout: LongWord; out data: array of Double): LongInt; stdcall;


  {-------------- ������� ��� ������ � ���������� ��������� ������-������� ----}
  // ���������� ���������� ������� �� ����/�����.
  function L502_StreamsEnable(hnd: t_l502_hnd; streams: LongWord): LongInt; stdcall;
  // ���������� ���������� ������� �� ����/�����.
  function L502_StreamsDisable(hnd: t_l502_hnd; streams: LongWord): LongInt; stdcall;
  // ������ ���������� ������� �����/������.
  function L502_StreamsStart(hnd: t_l502_hnd): LongInt; stdcall;
  // ������� ���������� ������� �����/������.
  function L502_StreamsStop(hnd: t_l502_hnd): LongInt; stdcall;
  // ��������, ������� �� ����� ������
  function L502_IsRunning(hnd: t_l502_hnd): LongInt; stdcall;


  // ������ ������ ��� � �������� ������ �� ������.
  function L502_Recv(hnd: t_l502_hnd; out buf : array of LongWord; size: LongWord; tout : LongWord): LongInt; stdcall;
  // �������� ��������� ������ ��� � �������� ������� � ������.
  function L502_Send(hnd: t_l502_hnd; const buf : array of LongWord; size: LongWord; tout: LongWord): LongInt; stdcall;
  // ��������� �������� �������� ��� �� ������.
  function L502_ProcessAdcData(hnd: t_l502_hnd; const src: array of LongWord;
                               out dest: array of Double; var size : LongWord;
                               flags : LongWord): LongInt; stdcall;
  // ��������� �������� �� ������ ������.
 function L502_ProcessData(hnd: t_l502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord):LongInt; stdcall;
  // ��������� �������� �� ������ ������ �������������� �������.
  function L502_ProcessDataWithUserExt(hnd: t_l502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord;
                     out usr_data: array of LongWord; var usr_data_size: LongWord):LongInt; stdcall;
  // ���������� ������ ��� ������ � ������.
  function L502_PrepareData(hnd: t_l502_hnd; const dac1, dac2: array of Double;
                            const digout: array of LongWord; size, flags : LongWord;
                            out out_buf: array of LongWord):LongInt; stdcall;

  // �������� ���������� �������� � ������ ������ �� ����.
  function L502_GetRecvReadyCount(hnd: t_l502_hnd; out rdy_cnt: LongWord):LongInt; stdcall;
  // �������� ������ ���������� ����� � ������ ������ �� �����.
  function L502_GetSendReadyCount(hnd: t_l502_hnd; out rdy_cnt: LongWord):LongInt; stdcall;
  // �������� ����� ���������� ���������� ����������� ������ ��� ��� ���������.
  function L502_GetNextExpectedLchNum(hnd: t_l502_hnd; out lch: LongWord):LongInt; stdcall;
  // ������ ���������� ������ ���������� ������
  function L502_PreloadStart(hnd: t_l502_hnd): LongInt; stdcall;

  //��������� ����� ������������ ������������ ������� �� �����
  function L502_OutCycleLoadStart(hnd: t_l502_hnd; size: LongWord):LongInt; stdcall;
  //��������� ����� ������������ ������������ ������� �� �����
  function L502_OutCycleSetup(hnd: t_l502_hnd; flags: LongWord):LongInt; stdcall;
  // ������� ������ ������������ �������
  function L502_OutCycleStop(hnd: t_l502_hnd; flags: LongWord):LongInt; stdcall;

  // ��������� ������� ������ � �������� ��� ����������� ����� ��� ������.
  function L502_SetDmaBufSize(hnd: t_l502_hnd;  dma_ch, size: LongWord): LongInt; stdcall;
  // ��������� ���� ���������� ��� �������� ������ �� DMA.
  function L502_SetDmaIrqStep(hnd: t_l502_hnd; dma_ch, step: LongWord): LongInt; stdcall;
  
  {------------ ������� ��� ������ � ���������� ����������� -------------------}
  function L502_BfCheckFirmwareIsLoaded(hnd: t_l502_hnd; out version: LongWord): LongInt; stdcall;
  /// �������� �������� ����������� ���������� BlackFin.
  function L502_BfLoadFirmware(hnd: t_l502_hnd; filename: string): LongInt; stdcall;
  /// ������ ����� ������ �� ������ ����������� ����������.
  function L502_BfMemRead(hnd: t_l502_hnd; addr : LongWord; out regs: array of LongWord;
                                size: LongWord): LongInt; stdcall;
  /// ������ ����� ������ � ������ ����������� ����������.
  function L502_BfMemWrite(hnd: t_l502_hnd; addr : LongWord;
                           const regs: array of LongWord; size: LongWord): LongInt; stdcall;
  /// �������� ����������� ������� ����������� ����������.
  function L502_BfExecCmd(hnd: t_l502_hnd; cmd_code : Word; par : LongWord;
                const snd_data : array of LongWord; snd_size : LongWord;
                out rcv_data : array of LongWord; rcv_size : LongWord;
                tout: LongWord; out recvd_size: LongWord): LongInt; stdcall;

  {-------------------  ������� ��� ������ � Flash-������� ������ -------------}
  /// ������ ����� ������ �� Flash-������ ������.
  function L502_FlashRead(hnd: t_l502_hnd; addr: LongWord;
                          out data: array of Byte; size: LongWord): LongInt; stdcall;
  ///������ ����� ������ �� Flash-������ ������.
  function L502_FlashWrite(hnd: t_l502_hnd; addr: LongWord;
                           const data: array of Byte; size: LongWord): LongInt; stdcall;
  /// �������� ����� �� Flash-������.
  function L502_FlashErase(hnd: t_l502_hnd; addr: LongWord; size: LongWord): LongInt; stdcall;
  /// ���������� ������ � ���������������� ������� Flash-������.
  function L502_FlashWriteEnable(hnd: LongWord): LongInt; stdcall;
  /// ������ ������ � ���������������� ������� Flash-������.
  function L502_FlashWriteDisable(hnd: t_l502_hnd): LongInt; stdcall;

  {-----------------  �������������� ��������������� ������� ------------------}
  //�������� ������ ����������.
  function L502_GetDllVersion() : LongWord; stdcall;
  // �������� ������ �������� ����������
  function L502_GetDriverVersion(hnd: t_l502_hnd; out ver: LongWord): LongInt; stdcall;
  // ��������� ������ �� ������
  function L502_GetErrorString(err: LongInt) : string; stdcall;
  // �������� ����������� �� �������� ������.
  function L502_LedBlink(hnd: t_l502_hnd): LongInt; stdcall;
  // ��������� ������������� ���������� �� ������� ������.
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
    // �������� ���������� ��������
    res := _get_serials(nil, 0, flags, devcnt);
    if (res >= 0) and (devcnt>0) then
    begin
      // �������� ������ ��� devcnt �������� �������
      ser_arr:=GetMemory(devcnt*L502_SERIAL_SIZE);
      //�������� �� ������
      res:= _get_serials(ser_arr, devcnt, flags, PLongWord(nil)^);
      if (res > 0) then
      begin
        //������������� ������ ������������� �������
        SetLength(serials, res);
        //��������� ���������� ������ � ������������ ������ �����
        for i:=0 to res-1 do
          serials[i] := string(ser_arr[i]);
      end;
      //����������� ������, ���������� ��� ��������� ������
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
      // �������� ���������� ��������
      res := _get_serials(ser_arr, Length(serials), flags, devcnt);
      if res >= 0 then
      begin
        //��������� ���������� ������ � ������������ ������ �����
        for i:=0 to res-1 do
          serials[i] := string(ser_arr[i]);
      end;
      //����������� ������, ���������� ��� ��������� ������
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
  ///������ ����� ������ �� Flash-������ ������.
  function L502_FlashWrite(hnd: t_l502_hnd; addr: LongWord;
                           const data: array of Byte; size: LongWord): LongInt; stdcall;
  begin
     if (LongWord(Length(data)) < size) then
      L502_FlashWrite := L502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      L502_FlashWrite:=_bf_mem_write(hnd, addr, data, size);
  end;
end.
 
