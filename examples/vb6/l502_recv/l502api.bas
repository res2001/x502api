Attribute VB_Name = "l502api"
' ������ ��� ���������� ��������, �������� � �������  ��� ���������� ������� L502 �� � ����

'������������ ���������� ���������� ������� � �������
Const L502_LTABLE_MAX_CH_CNT = 256
'���������� ���������� ��� ��������� ����������
Const L502_ADC_RANGE_CNT = 6
'������������ �������� ��� ����������� ���������� �� ����������� ������
Const L502_LCH_AVG_SIZE_MAX = 128
'������������ �������� �������� ������� ���
Const L502_ADC_FREQ_DIV_MAX = 1024& * 1024&
'������������ �������� �������� ������� ����������� ��������� �����
Const L502_DIN_FREQ_DIV_MAX = 1024& * 1024&

'������������ �������� ����������� �������� ��� ���
Const L502_ADC_INTERFRAME_DELAY_MAX = &H1FFFFF

'������� �� ��������� ��� ���������� ������� � BlackFin
Const L502_BF_CMD_DEFAULT_TOUT = 500
 
'��� ���, ��������������� ������������� �������� �����
Const L502_ADC_SCALE_CODE_MAX = 6000000
'��� ���, ��������������� ������������� �������� �����
Const L502_DAC_SCALE_CODE_MAX = 30000

'������������ ���������� �������� � ������ � ��������� ����������
Const L502_DEVNAME_SIZE = 32
'������������ ���������� �������� � ������ � �������� �������
Const L502_SERIAL_SIZE = 32

'����������� ��������� �������� ������� ������� �������
Const L502_EXT_REF_FREQ_MAX = 2000000
'������ ���������������� ������� Flash-������
Const L502_FLASH_USER_SIZE = &H100000

'C���������� ������� �� ���������� ������� � BlackFin � ��
Const L502_BF_REQ_TOUT = 500

'�������� ��� � �������
Const L502_DAC_RANGE = 5#

'���������� ������� ���
Const L502_DAC_CH_CNT = 2
 
'����� � ������, ����������, ��� ��������� ������������
Const L502_STREAM_IN_MSG_OVERFLOW = &H1010000




