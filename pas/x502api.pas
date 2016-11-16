unit x502api;
interface
uses Windows, SysUtils;

const
  // ������������ ���������� ���������� ������� � �������
  X502_LTABLE_MAX_CH_CNT      = 256;
  // ���������� ���������� ��� ��������� ����������
  X502_ADC_RANGE_CNT          = 6;

  // ���������� ������� ��� � ������ � ����� ������
  X502_ADC_COMM_CH_CNT        = 32;
  // ���������� ������� ��� � ���������������� ������
  X502_ADC_DIFF_CH_CNT        = 16;

  // ������������ �������� ��� ����������� ���������� �� ����������� ������
  X502_LCH_AVG_SIZE_MAX       = 128;
  //������������ �������� �������� ������� ���
  X502_ADC_FREQ_DIV_MAX       = (1024*1024);
  // ������������ �������� �������� ������� ����������� ��������� �����
  X502_DIN_FREQ_DIV_MAX       = (1024*1024);

  // ����������� �������� �������� ������� ����������� ������
  X502_OUT_FREQ_DIV_MIN       = 2;
  // ������������ �������� �������� ������� ����������� ������
  X502_OUT_FREQ_DIV_MAX       = 1024;
  // �������� �������� ������� ������ �� ��������� (������� ����� ������
  // ������������ � L502 � ������� �������� ���� ���� 0.5) 
  X502_OUT_FREQ_DIV_DEFAULT   = 2;

  // ������������ �������� ����������� �������� ��� ���
  X502_ADC_INTERFRAME_DELAY_MAX = $1FFFFF;
  // ������� �� ��������� ��� ���������� ������� � BlackFin
  X502_BF_CMD_DEFAULT_TOUT      = 500;

  // ��� ���, ��������������� ������������� �������� �����
  X502_ADC_SCALE_CODE_MAX       = 6000000;
  // ��� ���, ��������������� ������������� �������� �����
  X502_DAC_SCALE_CODE_MAX       = 30000;

  // ������������ ���������� �������� � ������ � ��������� ����������
  X502_DEVNAME_SIZE             = 32;
  // ������������ ���������� �������� � ������ � �������� �������
  X502_SERIAL_SIZE              = 32;

  // ������������ ���������� �������� � ������ � ��������� �����������
  X502_LOCATION_STR_SIZE        = 64;
  // ������ MAC-������ ��� Ethernet ����������
  X502_MAC_ADDR_SIZE            = 6;
  // ������ ������ � ��������� ���������� ����������
  X502_INSTANCE_NAME_SIZE       = 64;
  // ������������ ������ ������ � ������� �� ���������
  X502_PASSWORD_SIZE            = 32;

  // ����������� ��������� �������� ������� ������� �������
  X502_EXT_REF_FREQ_MAX         = 1500000;


  //������ ���������������� ������� Flash-������
  X502_FLASH_USER_SIZE          = $100000;
  //����������� ������� �� ���������� ������� � BlackFin � ��
  X502_BF_REQ_TOUT              = 500;
  // �������� ��� � �������
  X502_DAC_RANGE                = 5;
  // ���������� ������� ���
  X502_DAC_CH_CNT               = 2;
  // ���������� �������� ������� � ������
  X502_DOUT_LINES_CNT           = 16;

  // ����� � ������, ����������, ��� ��������� ������������
  X502_STREAM_IN_MSG_OVERFLOW   = $01010000;

  X502_DEVREC_SIGN              = $4C524543;

  { ----------------- ���� ������ ���������� ---------------------------}
    // ������� ��������� ��� ������
    X502_ERR_OK                           = 0;
    // � ������� ������� ���������������� ��������� ������
    X502_ERR_INVALID_HANDLE               = -1;
    // ������ ��������� ������
    X502_ERR_MEMORY_ALLOC                 = -2;
    // ������� ������� ��� �������� ����������
    X502_ERR_ALREADY_OPENED               = -3;
    // ���������� � ��������� ����������� �� ������� � �������
    X502_ERR_DEVICE_NOT_FOUND             = -4;
    // ������ � ���������� �������� (��� ������� ��-�� ����, ��� ���������� ��� ������� � ������ ���������)
    X502_ERR_DEVICE_ACCESS_DENIED         = -5;
    // ������ �������� ����������
    X502_ERR_DEVICE_OPEN                  = -6;
    // � ������� ������� ���������������� ���������
    X502_ERR_INVALID_POINTER              = -7;
    // ������� �� ����� ���� ��������� ��� ���������� ������ ����� ������
    X502_ERR_STREAM_IS_RUNNING            = -8;
    // ������ ������ ������ ����������� �����
    X502_ERR_RECV                         = -9;
    // ������ ������ ������ ��� ����������� ������
    X502_ERR_SEND                         = -10;
    // ��������� ������������ ����������� ������ ��� ������ ����������� �����
    X502_ERR_STREAM_OVERFLOW              = -11;
    // ����������� ��������� � ������ ����������� �����
    X502_ERR_UNSUP_STREAM_MSG             = -12;
    // ������ �������� ���������� ��������
    X502_ERR_MUTEX_CREATE                 = -13;
    // �������� ��������� ��������
    X502_ERR_MUTEX_INVALID_HANDLE         = -14;
    // ������� ����� �������� ������������ ��������
    X502_ERR_MUTEX_LOCK_TOUT              = -15;
    // ������ ������������ ��������
    X502_ERR_MUTEX_RELEASE                = -16;
    // ������������ ��������� ��������
    X502_ERR_INSUFFICIENT_SYSTEM_RESOURCES= -17;
    // ������ ����������� ��� �� �����������
    X502_ERR_NOT_IMPLEMENTED              = -18;
    // ������������� ������ �������
    X502_ERR_INSUFFICIENT_ARRAY_SIZE      = -19;
    // ������ ������ �������� FPGA
    X502_ERR_FPGA_REG_READ                = -20;
    // ������ ������ �������� FPGA
    X502_ERR_FPGA_REG_WRITE               = -21;
    // ���� ������ ��� ����������
    X502_ERR_STREAM_IS_NOT_RUNNING        = -22;
    // ������ ������������ ����������
    X502_ERR_INTERFACE_RELEASE            = -23;
    // ������ ������� ������
    X502_ERR_THREAD_START                 = -24;
    // ������ �������� ������
    X502_ERR_THREAD_STOP                  = -25;
    // ���������� ���� ���������
    X502_ERR_DEVICE_DISCONNECTED          = -26;
    // �������� ������ ������ �� ����������� ������
    X502_ERR_IOCTL_INVALID_RESP_SIZE      = -27;
    // �������� ��� ����������
    X502_ERR_INVALID_DEVICE               = -28;
    // ���������������� ������ � ����������
    X502_ERR_INVALID_DEVICE_RECORD        = -29;
    // �������� ����� ������������ ������
    X502_ERR_INVALID_CONFIG_HANDLE        = -30;
    // ����� � ����������� ������� ��� �� ���� �����������
    X502_ERR_DEVICE_NOT_OPENED            = -31;
    // ������ �������� �� �������� ��� �������� ���������� ����� � �����������
    X502_ERR_INVALID_OP_FOR_IFACE         = -32;
    // �� �������� ���� ������
    X502_ERR_FPGA_NOT_LOADED              = -33;
    // �������� ������������ USB-����������
    X502_ERR_INVALID_USB_CONFIGURATION    = -34;
    // ����� �������� ������ ���������� �������
    X502_ERR_INVALID_LTABLE_SIZE          = -102;
    // ����� �������� ����� ����������� ������
    X502_ERR_INVALID_LCH_NUMBER           = -103;
    // ������� ������ �������� ��������� ���
    X502_ERR_INVALID_LCH_RANGE            = -104;
    // ������� ����� ����� ��������� ��� ����������� ������
    X502_ERR_INVALID_LCH_MODE             = -105;
    // ������� ����� ����� ����������� ������ ��� ��������� �����������
    X502_ERR_INVALID_LCH_PHY_NUMBER       = -106;
    // ������� ����� ������ ���������� ��� ����������� ������
    X502_ERR_INVALID_LCH_AVG_SIZE         = -107;
    // ������� ����� �������� ������� ����� ������ ���
    X502_ERR_INVALID_ADC_FREQ_DIV         = -108;
    // ������� ����� �������� ������� ����������� ����� �������� �����
    X502_ERR_INVALID_DIN_FREQ_DIV         = -109;
    // ������� ����� ����� ������ ������ L502
    X502_ERR_INVALID_MODE                 = -110;
    // �������� ����� ������ ���
    X502_ERR_INVALID_DAC_CHANNEL          = -111;
    // �������� ��� ������ ������� ������� �������������
    X502_ERR_INVALID_REF_FREQ             = -112;
    // ������� ������ �������� ����������� ��������
    X502_ERR_INVALID_INTERFRAME_DELAY     = -113;
    // ������� ����� ����� �������������
    X502_ERR_INVALID_SYNC_MODE            = -114;
    // ������� ����� ����� ������ ������
    X502_ERR_INVALID_STREAM_CH            = -115;
    // ������� ����� �������� ������� ����������� ������
    X502_ERR_INVALID_OUT_FREQ_DIV         = -116;
    // ������ ������� ������� ������� �������������
    X502_ERR_REF_FREQ_NOT_LOCKED          = -131;
    // ����������� ������ � �������� �������� � �������
    X502_ERR_IOCTL_FAILD                  = -132;
    // ����� ������� �������� ���������� ���������� ������������ ������� � ��������
    X502_ERR_IOCTL_TIMEOUT                = -133;
    // ������ ��������� ���������� � ���������� �� ��������
    X502_ERR_GET_INFO                     = -134;
    // �� ����� �������� �� ���� ������� ����� ����� � �������� �����
    X502_ERR_DIG_IN_NOT_RDY               = -135;
    // ������� ������������ ���� �� ������
    X502_ERR_RECV_INSUFFICIENT_WORDS      = -136;
    // ������� ��������� ��������, ��������� ������� ���, ��� ��� ����������
    X502_ERR_DAC_NOT_PRESENT              = -137;
    // �������� ����� ������ � �������������� ������ ����������� �����
    X502_ERR_PROC_INVALID_CH_NUM          = -140;
    // �������� ��� ��������� � �������������� ������ ����������� �����
    X502_ERR_PROC_INVALID_CH_RANGE        = -141;
    // ����� �������� ����� �� Flash-������
    X502_ERR_FLASH_INVALID_ADDR           = -142;
    // ����� �������� ������ ����� ������ ��� ������ � Flash-�������
    X502_ERR_FLASH_INVALID_SIZE           = -143;
    // ����� ������� �������� ���������� ������ �� Flash-������
    X502_ERR_FLASH_WRITE_TOUT             = -144;
    // ����� ������� �������� ���������� �������� ����� Flash-������
    X502_ERR_FLASH_ERASE_TOUT             = -145;
    // �������� ������� ��� �������� Flash-������ �������� ������� ����� � 4 �����
    X502_ERR_FLASH_SECTOR_BOUNDARY        = -146;
    // �� ������� ������� ����� ��� ����������
    X502_ERR_SOCKET_OPEN                  = -147;
    // ��������� ����� �����������
    X502_ERR_CONNECTION_TOUT              = -148;
    // ���������� ������� ������ �����������
    X502_ERR_CONNECTION_CLOSED_BY_DEV     = -149;
    // �� ������� ���������� �������� ������ ������ ������
    X502_ERR_SOCKET_SET_BUF_SIZE          = -150;
    // ���������� ��� �������� ������ �� �����������
    X502_ERR_NO_DATA_CONNECTION           = -151;
    // �� ������� ��������� ��������� � ���������� ������
    X502_ERR_NO_STREAM_END_MSG            = -152;
    // ���������� ���� �������� ������ ��������
    X502_ERR_CONNECTION_RESET             = -153;
    // �� ������� ����� ���� � ��������� �������
    X502_ERR_HOST_UNREACHABLE             = -154;
    // ������ ������������ TCP-����������
    X502_ERR_TCP_CONNECTION_ERROR         = -155;
    // �� ������� ������� ���� �������� BlackFin
    X502_ERR_LDR_FILE_OPEN                = -180;
    // ������ ������ �� ���� �������� BlackFin
    X502_ERR_LDR_FILE_READ                = -181;
    // �������� ������ ����� �������� BlackFin
    X502_ERR_LDR_FILE_FORMAT              = -182;
    // ������������ ����������� LDR-�����, ����������� ��� ������ �������� BlackFin �� HDMA
    X502_ERR_LDR_FILE_UNSUP_FEATURE       = -183;
    // �������� ��������� ����� ��������� � �������� BlackFin
    X502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR  = -184;
    // ����� ������� ���������� ������� �� ������/������ ������ BlackFin
    X502_ERR_BF_REQ_TIMEOUT               = -185;
    // ������� ��� BlackFin ��� ��� ��������� � �������� ���������
    X502_ERR_BF_CMD_IN_PROGRESS           = -186;
    // ������� ����� ���������� ����������� ������� ����������� BlackFin
    X502_ERR_BF_CMD_TIMEOUT               = -187;
    // ���������� ������������ ������ � ����� �� ������� � BlackFin
    X502_ERR_BF_CMD_RETURN_INSUF_DATA     = -188;
    // ����� ������� �������� ���������� ���������� BlackFin � ������ ��������
    X502_ERR_BF_LOAD_RDY_TOUT             = -189;
    // ������� ��������� �������� ��� ������� ����� ���������� ��������� ���
    // ���������� ����������� ���������� � ������
    X502_ERR_BF_NOT_PRESENT               = -190;
    // �������� ����� ������ BlackFin ��� ������ ��� ������ �� HDMA
    X502_ERR_BF_INVALID_ADDR              = -191;
    // �������� ������ ������, ������������ � ����������� �������� � BlackFin
    X502_ERR_BF_INVALID_CMD_DATA_SIZE     = -192;

    {--------------- ��������� ���������� � ������� ------------------}
    X502_IFACE_UNKNOWN = 0; // ����������� ���������
    X502_IFACE_USB     = 1; // ���������� ���������� �� USB
    X502_IFACE_ETH     = 2; // ���������� ���������� �� Ethernet ����� TCP/IP
    X502_IFACE_PCI     = 3; // ���������� ���������� �� PCI/PCIe

    {-------- �����, ����������� ������� �������������� ������� ------}
     // �������, ��� ����� ������� �������� ������ ������ ��� ���������,
     //   ������� ��� �� �������
    X502_GETDEVS_FLAGS_ONLY_NOT_OPENED = 1;



    {-------- ����� ��� ���������� ��������� ��������. ---------------}
    //���������� (������� � ������ ���������) ������� �������� �������� �������
    X502_DIGOUT_WORD_DIS_H = $00020000;
    //���������� ������� �������� �������� �������
    X502_DIGOUT_WORD_DIS_L = $00010000;


    {-------- ��������� ��� ������ ������� ������� -------------------}
    X502_REF_FREQ_2000KHZ  = 2000000; // ������� 2���
    X502_REF_FREQ_1500KHZ  = 1500000; // ������� 1.5���

    {-------- ��������� ��������� ��� ������ ��� ----------------------}
    X502_ADC_RANGE_10 = 0; // �������� +/-10V
    X502_ADC_RANGE_5  = 1; // �������� +/-5V
    X502_ADC_RANGE_2  = 2; // �������� +/-2V
    X502_ADC_RANGE_1  = 3; // �������� +/-1V
    X502_ADC_RANGE_05 = 4; // �������� +/-0.5V
    X502_ADC_RANGE_02 = 5; // �������� +/-0.2V


    {-------- ����� ��������� ��� ����������� ������ ------------------}
    X502_LCH_MODE_COMM = 0; // ��������� ���������� ������������ ����� �����
    X502_LCH_MODE_DIFF = 1; // ���������������� ��������� ����������
    X502_LCH_MODE_ZERO = 2; // ��������� ������������ ����


    {-------- ������ ������������� ------------------------------------}
    X502_SYNC_INTERNAL        = 0; // ���������� ������
    X502_SYNC_EXTERNAL_MASTER = 1; // �� �������� ������� �� ������� �������������
    X502_SYNC_DI_SYN1_RISE    = 2; // �� ������ ������� DI_SYN1
    X502_SYNC_DI_SYN2_RISE    = 3; // �� ������ ������� DI_SYN2
    X502_SYNC_DI_SYN1_FALL    = 6; // �� ����� ������� DI_SYN1
    X502_SYNC_DI_SYN2_FALL    = 7;  // �� ����� ������� DI_SYN2

    {-------- �����, ����������� ���������� �������� ������ -------------------}
    X502_PROC_FLAGS_VOLT            = $00000001; // �������, ��� ����� ������������� ��������
                                                 // ��� � ������
    X502_PROC_FLAGS_DONT_CHECK_CH   = $00010000; // �������, ��� �� ����� ��������� ���������� ������� �������

    {-------- ����� ��� ����������� ���������� ������� ������ -----------------}
    X502_STREAM_ADC  = $01; //����� ������ �� ���
    X502_STREAM_DIN  = $02; // ����� ������ � �������� ������
    X502_STREAM_DAC1 = $10; // ����� ������ ������� ������ ���
    X502_STREAM_DAC2 = $20; // ����� ������ ������� ������ ���
    X502_STREAM_DOUT = $40; // ����� ������ �� �������� ������
    // ����������� ���� ������, ������������ ������ ������ �� ����
    X502_STREAM_ALL_IN = X502_STREAM_ADC or X502_STREAM_DIN;
    // ����������� ���� ������, ������������ ������ ������ �� �����
    X502_STREAM_ALL_OUT = X502_STREAM_DAC1 or X502_STREAM_DAC2 or X502_STREAM_DOUT;


    {--- ���������, ������������ ��� ������������� ������� �� �� � ������ -----}
    X502_STREAM_OUT_WORD_TYPE_DOUT   = $0; // �������� �����
    X502_STREAM_OUT_WORD_TYPE_DAC1   = $40000000; // ��� ��� 1-�� ������ ���
    X502_STREAM_OUT_WORD_TYPE_DAC2   = $80000000; // ��� ��� 2-�� ������ ���

    {------------------ ����� ������ ������ L502 ------------------------------}
    X502_MODE_FPGA  = 0; // ��� ������ ������ ���������� ����� ���� �����
                         //   ���������� ��������� BlackFin
    X502_MODE_DSP   = 1; // ��� ������ ������ ���������� ����� ����������
                         //   ���������, ������� ������ ���� ��������
                         //   ��������� ��� ��������� ���� �������
    X502_MODE_DEBUG = 2; // ���������� �����

    {------------------- ������ ������� ��� -----------------------------------}
    X502_DAC_CH1 = 0;  //������ ����� ���
    X502_DAC_CH2 = 1;  //������ ����� ���

    {----------- �����, ������������ ��� ������ ������ �� ��� -----------------}
    // ���������, ��� �������� ������ � ������� � ��� ������ ��� �����
    // ��������� � ���� ���. ���� ���� �� ������, �� ���������, ��� ��������
    // ���������� � �����
    X502_DAC_FLAGS_VOLT    = $0001;
    // ���������, ��� ����� ��������� ������������� ������������ �����
    //    ������� �������� �� ���.
    X502_DAC_FLAGS_CALIBR  = $0002;

    {---------- ������ ������� ��� �������� ������� ������ --------------------}
    X502_STREAM_CH_IN  = 0; // ����� ����� �� ����
    X502_STREAM_CH_OUT = 1; // ����� ����� �� �����

    {--- �������� �����, �� ������� ����� �������� ������������� ��������� ----}
    X502_PULLUPS_DI_H      = $01; // ������� �������� �������� ������ (������ ��� L502)
    X502_PULLUPS_DI_L      = $02; // ������� �������� �������� ������ (������ ��� L502)
    X502_PULLUPS_DI_SYN1   = $04; // ����� SYN1
    X502_PULLUPS_DI_SYN2   = $08; // ����� SYN2
    X502_PULLDOWN_CONV_IN  = $10; // �������� � 0 ����� ������������
                                  //   ������������� CONV_IN (������ ��� E502)
    X502_PULLDOWN_START_IN = $20; // �������� � 0 ����� ������������
                                  //   ������������� START_IN (������ ��� E502)



    {--------------- �����, ������������ ������� ����� � ������ ---------------}
    // ������� ������� �������������� ���������� ���
    X502_DEVFLAGS_DAC_PRESENT           = $00000001;
    // ������� ������� ����������������
    X502_DEVFLAGS_GAL_PRESENT           = $00000002;
    // ������� ������� ����������� ���������� BlackFin
    X502_DEVFLAGS_BF_PRESENT            = $00000004;

    // �������, ��� ���������� ������������ ��������� USB
    X502_DEVFLAGS_IFACE_SUPPORT_USB     = $00000100;
    // �������, ��� ���������� ������������ Ethernet
    X502_DEVFLAGS_IFACE_SUPPORT_ETH     = $00000200;
    // �������, ��� ���������� ������������ ��������� PCI/PCI-Express
    X502_DEVFLAGS_IFACE_SUPPORT_PCI     = $00000400;

    // �������, ��� ���������� ��������� � ������������� ����������
    X502_DEVFLAGS_INDUSTRIAL            = $00008000;

    // �������, ��� �� Flash-������ ������������ ���������� � ������
    X502_DEVFLAGS_FLASH_DATA_VALID      = $00010000;
    // �������, ��� �� Flash-������ ������������ �������������� ������������� ������������ ���
    X502_DEVFLAGS_FLASH_ADC_CALIBR_VALID = $00020000;
    // �������, ��� �� Flash-������ ������������ �������������� ������������� ������������ ���
    X502_DEVFLAGS_FLASH_DAC_CALIBR_VALID = $00040000;

    // �������, ��� ������������ �������� ���� � ��� ������� ���� ���������
    X502_DEVFLAGS_FPGA_LOADED            = $00800000;
    // �������, ��� ���������� ��� ������� (������������ ������ ������ t_x502_devrec)
    X502_DEVFLAGS_DEVREC_OPENED          = $01000000;


    { ��� ����������� ������ � ������������� ���������� }
    // � ���� ������������ ���������� �� ���������� ����������
    X502_LOCATION_TYPE_NONE             = 0;
    // � ���� ������������ ���������� ���������� ������ � ������� ����������
    X502_LOCATION_TYPE_ADDR             = 1;
    // � ���� ������������ ���������� ���������� ������ � ������ ����������
    X502_LOCATION_TYPE_INSTANCE_NAME    = 2;

    {----------------����� ��� ������ ������������ ������----------------------}
    // ���� ���������, ��� ������� ��� ����� ������� ����� ��������� ��� ��������
    //    ����� ����� ����������� �������.
    X502_OUT_CYCLE_FLAGS_FORCE = $01;

    // ���� ���������, ��� ������� ������ ��������� ������ �������� �������
    // � ��������� ������� �� ����� (��� X502_OutCycleSetup()) ��� ����������
    // ��������� ������������ ������� (��� X502_OutCycleStop())
    X502_OUT_CYCLE_FLAGS_WAIT_DONE = $02;

	
	{ ----- ���� ������������ ������, ������� ����� �������������� ��� ��� 
	        � ����������� �� ���� ������, ������ �������� � �.�. -------------}
    // ��������� ��������� �������� ������� ������, ��������� �� #X502_OUT_FREQ_DIV_DEFAULT
    X502_FEATURE_OUT_FREQ_DIV       = 1;
    // ����������� ������ ������ ��������� ������ � ������� X502_OutGetStatusFlags() 
    X502_FEATURE_OUT_STATUS_FLAGS   = 2;

	{ --------------- ����� ��������� ��� ����������� ������ ------------------}
    // ���� ���������, ��� � ��������� ����� ����� � ������ �� �������� ���� 
    X502_OUT_STATUS_FLAG_BUF_IS_EMPTY = $01;
    // ���� ���������, ��� ���� ����������� ������ �� ����� � ������ ������ �����������
    //    �����-������ ��� � ������� ���������� ������ ������� � �������
    //    X502_OutGetStatusFlags() (� ����������� �� ����, ��� ���� ���������) 
    X502_OUT_STATUS_FLAG_BUF_WAS_EMPTY = $02;