Public Enum t_lpcie_errs
    L502_ERR_OK = 0
    '� ������� ������� ���������������� ��������� ������
    L502_ERR_INVALID_HANDLE = -1
    '������ ��������� ������
    L502_ERR_MEMORY_ALLOC = -2
    '������� ������� ��� �������� ����������
    L502_ERR_ALREADY_OPENED = -3
    '���������� � ��������� ����������� �� ������� � �������
    L502_ERR_DEVICE_NOT_FOUND = -4
    '������ � ���������� �������� (��� ������� ��-�� ����, ��� ���������� ��� ������� � ������ ���������)
    L502_ERR_DEVICE_ACCESS_DENIED = -5
    '������ �������� ����������
    L502_ERR_DEVICE_OPEN = -6
    '� ������� ������� ���������������� ���������
    L502_ERR_INVALID_POINTER = -7
    '������� �� ����� ���� ��������� ��� ���������� ������ ����� ������
    L502_ERR_STREAM_IS_RUNNING = -8
    '������ ������ ������ ����������� �����
    L502_ERR_RECV = -9
    '������ ������ ������ ��� ����������� ������
    L502_ERR_SEND = -10
    '��������� ������������ ����������� ������ ��� ������ ����������� �����
    L502_ERR_STREAM_OVERFLOW = -11
    '����������� ��������� � ������ ����������� �����
    L502_ERR_UNSUP_STREAM_MSG = -12
    '������ �������� ���������� ��������
    L502_ERR_MUTEX_CREATE = -13
    '�������� ��������� ��������
    L502_ERR_MUTEX_INVALID_HANDLE = -14
    '������� ����� �������� ������������ ��������
    L502_ERR_MUTEX_LOCK_TOUT = -15
    '������ ������������ ��������
    L502_ERR_MUTEX_RELEASE = -16
    '������������ ��������� ��������
    L502_ERR_INSUFFICIENT_SYSTEM_RESOURCES = -17
    '������ ����������� ��� �� �����������
    L502_ERR_NOT_IMPLEMENTED = -18
    '������������� ������ �������
    L502_ERR_INSUFFICIENT_ARRAY_SIZE = -19
    '������ ������ �������� FPGA
    L502_ERR_FPGA_REG_READ = -20
    '������ ������ �������� FPGA
    L502_ERR_FPGA_REG_WRITE = -21
    '���� ������ ��� ����������
    L502_ERR_STREAM_IS_NOT_RUNNING = -22
    '����� �������� ������ ���������� �������
    L502_ERR_INVALID_LTABLE_SIZE = -102
    '����� �������� ����� ����������� ������
    L502_ERR_INVALID_LCH_NUMBER = -103
    '������� ������ �������� ��������� ���
    L502_ERR_INVALID_LCH_RANGE = -104
    '������� ����� ����� ��������� ��� ����������� ������
    L502_ERR_INVALID_LCH_MODE = -105
    '������� ����� ����� ����������� ������ ��� ��������� �����������
    L502_ERR_INVALID_LCH_PHY_NUMBER = -106
    '������� ����� ������ ���������� ��� ����������� ������
    L502_ERR_INVALID_LCH_AVG_SIZE = -107
    '������� ����� �������� ������� ����� ������ ���
    L502_ERR_INVALID_ADC_FREQ_DIV = -108
    '������� ����� �������� ������� ����������� ����� �������� �����
    L502_ERR_INVALID_DIN_FREQ_DIV = -108
    '������� ����� ����� ������ ������ L502
    L502_ERR_INVALID_MODE = -109
    '�������� ����� ������ ���
    L502_ERR_INVALID_DAC_CHANNEL = -110
    '�������� ��� ������ ������� ������� �������������
    L502_ERR_INVALID_REF_FREQ = -111
    '������� ������ �������� ����������� ��������
    L502_ERR_INVALID_INTERFRAME_DELAY = -112
    '������� ����� ����� �������������
    L502_ERR_INVALID_SYNC_MODE = -113
    '������� ����� ����� ������ DMA
    L502_ERR_INVALID_DMA_CH = -114

    '������ ������� ������� ������� �������������
    L502_ERR_REF_FREQ_NOT_LOCKED = -131
    '����������� ������ � �������� �������� � �������
    L502_ERR_IOCTL_FAILD = -132
    '����� ������� �������� ���������� ���������� ������������ ������� � ��������
    L502_ERR_IOCTL_TIMEOUT = -133
    '������ ��������� ���������� � ���������� �� ��������
    L502_ERR_GET_INFO = -134
    '�� ����� �������� �� ���� ������� ����� ����� � �������� �����
    L502_ERR_DIG_IN_NOT_RDY = -135
    '������� ������������ ���� �� ������
    L502_ERR_RECV_INSUFFICIENT_WORDS = -136
    '������� ��������� ��������, ��������� ������� ���, ��� ��� ����������
    L502_ERR_DAC_NOT_PRESENT = -137
    '�������� ����� ������ � �������������� ������ ����������� �����
    L502_ERR_PROC_INVALID_CH_NUM = -140
    '�������� ��� ��������� � �������������� ������ ����������� �����
    L502_ERR_PROC_INVALID_CH_RANGE = -141
    '����� �������� ����� �� Flash-������
    L502_ERR_FLASH_INVALID_ADDR = -142
    '����� �������� ������ ����� ������ ��� ������ � Flash-�������
    L502_ERR_FLASH_INVALID_SIZE = -143
    '����� ������� �������� ���������� ������ �� Flash-������
    L502_ERR_FLASH_WRITE_TOUT = -144
    '����� ������� �������� ���������� �������� ����� Flash-������
    L502_ERR_FLASH_ERASE_TOUT = -145
    '�������� ������� ��� �������� Flash-������ �������� ������� ����� � 4 �����
    L502_ERR_FLASH_SECTOR_BOUNDARY = -146
    '�� ������� ������� ���� �������� BlackFin
    L502_ERR_LDR_FILE_OPEN = -180
    '������ ������ �� ���� �������� BlackFin
    L502_ERR_LDR_FILE_READ = -181
    '�������� ������ ����� �������� BlackFin
    L502_ERR_LDR_FILE_FORMAT = -182
    '������������ ����������� LDR-�����, ����������� ��� ������ �������� BlackFin �� HDMA
    L502_ERR_LDR_FILE_UNSUP_FEATURE = -183
    '�������� ��������� ����� ��������� � �������� BlackFin
    L502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR = -184
    '����� ������� ���������� ������� �� ������/������ ������ BlackFin
    L502_ERR_BF_REQ_TIMEOUT = -185
    '������� ��� BlackFin ��� ��� ��������� � �������� ���������
    L502_ERR_BF_CMD_IN_PROGRESS = -186
    '������� ����� ���������� ����������� ������� ����������� BlackFin
    L502_ERR_BF_CMD_TIMEOUT = -187
    '���������� ������������ ������ � ����� �� ������� � BlackFin
    L502_ERR_BF_CMD_RETURN_INSUF_DATA = -188
    '����� ������� �������� ���������� ���������� BlackFin � ������ ��������
    L502_ERR_BF_LOAD_RDY_TOUT = -189
    '������� ��������� �������� ��� ������� ����� ���������� ��������� ���
    '���������� ����������� ���������� � ������
    L502_ERR_BF_NOT_PRESENT = -190
    '�������� ����� ������ BlackFin ��� ������ ��� ������ �� HDMA
    L502_ERR_BF_INVALID_ADDR = -191
    '�������� ������ ������, ������������ � ����������� �������� � BlackFin
    L502_ERR_BF_INVALID_CMD_DATA_SIZE = -192
End Enum


'�����, ����������� ������� �������������� �������
Public Enum t_l502_getdevs_flags
   ' �������, ��� ����� ������� �������� ������ ������ ��� ���������,  ������� ��� �� �������
    L502_GETDEVS_FLAGS_ONLY_NOT_OPENED = &H1&
End Enum


'����� ��� ���������� ��������� ��������.
Public Enum t_l502_digout_word_flags
    L502_DIGOUT_WORD_DIS_H = &H20000      '���������� (������� � ������ ���������)  ������� �������� �������� �������
    L502_DIGOUT_WORD_DIS_L = &H10000  '���������� ������� �������� �������� �������
End Enum

'��������� ��� ������ ������� �������
Public Enum t_l502_ref_freq
    L502_REF_FREQ_2000KHZ = 2000000   '������� 2���
    L502_REF_FREQ_1500KHZ = 1500000  '������� 1.5���
End Enum

'��������� ��������� ��� ������ ���
Public Enum t_l502_adc_range
    L502_ADC_RANGE_10 = 0 '�������� +/-10V
    L502_ADC_RANGE_5 = 1  '�������� +/-5V
    L502_ADC_RANGE_2 = 2  '�������� +/-2V
    L502_ADC_RANGE_1 = 3  '�������� +/-1V
    L502_ADC_RANGE_05 = 4 '�������� +/-0.5V
    L502_ADC_RANGE_02 = 5  '�������� +/-0.2V
End Enum

'����� ��������� ��� ����������� ������
Public Enum t_l502_lch_mode
    L502_LCH_MODE_COMM = 0  '��������� ���������� ������������ ����� �����
    L502_LCH_MODE_DIFF = 1 '���������������� ��������� ����������
    L502_LCH_MODE_ZERO = 2  '��������� ������������ ����
End Enum