type
  st_x502_devrec_inptr = record
  end;
  t_x502_devrec_inptr_hnd = ^st_x502_devrec_inptr;

  // ���������, ����������� ����������, �� ������� � ��� ����� ���������� ����������
  t_x502_devrec = record
    sign    : LongWord;  { ������� �������������� ���������.
                       ���� ������ ������������� (������������� ������-���� ����������),
                       �� ������ ���� ����� #X502_DEVREC_SIGN) }
    devname : array [0..X502_DEVNAME_SIZE-1] of AnsiChar; // �������� ����������
    serial  : array [0..X502_SERIAL_SIZE-1] of AnsiChar; // �������� �����
    location: array [0..X502_LOCATION_STR_SIZE-1] of AnsiChar; // �������� ����������� (���� ����)
    flags   : LongWord; // ����� �� #t_x502_dev_flags, ����������� ����������
    iface   : Byte;    // ���������, �� �������� ���������� ����������
    location_type : Byte; // ����������, ��� ������ ��������� � ���� location
    res     : array [0..122-1] of Byte;  // ������
    internal : t_x502_devrec_inptr_hnd; { ������������ ��������� �� ��������� �
                                          ����������� �� ���������� ��� �����������
                                          ��� ��������. }
  end;


  // ��������� ������ - ��������� �� ������������ ���������
  t_x502_hnd = ^st_x502;
  st_x502 = record
  end;

  { ������������� ������������ ��������� }
  t_x502_cbr_coef = record
    offs: Double;  // �������� ����
    k : Double; // ����������� �����
  end;


  { ������������� ������������ ������. }
  t_x502_cbr = record
    //������������� ������������ ���
    adc: array[0..X502_ADC_RANGE_CNT-1] of t_x502_cbr_coef;
    res1: array[0..63] of LongWord;
    // ������������� ������������ ���
    dac: array[0..X502_DAC_CH_CNT-1] of t_x502_cbr_coef;
    res2: array[0..19] of LongWord;
  end;

  { ���������� � ������}
  t_x502_info = record
    name: array[0..X502_DEVNAME_SIZE-1] of AnsiChar; // �������� ���������� ("L502")
    serial: array[0..X502_SERIAL_SIZE-1] of AnsiChar; // �������� �����
    devflags: LongWord; // ����� ����������� ������� � ������ ������������ �����
    fpga_ver : Word; // ������ ���� (������� ���� - ��������, ������� - ��������)
    plda_ver : Byte; // ������ ����, ������������ ���������� ������
    board_rev: Byte; // ������� �����
    mcu_firmware_ver : LongWord; // ������ �������� ����������� Cortex-M4. ������������� ������ ��� E502
    factory_mac : array[0..X502_MAC_ADDR_SIZE-1] of byte; { ��������� MAC-����� --- ������������ ������ ���
                                                                ��������� � Ethernet-����������� }
    res : array[0..110-1] of Byte; //������
    cbr : t_x502_cbr; // ��������� ������������� ������������ (�� Flash-������)
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

  //��������� ����� � �������
  function X502_Close(hnd: t_x502_hnd): LongInt; stdcall;
  //��������� ���������� � ������
  function X502_GetDevInfo(hnd: t_x502_hnd; out info: t_x502_info) : LongInt; stdcall;

  // �������� ������������� �������� � ������
  function X502_Configure(hnd: t_x502_hnd; flags: LongWord): LongInt; stdcall;
  // ��������� ���������� ����������� ������
  function X502_SetLChannel(hnd: t_x502_hnd; lch, phy_ch, mode, range, avg: LongWord): LongInt; stdcall;
  // ��������� ���������� ���������� �������
  function X502_SetLChannelCount(hnd: t_x502_hnd; lch_cnt : LongWord): LongInt; stdcall;
  // ��������� ���������� ���������� �������
  function X502_GetLChannelCount(hnd: t_x502_hnd; out lch_cnt: LongWord): LongInt; stdcall;
  // ��������� �������� ������� ����� ��� ���
  function X502_SetAdcFreqDivider(hnd: t_x502_hnd; adc_freq_div : LongWord): LongInt; stdcall;
  // ��������� �������� ����������� �������� ��� ���
  function X502_SetAdcInterframeDelay(hnd: t_x502_hnd; delay : LongWord): LongInt; stdcall;
  // ��������� �������� ������� ����������� ����� � �������� �����
  function X502_SetDinFreqDivider(hnd: t_x502_hnd; din_freq_div: LongWord): LongInt; stdcall;
  // ��������� �������� ������� ����������� ������
  function X502_SetOutFreqDivider(hnd: t_x502_hnd; out_freq_div : LongWord): LongInt; stdcall;
  // ��������� ������� ����� ���
  function X502_SetAdcFreq(hnd: t_x502_hnd; var f_acq, f_frame: Double): LongInt; stdcall;
  // ��������� ������� ����������� ����� � �������� ������
  function X502_SetDinFreq(hnd: t_x502_hnd; var f_din: Double): LongInt; stdcall;
  // ��������� ������� ����������� ������.
  function X502_SetOutFreq(hnd: t_x502_hnd; var f_dout: Double): LongInt; stdcall;
  // �������� ������� �������� ������ ����� ���
  function X502_GetAdcFreq(hnd: t_x502_hnd; out f_acq, f_frame: Double): LongInt; stdcall;
  // ��������� �������� ������� ������� �������������
  function X502_SetRefFreq(hnd: t_x502_hnd; freq: LongWord): LongInt; stdcall;
  // ��������� �������� ������� ������� ������� �������������
  function X502_SetExtRefFreqValue(hnd: t_x502_hnd; freq : Double): LongInt; stdcall;
  //��������� �������� ������� ������� �������������
  function X502_GetRefFreqValue(hnd: t_x502_hnd; out freq : Double): LongInt; stdcall;

  // ��������� ������ ��������� ������� �������������.
  function X502_SetSyncMode(hnd: t_x502_hnd; sync_mode: LongWord): LongInt; stdcall;
  // ��������� ������ ������� ������� �������������.
  function X502_SetSyncStartMode(hnd: t_x502_hnd; sync_start_mode: LongWord): LongInt; stdcall;
  // ���������� ����� ������ ������
  function X502_SetMode(hnd: t_x502_hnd; mode: LongWord): LongInt; stdcall;
  // ��������� �������� ������ ������ ������.
  function X502_GetMode(hnd: t_x502_hnd; out mode: LongWord): LongInt; stdcall;
  // ���������� ������������ ��� ���������� �������� ���.
  function X502_SetAdcCoef(hnd: t_x502_hnd; range: LongWord; k, offs: Double): LongInt; stdcall;
  // ��������� ������� ������������� ������������� ���.
  function X502_GetAdcCoef(hnd: t_x502_hnd; range: LongWord; out k, offs: Double): LongInt; stdcall;
  // ���������� ������������ ��� ���������� �������� ���.
  function X502_SetDacCoef(hnd: t_x502_hnd; ch : LongWord; k, double: Double): LongInt; stdcall;
  // ��������� ������� ������������� ������������� ���.
  function X502_GetDacCoef(hnd: t_x502_hnd; ch : LongWord; out k, double: Double): LongInt; stdcall;

  // ������� ������� ����� ���
  function X502_CalcAdcFreq(ref_freq: Double; lch_cnt: LongWord; var f_acq, f_frame: Double; out result_freq_div : LongWord; out  result_frame_delay: LongWord): LongInt; stdcall;
  // ������� ������� ����������� ����� � �������� ������.
  function X502_CalcDinFreq(ref_freq: Double; var f_din : Double; out result_freq_div : LongWord): LongInt; stdcall;
  // ������� ������� ����������� ������
  function X502_CalcOutFreq(ref_freq: Double; var f_dout : Double; out result_freq_div : LongWord): LongInt; stdcall;
  {----------------------- ������� ������������ �����-������ ------------------}
  // ����������� ����� ������ �� ����� ���.
  function X502_AsyncOutDac(hnd: t_x502_hnd; ch: LongWord; data: Double; flags: LongWord): LongInt; stdcall;
  // ����������� ����� ������ �� �������� ������.
  function X502_AsyncOutDig(hnd: t_x502_hnd; val, msk: LongWord): LongInt; stdcall;
  // ����������� ���� �������� � �������� ������.
  function X502_AsyncInDig(hnd: t_x502_hnd; out din: LongWord): LongInt; stdcall;
  // ����������� ���� ������ ����� ���.
  function X502_AsyncGetAdcFrame(hnd: t_x502_hnd; flags: LongWord; tout: LongWord; out data: array of Double): LongInt; stdcall;


  {-------------- ������� ��� ������ � ���������� ��������� ������-������� ----}
  // ���������� ���������� ������� �� ����/�����.
  function X502_StreamsEnable(hnd: t_x502_hnd; streams: LongWord): LongInt; stdcall;
  // ���������� ���������� ������� �� ����/�����.
  function X502_StreamsDisable(hnd: t_x502_hnd; streams: LongWord): LongInt; stdcall;
  // �������� ��������, ����� ���������� ������ ���������.
  function X502_GetEnabledStreams(hnd: t_x502_hnd; out streams: LongWord): LongInt; stdcall;
  // ������ ���������� ������� �����/������.
  function X502_StreamsStart(hnd: t_x502_hnd): LongInt; stdcall;
  // ������� ���������� ������� �����/������.
  function X502_StreamsStop(hnd: t_x502_hnd): LongInt; stdcall;
  // ��������, ������� �� ����� ������
  function X502_IsRunning(hnd: t_x502_hnd): LongInt; stdcall;


  // ������ ������ ��� � �������� ������ �� ������.
  function X502_Recv(hnd: t_x502_hnd; out buf : array of LongWord; size: LongWord; tout : LongWord): LongInt; stdcall;
  // �������� ��������� ������ ��� � �������� ������� � ������.
  function X502_Send(hnd: t_x502_hnd; const buf : array of LongWord; size: LongWord; tout: LongWord): LongInt; stdcall;
  // ��������� �������� �������� ��� �� ������.
  function X502_ProcessAdcData(hnd: t_x502_hnd; const src: array of LongWord;
                               out dest: array of Double; var size : LongWord;
                               flags : LongWord): LongInt; stdcall;
  // ��������� �������� �� ������ ������.
 function X502_ProcessData(hnd: t_x502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord):LongInt; stdcall;
  // ��������� �������� �� ������ ������ �������������� �������.
  function X502_ProcessDataWithUserExt(hnd: t_x502_hnd; const src: array of LongWord; size: LongWord;
                     flags : LongWord; out adc_data: array of Double; var adc_data_size : LongWord;
                     out din_data: array of LongWord; var din_data_size: LongWord;
                     out usr_data: array of LongWord; var usr_data_size: LongWord):LongInt; stdcall;
  // ���������� ������ ��� ������ � ������.
  function X502_PrepareData(hnd: t_x502_hnd; const dac1, dac2: array of Double;
                            const digout: array of LongWord; size, flags : LongWord;
                            out out_buf: array of LongWord):LongInt; stdcall;

  // �������� ���������� �������� � ������ ������ �� ����.
  function X502_GetRecvReadyCount(hnd: t_x502_hnd; out rdy_cnt: LongWord):LongInt; stdcall;
  // �������� ������ ���������� ����� � ������ ������ �� �����.
  function X502_GetSendReadyCount(hnd: t_x502_hnd; out rdy_cnt: LongWord):LongInt; stdcall;
  // �������� ����� ���������� ���������� ����������� ������ ��� ��� ���������.
  function X502_GetNextExpectedLchNum(hnd: t_x502_hnd; out lch: LongWord):LongInt; stdcall;
  // ������ ���������� ������ ���������� ������
  function X502_PreloadStart(hnd: t_x502_hnd): LongInt; stdcall;

  //��������� ����� ������������ ������������ ������� �� �����
  function X502_OutCycleLoadStart(hnd: t_x502_hnd; size: LongWord):LongInt; stdcall;
  //��������� ����� ������������ ������������ ������� �� �����
  function X502_OutCycleSetup(hnd: t_x502_hnd; flags: LongWord):LongInt; stdcall;
  // ������� ������ ������������ �������
  function X502_OutCycleStop(hnd: t_x502_hnd; flags: LongWord):LongInt; stdcall;
  // ��������, ��������� �� ��������� ��� ������� ������������ �������
  function X502_OutCycleCheckSetupDone(hnd: t_x502_hnd; out done : LongBool):LongInt; stdcall;  
  // ������ ������ ������� ������
  function X502_OutGetStatusFlags(hnd: t_x502_hnd; out status : LongWord): LongInt; stdcall;

  // ��������� ������� ������ ��� ����������� ����� ��� ������.
  function X502_SetStreamBufSize(hnd: t_x502_hnd;  dma_ch, size: LongWord): LongInt; stdcall;
  // ��������� ���� ��� �������� ������ �� ���� ��� �����.
  function X502_SetStreamStep(hnd: t_x502_hnd; dma_ch, step: LongWord): LongInt; stdcall;

  {------------ ������� ��� ������ � ���������� ����������� -------------------}

  // �������� �������� ����������� ���������� BlackFin.
  function X502_BfLoadFirmware(hnd: t_x502_hnd; filename: string): LongInt; stdcall;
  // ��������, ��������� �� �������� BlackFIn
  function X502_BfCheckFirmwareIsLoaded(hnd: t_x502_hnd; out version: LongWord): LongInt; stdcall;
  /// ������ ����� ������ �� ������ ����������� ����������.
  function X502_BfMemRead(hnd: t_x502_hnd; addr : LongWord; out regs: array of LongWord;
                                size: LongWord): LongInt; stdcall;
  /// ������ ����� ������ � ������ ����������� ����������.
  function X502_BfMemWrite(hnd: t_x502_hnd; addr : LongWord;
                           const regs: array of LongWord; size: LongWord): LongInt; stdcall;
  /// �������� ����������� ������� ����������� ����������.
  function X502_BfExecCmd(hnd: t_x502_hnd; cmd_code : Word; par : LongWord;
                const snd_data : array of LongWord; snd_size : LongWord;
                out rcv_data : array of LongWord; rcv_size : LongWord;
                tout: LongWord; out recvd_size: LongWord): LongInt; stdcall;

  {-------------------  ������� ��� ������ � Flash-������� ������ -------------}
  /// ������ ����� ������ �� Flash-������ ������.
  function X502_FlashRead(hnd: t_x502_hnd; addr: LongWord;
                          out data: array of Byte; size: LongWord): LongInt; stdcall;
  ///������ ����� ������ �� Flash-������ ������.
  function X502_FlashWrite(hnd: t_x502_hnd; addr: LongWord;
                           const data: array of Byte; size: LongWord): LongInt; stdcall;
  /// �������� ����� �� Flash-������.
  function X502_FlashErase(hnd: t_x502_hnd; addr: LongWord; size: LongWord): LongInt; stdcall;
  /// ���������� ������ � ���������������� ������� Flash-������.
  function X502_FlashWriteEnable(hnd: t_x502_hnd): LongInt; stdcall;
  /// ������ ������ � ���������������� ������� Flash-������.
  function X502_FlashWriteDisable(hnd: t_x502_hnd): LongInt; stdcall;

  {-----------------  �������������� ��������������� ������� ------------------}
  //�������� ������ ����������.
  function X502_GetLibraryVersion() : LongWord; stdcall;
  // ��������� ������ �� ������
  function X502_GetErrorString(err: LongInt) : string; stdcall;
  // �������� ����������� �� �������� ������.
  function X502_LedBlink(hnd: t_x502_hnd): LongInt; stdcall;
  // ��������� ������������� ���������� �� ������� ������.
  function X502_SetDigInPullup(hnd: t_x502_hnd; pullups : LongWord): LongInt; stdcall;
  // �������� ��������� ������� �������� �����������
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
  ///������ ����� ������ �� Flash-������ ������.
  function X502_FlashWrite(hnd: t_x502_hnd; addr: LongWord;
                           const data: array of Byte; size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(data)) < size) then
      X502_FlashWrite := X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      X502_FlashWrite:=_bf_mem_write(hnd, addr, data, size);
  end;
end.