'������ �������������.
Public Enum t_l502_sync_mode
    L502_SYNC_INTERNAL = 0        '���������� ������
    L502_SYNC_EXTERNAL_MASTER = 1 '�� �������� ������� �� ������� �������������
    L502_SYNC_DI_SYN1_RISE = 2    '�� ������ ������� DI_SYN1
    L502_SYNC_DI_SYN2_RISE = 3    '�� ������ ������� DI_SYN2
    L502_SYNC_DI_SYN1_FALL = 6    '�� ����� ������� DI_SYN1
    L502_SYNC_DI_SYN2_FALL = 7     '�� ����� ������� DI_SYN2
End Enum

'�����, ����������� ���������� �������� ������
Public Enum t_l502_proc_flags
    '�������, ��� ����� ������������� �������� ��� � ������
    L502_PROC_FLAGS_VOLT = &H1&
    ' �������, ��� �� ����� ��������� ���������� ������� �������
    '   � �������� ������ � �������� �� ���������� �������.
    '   ����� �������������� ��� ������������� �������� BlackFin
    '    ��� �������� � �� �� ���� ������.
    L502_PROC_FLAGS_DONT_CHECK_CH = &H10000
End Enum

'����� ��� ����������� ���������� ������� ������
Public Enum t_l502_streams
    L502_STREAM_ADC = &H1&  '����� ������ �� ���
    L502_STREAM_DIN = &H2&   '����� ������ � �������� ������
    L502_STREAM_DAC1 = &H10& '����� ������ ������� ������ ���
    L502_STREAM_DAC2 = &H20& '����� ������ ������� ������ ���
    L502_STREAM_DOUT = &H40& '����� ������ �� �������� ������
    '����������� ���� ������, ������������ ������ ������ �� ����
    L502_STREAM_ALL_IN = L502_STREAM_ADC Or L502_STREAM_DIN
    '����������� ���� ������, ������������ ������ ������ �� �����
    L502_STREAM_ALL_OUT = L502_STREAM_DAC1 Or L502_STREAM_DAC2 Or L502_STREAM_DOUT
End Enum

'���������, ������������ ��� ������������� ������� �� �� � ������
Public Enum t_stream_out_wrd_type
    L502_STREAM_OUT_WORD_TYPE_DOUT = &H0&   '�������� �����
    L502_STREAM_OUT_WORD_TYPE_DAC1 = &H40000000    '��� ��� 1-�� ������ ���
    L502_STREAM_OUT_WORD_TYPE_DAC2 = &H80000000    '��� ��� 2-�� ������ ���
End Enum

'����� ������ ������ L502
Public Enum t_l502_mode
    L502_MODE_FPGA = 0  '��� ������ ������ ���������� ����� ���� �����
                                                    '���������� ��������� BlackFin
    L502_MODE_DSP = 1    '��� ������ ������ ���������� ����� ����������
                                                    '���������, ������� ������ ���� ��������
                                                    '��������� ��� ��������� ���� �������
    L502_MODE_DEBUG = 2  '���������� �����
End Enum

'������ ������� ���.
Public Enum t_l502_dac_ch
   L502_DAC_CH1 = 0 '������ ����� ���
   L502_DAC_CH2 = 1  '������ ����� ���
End Enum

'�����, ������������ ��� ������ ������ �� ���.
 Public Enum t_l502_dacout_flags
    '���������, ��� �������� ������ � ������� � ��� ������ ��� �����
    '    ��������� ��� � ���� ���. ���� ���� �� ������, �� ���������, ��� ��������
    '    ���������� � �����
    L502_DAC_FLAGS_VOLT = &H1&
    '���������, ��� ����� ��������� ������������� ������������ �����
    '    ������� �������� �� ���.
    L502_DAC_FLAGS_CALIBR = &H2&
End Enum

'������ ������� DMA
Public Enum t_l502_dma_ch
    L502_DMA_CH_IN = 0  '����� ����� DMA �� ����
    L502_DMA_CH_OUT = 1 '����� ����� DMA �� �����
End Enum

'�������� �����, �� ������� ����� �������� ������������� ���������
Public Enum t_l502_pullups
    L502_PULLUPS_DI_H = &H1&     '������� �������� �������� ������
    L502_PULLUPS_DI_L = &H2&     '������� �������� �������� ������
    L502_PULLUPS_DI_SYN1 = &H4&  '����� SYN1
    L502_PULLUPS_DI_SYN2 = &H8&  '����� SYN2
End Enum

'�����, ������������ ������� ����� � ������
Public Enum t_l502_dev_flags
    '������� ������� �������������� ���������� ���
    L502_DEVFLAGS_DAC_PRESENT = &H1&
    '������� ������� ����������������
    L502_DEVFLAGS_GAL_PRESENT = &H2&
    '������� ������� ����������� ���������� BlackFin
    L502_DEVFLAGS_BF_PRESENT = &H4&
    '�������, ��� �� Flash-������ ������������ ���������� � ������
    L502_DEVFLAGS_FLASH_DATA_VALID = &H10000
    '�������, ��� �� Flash-������ ������������ �������������� �������������
    '   ������������ ���
    L502_DEVFLAGS_FLASH_ADC_CALIBR_VALID = &H20000
    '�������, ��� �� Flash-������ ������������ �������������� �������������
    '  ������������ ���
    L502_DEVFLAGS_FLASH_DAC_CALIBR_VALID = &H40000
End Enum

'����� ��� ������ ������������ ������
Public Enum t_l502_out_cycle_flags
    L502_OUT_CYCLE_FLAGS_FORCE = &H1&
End Enum




' ������������� ������������ ���������
Public Type t_l502_cbr_coef
    offs As Double '�������� ����
    k As Double '����������� �����
End Type


'������������� ������������ ������
Public Type t_l502_cbr
    adc(0 To L502_ADC_RANGE_CNT - 1) As t_l502_cbr_coef '������������� ������������ ���
    res1(0 To 64 - 1) As Long '������
    dac(0 To L502_DAC_CH_CNT - 1) As t_l502_cbr_coef '������������� ������������ ���
    res2(0 To 20 - 1) As Long
End Type


Public Type t_l502_info
    DevName(0 To L502_DEVNAME_SIZE - 1) As Byte
    Serial(0 To L502_SERIAL_SIZE - 1) As Byte '�������� �����
    devflags As Long '����� �� #t_l502_dev_flags, ����������� ������� � ������ ������������ �����
    fpga_ver As Integer '������ ���� (������� ���� - ��������, ������� - ��������)
    plda_ver As Byte '������ ����, ������������ ���������� ������
    board_rev As Byte '������� �����
    res(0 To 120 - 1) As Byte '������
    cbr As t_l502_cbr      '��������� ������������� ������������ (�� Flash-������)
End Type



Private Declare Function tmp_L502_GetSerialList Lib "l502api" Alias "L502_GetSerialList" (ByVal serials As Long, ByVal size As Long, ByVal flags As Long, ByRef devcnt As Long) As Long


Public Declare Function L502_Create Lib "l502api" () As Long
Public Declare Function L502_Free Lib "l502api" (ByVal hnd As Long) As Long
Public Declare Function L502_Open Lib "l502api" (ByVal hnd As Long, ByVal Serial As String) As Long
Public Declare Function L502_Close Lib "l502api" (ByVal hnd As Long) As Long
Public Declare Function L502_GetDevInfo Lib "l502api" (ByVal hnd As Long, ByRef info As t_l502_info) As Long


Public Declare Function L502_Configure Lib "l502api" (ByVal hnd As Long, ByVal flags As Long) As Long
Public Declare Function L502_SetLChannel Lib "l502api" (ByVal hnd As Long, ByVal lch As Long, ByVal phy_ch As Long, ByVal mode As Long, ByVal range As Long, ByVal avg As Long) As Long
'��������� ���������� ���������� �������.
Public Declare Function L502_SetLChannelCount Lib "l502api" (ByVal hnd As Long, ByVal lch_cnt As Long) As Long
'��������� ���������� ���������� �������.
Public Declare Function L502_GetLChannelCount Lib "l502api" (ByVal hnd As Long, ByRef lch_cnt As Long) As Long
'��������� �������� ������� ����� ��� ���
Public Declare Function L502_SetAdcFreqDivider Lib "l502api" (ByVal hnd As Long, ByVal adc_freq_div As Long) As Long
'��������� �������� ����������� �������� ��� ���.
Public Declare Function L502_SetAdcInterframeDelay Lib "l502api" (ByVal hnd As Long, ByVal delay As Long) As Long
'��������� �������� ������� ����������� ����� � �������� �����.
Public Declare Function L502_SetDinFreqDivider Lib "l502api" (ByVal hnd As Long, ByVal din_freq_div As Long) As Long
'��������� ������� ����� ���.
Public Declare Function L502_SetAdcFreq Lib "l502api" (ByVal hnd As Long, ByRef f_acq As Double, ByRef f_frame As Double) As Long
'��������� ������� ����������� ����� � �������� ������.
Public Declare Function L502_SetDinFreq Lib "l502api" (ByVal hnd As Long, ByRef f_din As Double) As Long
'�������� ������� �������� ������ ����� ���
Public Declare Function L502_GetAdcFreq Lib "l502api" (ByVal hnd As Long, ByRef f_acq As Double, ByRef f_frame As Double) As Long
'��������� �������� ������� ������� �������������.
Public Declare Function L502_SetRefFreq Lib "l502api" (ByVal hnd As Long, ByVal freq As Long) As Long
'��������� ������ ��������� ������� �������������.
Public Declare Function L502_SetSyncMode Lib "l502api" (ByVal hnd As Long, ByVal sync_mode As Long) As Long
'��������� ������ ������� ������� �������������
Public Declare Function L502_SetSyncStartMode Lib "l502api" (ByVal hnd As Long, ByVal sync_start_mode As Long) As Long
'���������� ����� ������ ������.
Public Declare Function L502_SetMode Lib "l502api" (ByVal hnd As Long, ByVal mode As Long) As Long
'��������� �������� ������ ������ ������.
Public Declare Function L502_GetMode Lib "l502api" (ByVal hnd As Long, ByRef mode As Long) As Long
'���������� ������������ ��� ���������� �������� ���
Public Declare Function L502_SetAdcCoef Lib "l502api" (ByVal hnd As Long, ByVal range As Long, ByVal k As Double, ByVal offs As Double) As Long
'��������� ������� ������������� ������������� ���.
Public Declare Function L502_GetAdcCoef Lib "l502api" (ByVal hnd As Long, ByVal range As Long, ByRef k As Double, ByRef offs As Double) As Long
'���������� ������������ ��� ���������� �������� ���.
Public Declare Function L502_SetDacCoef Lib "l502api" (ByVal hnd As Long, ByVal ch As Long, ByVal k As Double, ByVal offs As Double) As Long
'��������� ������� ������������� ������������� ���
Public Declare Function L502_GetDacCoef Lib "l502api" (ByVal hnd As Long, ByVal ch As Long, ByRef k As Double, ByRef offs As Double) As Long

'����������� ����� ������ �� ����� ���.
Public Declare Function L502_AsyncOutDac Lib "l502api" (ByVal hnd As Long, ByVal ch As Long, ByVal data As Double, ByVal flags As Long) As Long
'����������� ����� ������ �� �������� ������.
Public Declare Function L502_AsyncOutDig Lib "l502api" (ByVal hnd As Long, ByVal val As Long, ByVal msk As Long) As Long
'����������� ���� �������� � �������� ������.
Public Declare Function L502_AsyncInDig Lib "l502api" (ByVal hnd As Long, ByRef din As Long) As Long

Private Declare Function tmp_L502_AsyncGetAdcFrame Lib "l502api" Alias "L502_AsyncGetAdcFrame" (ByVal hnd As Long, ByVal flags As Long, ByVal tout As Long, ByRef data As Double) As Long

'���������� ���������� ������� �� ����/�����.
Public Declare Function L502_StreamsEnable Lib "l502api" (ByVal hnd As Long, ByVal streams As Long) As Long
'���������� ���������� ������� �� ����/�����.
Public Declare Function L502_StreamsDisable Lib "l502api" (ByVal hnd As Long, ByVal streams As Long) As Long
'������ ���������� ������� �����/������.
Public Declare Function L502_StreamsStart Lib "l502api" (ByVal hnd As Long) As Long
'������� ���������� ������� �����/������.
Public Declare Function L502_StreamsStop Lib "l502api" (ByVal hnd As Long) As Long
'��������, ������� �� ���������� ����/�����
Public Declare Function L502_IsRunning Lib "l502api" (ByVal hnd As Long) As Long
'������ ������ ��� � �������� ������ �� ������.
Private Declare Function tmp_L502_Recv Lib "l502api" Alias "L502_Recv" (ByVal hnd As Long, ByRef buf As Long, ByVal size As Long, ByVal tout As Long) As Long
'�������� ��������� ������ ��� � �������� ������� � ������.
Private Declare Function tmp_L502_Send Lib "l502api" Alias "L502_Send" (ByVal hnd As Long, ByRef buf As Long, ByVal size As Long, ByVal tout As Long) As Long
'��������� �������� �������� ��� �� ������.
Private Declare Function tmp_L502_ProcessAdcData Lib "l502api" Alias "L502_ProcessAdcData" (ByVal hnd As Long, ByRef src As Long, ByRef dest As Double, ByRef size As Long, ByVal flags As Long) As Long
'��������� �������� �� ������ ������.
Private Declare Function tmp_L502_ProcessData Lib "l502api" Alias "L502_ProcessData" (ByVal hnd As Long, ByRef src As Long, ByVal flags As Long, ByRef adc_data As Double, ByRef adc_data_size As Long, ByRef din_data As Long, ByRef din_data_size As Long) As Long
'��������� �������� �� ������ ������ � ����������������� �������.
Private Declare Function tmp_L502_ProcessDataWithUserExt Lib "l502api" Alias "L502_ProcessDataWithUserExt" (ByVal hnd As Long, ByRef src As Long, ByVal size As Long, ByVal flags As Long, ByRef adc_data As Double, ByRef adc_data_size As Long, ByRef din_data As Long, ByRef din_data_size As Long, ByRef usr_data As Long, ByRef usr_data_size As Long) As Long
'���������� ������ ��� ������ � ������.
Private Declare Function tmp_L502_PrepareData Lib "l502api" Alias "L502_PrepareData" (ByVal hnd As Long, ByRef dac1 As Double, ByRef dac2 As Double, ByRef digout As Long, ByVal size As Long, ByVal flags As Long, ByRef out_buf As Long) As Long

'�������� ���������� �������� � ������ ������ �� ����.
Public Declare Function L502_GetRecvReadyCountLib Lib "l502api" (ByVal hnd As Long, ByRef rdy_cnt As Long) As Long
'�������� ������ ���������� ����� � ������ ������ �� �����.
Public Declare Function L502_GetSendReadyCount Lib "l502api" (ByVal hnd As Long, ByRef rdy_cnt As Long) As Long
'�������� ����� ���������� ���������� ����������� ������ ��� ���  ���������.
Public Declare Function L502_GetNextExpectedLchNum Lib "l502api" (ByVal hnd As Long, ByRef lch As Long) As Long
'������ ���������� ������ ���������� ������
Public Declare Function L502_PreloadStart Lib "l502api" (ByVal hnd As Long) As Long
'������ �������� ������������ ������� �� �����
Public Declare Function L502_OutCycleLoadStart Lib "l502api" (ByVal hnd As Long, ByVal size As Long) As Long
'��������� ����� ������������ ������������ ������� �� �����
Public Declare Function L502_OutCycleSetup Lib "l502api" (ByVal hnd As Long, ByVal flags As Long) As Long
'������� ������ ������������ �������
Public Declare Function L502_OutCycleStop Lib "l502api" (ByVal hnd As Long, ByVal flags As Long) As Long
'��������� ������� ������ � �������� ��� ����������� ����� ��� ������.
Public Declare Function L502_SetDmaBufSize Lib "l502api" (ByVal hnd As Long, ByVal dma_ch As Long, ByVal size As Long) As Long
'��������� ���� ���������� ��� �������� ������ �� DMA.
Public Declare Function L502_SetDmaIrqStep Lib "l502api" (ByVal hnd As Long, ByVal dma_ch As Long, ByVal step As Long) As Long

'�������� �������� ����������� ���������� BlackFin.
Public Declare Function L502_BfLoadFirmware Lib "l502api" (ByVal hnd As Long, ByVal filename As String) As Long
'��������, ��������� �� �������� BlackFIn.
Public Declare Function L502_BfCheckFirmwareIsLoaded Lib "l502api" (ByVal hnd As Long, ByRef version As Long) As Long
'������ ����� ������ �� ������ ����������� ����������.
Private Declare Function tmp_L502_BfMemRead Lib "l502api" Alias "L502_BfMemRead" (ByVal hnd As Long, ByVal addr As Long, ByRef regs As Long, ByVal size As Long) As Long
'������ ����� ������ � ������ ����������� ����������.
Private Declare Function tmp_L502_BfMemWrite Lib "l502api" Alias "L502_BfMemWrite" (ByVal hnd As Long, ByVal addr As Long, ByRef regs As Long, ByVal size As Long) As Long
'�������� ����������� ������� ����������� ����������.
Private Declare Function tmp_L502_BfExecCmd Lib "l502api" Alias "L502_BfExecCmd" (ByVal hnd As Long, ByVal cmd_code As Integer, ByVal par As Long, ByRef snd_data As Long, ByVal snd_size As Long, ByRef rcv_data As Long, ByVal rcv_size As Long, ByVal tout As Long, ByRef recvd_size As Long) As Long

'������ ����� ������ �� Flash-������.
 Private Declare Function tmp_L502_FlashRead Lib "l502api" Alias "L502_FlashRead" (ByVal hnd As Long, ByVal addr As Long, ByRef data As Byte, ByVal size As Long) As Long
 '������ ����� ������ �� Flash-������ ������.
 Private Declare Function tmp_L502_FlashWrite Lib "l502api" Alias "L502_FlashWrite" (ByVal hnd As Long, ByVal addr As Long, ByRef data As Byte, ByVal size As Long) As Long
'�������� ����� �� Flash-������.
 Public Declare Function L502_FlashErase Lib "l502api" (ByVal hnd As Long, ByVal addr As Long, ByVal size As Long) As Long
 '���������� ������ � ���������������� ������� Flash-������.
 Public Declare Function L502_FlashWriteEnable Lib "l502api" (ByVal hnd As Long) As Long
 '������ ������ � ���������������� ������� Flash-������.
 Public Declare Function L502_FlashWriteDisable Lib "l502api" (ByVal hnd As Long) As Long
 
 '�������� ������ ����������.
Public Declare Function L502_GetDllVersion Lib "l502api" () As Long
'�������� ������ �������� ����������.
Public Declare Function L502_GetDriverVersion Lib "l502api" (ByVal hnd As Long, ByRef ver As Long) As Long
'�������� ����������� �� �������� ������.
Public Declare Function L502_LedBlink Lib "l502api" (ByVal hnd As Long) As Long
'��������� ������������� ���������� �� ������� ������.
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

