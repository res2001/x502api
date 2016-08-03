Attribute VB_Name = "x502api"
' ������ ��� ���������� ��������, �������� � �������, ����� ��� ������� L502 � E502 �� � ����

'������������ ���������� ���������� ������� � �������
Public Const X502_LTABLE_MAX_CH_CNT = 256
'���������� ���������� ��� ��������� ����������
Public Const X502_ADC_RANGE_CNT = 6

'���������� ������� ��� � ������ � ����� ������
Public Const X502_ADC_COMM_CH_CNT = 32

'���������� ������� ��� � ���������������� ������
Public Const X502_ADC_DIFF_CH_CNT = 16

'������������ �������� ��� ����������� ���������� �� ����������� ������
Public Const X502_LCH_AVG_SIZE_MAX = 128
'������������ �������� �������� ������� ���
Public Const X502_ADC_FREQ_DIV_MAX = 1024& * 1024&
'������������ �������� �������� ������� ����������� ��������� �����
Public Const X502_DIN_FREQ_DIV_MAX = 1024& * 1024&

'����������� �������� �������� ������� ����������� ������
Public Const X502_OUT_FREQ_DIV_MIN = 2
'������������ �������� �������� ������� ����������� ������
Public Const X502_OUT_FREQ_DIV_MAX = 1024
'�������� �������� ������� ������ �� ��������� (������� ����� ������
'������������ � L502 � ������� �������� ���� ���� 0.5)
Public Const X502_OUT_FREQ_DIV_DEFAULT = 2

'������������ �������� ����������� �������� ��� ���
Public Const X502_ADC_INTERFRAME_DELAY_MAX = &H1FFFFF

'������� �� ��������� ��� ���������� ������� � BlackFin
Public Const X502_BF_CMD_DEFAULT_TOUT = 500
 
'��� ���, ��������������� ������������� �������� �����
Public Const X502_ADC_SCALE_CODE_MAX = 6000000
'��� ���, ��������������� ������������� �������� �����
Public Const X502_DAC_SCALE_CODE_MAX = 30000

'������������ ���������� �������� � ������ � ��������� ����������
Public Const X502_DEVNAME_SIZE = 32
'������������ ���������� �������� � ������ � �������� �������
Public Const X502_SERIAL_SIZE = 32
'������������ ���������� �������� � ������ � ��������� �����������
Public Const X502_LOCATION_STR_SIZE = 64
'������ MAC-������ ��� Ethernet ����������
Public Const X502_MAC_ADDR_SIZE = 6
'������ ������ � ��������� ���������� ����������
Public Const X502_INSTANCE_NAME_SIZE = 64
'������������ ������ ������ � ������� �� ���������
Public Const X502_PASSWORD_SIZE = 32

'����������� ��������� �������� ������� ������� �������
Public Const X502_EXT_REF_FREQ_MAX = 1500000
'������ ���������������� ������� Flash-������
Public Const X502_FLASH_USER_SIZE = &H100000

'C���������� ������� �� ���������� ������� � BlackFin � ��
Public Const X502_BF_REQ_TOUT = 500

'�������� ��� � �������
Public Const X502_DAC_RANGE = 5#

'���������� ������� ���
Public Const X502_DAC_CH_CNT = 2
 
'���������� �������� ������� � ������
Public Const X502_DOUT_LINES_CNT = 16
'����� � ������, ����������, ��� ��������� ������������
Public Const X502_STREAM_IN_MSG_OVERFLOW = &H1010000

'�������� ���� ��������� � ������ � ���������� #t_x502_devrec.
Public Const X502_DEVREC_SIGN = &H4C524543

'���� ������ ����������
Public Enum t_lpcie_errs
    '������� ��������� ��� ������
    X502_ERR_OK = 0
    '� ������� ������� ���������������� ��������� ������
    X502_ERR_INVALID_HANDLE = -1
    '������ ��������� ������
    X502_ERR_MEMORY_ALLOC = -2
    '������� ������� ��� �������� ����������
    X502_ERR_ALREADY_OPENED = -3
    '���������� � ��������� ����������� �� ������� � �������
    X502_ERR_DEVICE_NOT_FOUND = -4
    '������ � ���������� �������� (��� ������� ��-�� ����, ��� ���������� ��� ������� � ������ ���������)
    X502_ERR_DEVICE_ACCESS_DENIED = -5
    '������ �������� ����������
    X502_ERR_DEVICE_OPEN = -6
    '� ������� ������� ���������������� ���������
    X502_ERR_INVALID_POINTER = -7
    '������� �� ����� ���� ��������� ��� ���������� ������ ����� ������
    X502_ERR_STREAM_IS_RUNNING = -8
    '������ ������ ������ ����������� �����
    X502_ERR_RECV = -9
    '������ ������ ������ ��� ����������� ������
    X502_ERR_SEND = -10
    '��������� ������������ ����������� ������ ��� ������ ����������� �����
    X502_ERR_STREAM_OVERFLOW = -11
    '����������� ��������� � ������ ����������� �����
    X502_ERR_UNSUP_STREAM_MSG = -12
    '������ �������� ���������� ��������
    X502_ERR_MUTEX_CREATE = -13
    '�������� ��������� ��������
    X502_ERR_MUTEX_INVALID_HANDLE = -14
    '������� ����� �������� ������������ ��������
    X502_ERR_MUTEX_LOCK_TOUT = -15
    '������ ������������ ��������
    X502_ERR_MUTEX_RELEASE = -16
    '������������ ��������� ��������
    X502_ERR_INSUFFICIENT_SYSTEM_RESOURCES = -17
    '������ ����������� ��� �� �����������
    X502_ERR_NOT_IMPLEMENTED = -18
    '������������� ������ �������
    X502_ERR_INSUFFICIENT_ARRAY_SIZE = -19
    '������ ������ �������� FPGA
    X502_ERR_FPGA_REG_READ = -20
    '������ ������ �������� FPGA
    X502_ERR_FPGA_REG_WRITE = -21
    '���� ������ ��� ����������
    X502_ERR_STREAM_IS_NOT_RUNNING = -22
    '������ ������������ ����������
    X502_ERR_INTERFACE_RELEASE = -23
    '������ ������� ������
    X502_ERR_THREAD_START = -24
    '������ �������� ������
    X502_ERR_THREAD_STOP = -25
    '���������� ���� ���������
    X502_ERR_DEVICE_DISCONNECTED = -26
    '�������� ������ ������ �� ����������� ������
    X502_ERR_IOCTL_INVALID_RESP_SIZE = -27
    '�������� ��� ����������
    X502_ERR_INVALID_DEVICE = -28
    '���������������� ������ � ����������
    X502_ERR_INVALID_DEVICE_RECORD = -29
    '�������� ��������� ������������ ������
    X502_ERR_INVALID_CONFIG_HANDLE = -30
    '����� � ����������� ������� ��� �� ���� �����������
    X502_ERR_DEVICE_NOT_OPENED = -31
    '������ �������� �� �������� ��� �������� ���������� ����� � �����������
    X502_ERR_INVALID_OP_FOR_IFACE = -32
    '�� �������� ���� ������
    X502_ERR_FPGA_NOT_LOADED = -33
    '�������� ������������ USB-����������
    X502_ERR_INVALID_USB_CONFIGURATION = -34
    '�������� ��������� ��������� ������ ��������� � ����
    X502_ERR_INVALID_SVC_BROWSE_HANDLE = -35
    '�������� ��������� ������ � �������
    X502_ERR_INVALID_SVC_RECORD_HANDLE = -36
    '�� �������� ��������� ����������� ��������� � ��������� ����
    X502_ERR_DNSSD_NOT_RUNNING = -37
    '������ ��� ��������� � ��������� ����������� ��������� � ��������� ����
    X502_ERR_DNSSD_COMMUNICATION = -38
    '�������� ������� ������� ���������� ����������������� �������� ����������
    X502_ERR_SVC_RESOLVE_TIMEOUT = -39
    '������ � ��������� ����� ���������� ����������
    X502_ERR_INSTANCE_NAME_ENCODING = -40
    '���������� ������� �� ���������
    X502_ERR_INSTANCE_MISMATCH = -41
    '����������� �� �������������� ������� ������� �������� ����������
    X502_ERR_NOT_SUP_BY_FIRMWARE = -42
    '����������� �� �������������� ������� ������� �������� ����������
    X502_ERR_NOT_SUP_BY_DRIVER = -43
    '��������� ����� �������� ������������ ������������ ������� �� �����
    X502_ERR_OUT_CYCLE_SETUP_TOUT = -44
    '����������� ��� �������������� �����������
    X502_ERR_UNKNOWN_FEATURE_CODE = -45


    '����� �������� ������ ���������� �������
    X502_ERR_INVALID_LTABLE_SIZE = -102
    '����� �������� ����� ����������� ������
    X502_ERR_INVALID_LCH_NUMBER = -103
    '������� ������ �������� ��������� ���
    X502_ERR_INVALID_LCH_RANGE = -104
    '������� ����� ����� ��������� ��� ����������� ������
    X502_ERR_INVALID_LCH_MODE = -105
    '������� ����� ����� ����������� ������ ��� ��������� �����������
    X502_ERR_INVALID_LCH_PHY_NUMBER = -106
    '������� ����� ������ ���������� ��� ����������� ������
    X502_ERR_INVALID_LCH_AVG_SIZE = -107
    '������� ����� �������� ������� ����� ������ ���
    X502_ERR_INVALID_ADC_FREQ_DIV = -108
    '������� ����� �������� ������� ����������� ����� �������� �����
    X502_ERR_INVALID_DIN_FREQ_DIV = -109
    '������� ����� ����� ������ ������ X502
    X502_ERR_INVALID_MODE = -110
    '�������� ����� ������ ���
    X502_ERR_INVALID_DAC_CHANNEL = -111
    '�������� ��� ������ ������� ������� �������������
    X502_ERR_INVALID_REF_FREQ = -112
    '������� ������ �������� ����������� ��������
    X502_ERR_INVALID_INTERFRAME_DELAY = -113
    '������� ����� ����� �������������
    X502_ERR_INVALID_SYNC_MODE = -114
    '������� ����� ����� ������ ������
    X502_ERR_INVALID_STREAM_CH = -115
    '������� ����� �������� ������� ����������� ������
    X502_ERR_INVALID_OUT_FREQ_DIV = -116

    '������ ������� ������� ������� �������������
    X502_ERR_REF_FREQ_NOT_LOCKED = -131
    '����������� ������ � �������� �������� � �������
    X502_ERR_IOCTL_FAILD = -132
    '����� ������� �������� ���������� ���������� ������������ ������� � ��������
    X502_ERR_IOCTL_TIMEOUT = -133
    '������ ��������� ���������� � ���������� �� ��������
    X502_ERR_GET_INFO = -134
    '�� ����� �������� �� ���� ������� ����� ����� � �������� �����
    X502_ERR_DIG_IN_NOT_RDY = -135
    '������� ������������ ���� �� ������
    X502_ERR_RECV_INSUFFICIENT_WORDS = -136
    '������� ��������� ��������, ��������� ������� ���, ��� ��� ����������
    X502_ERR_DAC_NOT_PRESENT = -137
    '�������� ������������ ���� � ������
    X502_ERR_SEND_INSUFFICIENT_WORDS = -138
    '�� ������ ������ �� ���������� �������
    X502_ERR_NO_CMD_RESPONSE = -139

    '�������� ����� ������ � �������������� ������ ����������� �����
    X502_ERR_PROC_INVALID_CH_NUM = -140
    '�������� ��� ��������� � �������������� ������ ����������� �����
    X502_ERR_PROC_INVALID_CH_RANGE = -141
    '����� �������� ����� �� Flash-������
    X502_ERR_FLASH_INVALID_ADDR = -142
    '����� �������� ������ ����� ������ ��� ������ � Flash-�������
    X502_ERR_FLASH_INVALID_SIZE = -143
    '����� ������� �������� ���������� ������ �� Flash-������
    X502_ERR_FLASH_WRITE_TOUT = -144
    '����� ������� �������� ���������� �������� ����� Flash-������
    X502_ERR_FLASH_ERASE_TOUT = -145
    '�������� ������� ��� �������� Flash-������ �������� ������� ����� � 4 �����
    X502_ERR_FLASH_SECTOR_BOUNDARY = -146

    '�� ������� ������� ����� ��� ����������
    X502_ERR_SOCKET_OPEN = -147
    '��������� ����� �����������
    X502_ERR_CONNECTION_TOUT = -148
    '���������� ������� ������ �����������
    X502_ERR_CONNECTION_CLOSED_BY_DEV = -149
    '�� ������� ���������� �������� ������ ������ ������
    X502_ERR_SOCKET_SET_BUF_SIZE = -150
    '���������� ��� �������� ������ �� �����������
    X502_ERR_NO_DATA_CONNECTION = -151
    '�� ������� ��������� ��������� � ���������� ������
    X502_ERR_NO_STREAM_END_MSG = -152
    '���������� ���� �������� ������ ��������
    X502_ERR_CONNECTION_RESET = -153
    '�� ������� ����� ���� � ��������� �������
    X502_ERR_HOST_UNREACHABLE = -154
    '������ ������������ TCP-����������
    X502_ERR_TCP_CONNECTION_ERROR = -155

    '�� ������� ������� ���� �������� BlackFin
    X502_ERR_LDR_FILE_OPEN = -180
    '������ ������ �� ���� �������� BlackFin
    X502_ERR_LDR_FILE_READ = -181
    '�������� ������ ����� �������� BlackFin
    X502_ERR_LDR_FILE_FORMAT = -182
    '������������ ����������� LDR-�����, ����������� ��� ������ �������� BlackFin �� HDMA
    X502_ERR_LDR_FILE_UNSUP_FEATURE = -183
    '�������� ��������� ����� ��������� � �������� BlackFin
    X502_ERR_LDR_FILE_UNSUP_STARTUP_ADDR = -184
    '����� ������� ���������� ������� �� ������/������ ������ BlackFin
    X502_ERR_BF_REQ_TIMEOUT = -185
    '������� ��� BlackFin ��� ��� ��������� � �������� ���������
    X502_ERR_BF_CMD_IN_PROGRESS = -186
    '������� ����� ���������� ����������� ������� ����������� BlackFin
    X502_ERR_BF_CMD_TIMEOUT = -187
    '���������� ������������ ������ � ����� �� ������� � BlackFin
    X502_ERR_BF_CMD_RETURN_INSUF_DATA = -188
    '����� ������� �������� ���������� ���������� BlackFin � ������ ��������
    X502_ERR_BF_LOAD_RDY_TOUT = -189
    '������� ��������� �������� ��� ������� ����� ���������� ��������� ��� ���������� ����������� ���������� � ������
    X502_ERR_BF_NOT_PRESENT = -190
    '�������� ����� ������ BlackFin ��� ������ ��� ������ �� HDMA
    X502_ERR_BF_INVALID_ADDR = -191
    '�������� ������ ������, ������������ � ����������� �������� � BlackFin
    X502_ERR_BF_INVALID_CMD_DATA_SIZE = -192
End Enum


'��������� ���������� � �������
Public Enum t_x502_iface
    X502_IFACE_UNKNOWN = 0 '����������� ���������
    X502_IFACE_USB = 1     '���������� ���������� �� USB
    X502_IFACE_ETH = 2     '���������� ���������� �� Ethernet ����� TCP/IP
    X502_IFACE_PCI = 3     '���������� ���������� �� PCI/PCIe
End Enum

'�����, ����������� ������� �������������� �������
Public Enum t_x502_getdevs_flags
   ' �������, ��� ����� ������� �������� ������ ������ ��� ���������,  ������� ��� �� �������
    X502_GETDEVS_FLAGS_ONLY_NOT_OPENED = &H1&
End Enum


'����� ��� ���������� ��������� ��������.
Public Enum t_x502_digout_word_flags
    X502_DIGOUT_WORD_DIS_H = &H20000      '���������� (������� � ������ ���������)  ������� �������� �������� �������
    X502_DIGOUT_WORD_DIS_L = &H10000  '���������� ������� �������� �������� �������
End Enum

'��������� ��� ������ ������� �������
Public Enum t_x502_ref_freq
    X502_REF_FREQ_2000KHZ = 2000000   '������� 2���
    X502_REF_FREQ_1500KHZ = 1500000  '������� 1.5���
End Enum

'��������� ��������� ��� ������ ���
Public Enum t_x502_adc_range
    X502_ADC_RANGE_10 = 0 '�������� +/-10V
    X502_ADC_RANGE_5 = 1  '�������� +/-5V
    X502_ADC_RANGE_2 = 2  '�������� +/-2V
    X502_ADC_RANGE_1 = 3  '�������� +/-1V
    X502_ADC_RANGE_05 = 4 '�������� +/-0.5V
    X502_ADC_RANGE_02 = 5  '�������� +/-0.2V
End Enum

'����� ��������� ��� ����������� ������
Public Enum t_x502_lch_mode
    X502_LCH_MODE_COMM = 0  '��������� ���������� ������������ ����� �����
    X502_LCH_MODE_DIFF = 1 '���������������� ��������� ����������
    X502_LCH_MODE_ZERO = 2  '��������� ������������ ����
End Enum

'������ �������������.
Public Enum t_x502_sync_mode
    X502_SYNC_INTERNAL = 0        '���������� ������
    X502_SYNC_EXTERNAL_MASTER = 1 '�� �������� ������� �� ������� ������������ �������������
    X502_SYNC_DI_SYN1_RISE = 2    '�� ������ ������� DI_SYN1
    X502_SYNC_DI_SYN1_FALL = 3    '�� ������ ������� DI_SYN2
    X502_SYNC_DI_SYN2_RISE = 6    '�� ����� ������� DI_SYN1
    X502_SYNC_DI_SYN2_FALL = 7    '�� ����� ������� DI_SYN2
End Enum

'�����, ����������� ���������� �������� ������
Public Enum t_x502_proc_flags
    '�������, ��� ����� ������������� �������� ��� � ������
    X502_PROC_FLAGS_VOLT = &H1&
    ' �������, ��� �� ����� ��������� ���������� ������� �������
    '   � �������� ������ � �������� �� ���������� �������.
    '   ����� �������������� ��� ������������� �������� BlackFin
    '    ��� �������� � �� �� ���� ������.
    X502_PROC_FLAGS_DONT_CHECK_CH = &H10000
End Enum

'����� ��� ����������� ���������� ������� ������
Public Enum t_x502_streams
    X502_STREAM_ADC = &H1&  '����� ������ �� ���
    X502_STREAM_DIN = &H2&   '����� ������ � �������� ������
    X502_STREAM_DAC1 = &H10& '����� ������ ������� ������ ���
    X502_STREAM_DAC2 = &H20& '����� ������ ������� ������ ���
    X502_STREAM_DOUT = &H40& '����� ������ �� �������� ������
    '����������� ���� ������, ������������ ������ ������ �� ����
    X502_STREAM_ALL_IN = X502_STREAM_ADC Or X502_STREAM_DIN
    '����������� ���� ������, ������������ ������ ������ �� �����
    X502_STREAM_ALL_OUT = X502_STREAM_DAC1 Or X502_STREAM_DAC2 Or X502_STREAM_DOUT
End Enum

'���������, ������������ ��� ������������� ������� �� �� � ������
Public Enum t_x502_stream_out_wrd_type
    X502_STREAM_OUT_WORD_TYPE_DOUT = &H0&          '�������� �����
    X502_STREAM_OUT_WORD_TYPE_DAC1 = &H40000000    '��� ��� 1-�� ������ ���
    X502_STREAM_OUT_WORD_TYPE_DAC2 = &H80000000    '��� ��� 2-�� ������ ���
End Enum

'����� ������ ������
Public Enum t_x502_mode
    X502_MODE_FPGA = 0  '��� ������ ������ ���������� ����� ���� �����
                        '���������� ��������� BlackFin
    X502_MODE_DSP = 1   '��� ������ ������ ���������� ����� ����������
                        '���������, ������� ������ ���� ��������
                        '��������� ��� ��������� ���� �������
    X502_MODE_DEBUG = 2  '���������� �����
End Enum

'������ ������� ���.
Public Enum t_x502_dac_ch
   X502_DAC_CH1 = 0 '������ ����� ���
   X502_DAC_CH2 = 1 '������ ����� ���
End Enum

'�����, ������������ ��� ������ ������ �� ���.
 Public Enum t_x502_dacout_flags
    '���������, ��� �������� ������ � ������� � ��� ������ ��� �����
    '    ��������� ��� � ���� ���. ���� ���� �� ������, �� ���������, ��� ��������
    '    ���������� � �����
    X502_DAC_FLAGS_VOLT = &H1&
    '���������, ��� ����� ��������� ������������� ������������ �����
    '    ������� �������� �� ���.
    X502_DAC_FLAGS_CALIBR = &H2&
End Enum

'������ ������� ��� �������� ������� ������
Public Enum t_x502_stream_ch
    X502_STREAM_CH_IN = 0  '����� ����� �� ����
    X502_STREAM_CH_OUT = 1 '����� ����� �� �����
End Enum

'�������� �����, �� ������� ����� �������� ������������� ���������
Public Enum t_x502_pullups
    X502_PULLUPS_DI_H = &H1        '������� �������� �������� ������ (������ ��� L502)
    X502_PULLUPS_DI_L = &H2        '������� �������� �������� ������ (������ ��� L502)
    X502_PULLUPS_DI_SYN1 = &H4     '����� SYN1
    X502_PULLUPS_DI_SYN2 = &H8     '����� SYN2
    X502_PULLDOWN_CONV_IN = &H10   '�������� � 0 ����� ������������
                                   ' ������������� CONV_IN (������ ��� E502)
    X502_PULLDOWN_START_IN = &H20  '�������� � 0 ����� ������������
                                   ' ������������� START_IN (������ ��� E502)
End Enum

'�����, ������������ ������� ����� � ������
Public Enum t_x502_dev_flags
    '������� ������� �������������� ���������� ���
    X502_DEVFLAGS_DAC_PRESENT = &H1&
    '������� ������� ����������������
    X502_DEVFLAGS_GAL_PRESENT = &H2&
    '������� ������� ����������� ���������� BlackFin
    X502_DEVFLAGS_BF_PRESENT = &H4&

    '�������, ��� ���������� ������������ ��������� USB
    X502_DEVFLAGS_IFACE_SUPPORT_USB = &H100&
    '�������, ��� ���������� ������������ Ethernet
    X502_DEVFLAGS_IFACE_SUPPORT_ETH = &H200&
    '�������, ��� ���������� ������������ ��������� PCI/PCI-Express
    X502_DEVFLAGS_IFACE_SUPPORT_PCI = &H400&

    '�������, ��� ���������� ��������� � ������������� ����������
    X502_DEVFLAGS_INDUSTRIAL = &H8000&

    '�������, ��� �� Flash-������ ������������ ���������� � ������
    X502_DEVFLAGS_FLASH_DATA_VALID = &H10000
    '�������, ��� �� Flash-������ ������������ �������������� ������������� ������������ ���
    X502_DEVFLAGS_FLASH_ADC_CALIBR_VALID = &H20000
    '�������, ��� �� Flash-������ ������������ �������������� ������������� ������������ ���
    X502_DEVFLAGS_FLASH_DAC_CALIBR_VALID = &H40000

    '�������, ��� ������������ �������� ���� � ��� ������� ���� ���������
    X502_DEVFLAGS_FPGA_LOADED = &H800000
    '�������, ��� ���������� ��� ������� (������������ ������ ������ t_x502_devrec)
    X502_DEVFLAGS_DEVREC_OPENED = &H1000000
End Enum

Public Enum t_x502_location_type
    '� ���� ������������ ���������� �� ���������� ����������
    X502_LOCATION_TYPE_NONE = 0
    '� ���� ������������ ���������� ���������� ������ � ������� ����������
    X502_LOCATION_TYPE_ADDR = 1
    '� ���� ������������ ���������� ���������� ������ � ������ ����������
    X502_LOCATION_TYPE_INSTANCE_NAME = 2
End Enum

'����� ��� ������ ������������ ������
Public Enum t_x502_out_cycle_flags
    '������� ��� ����� ������� ����� ��������� ��� �������� ����� ����� ����������� �������
    X502_OUT_CYCLE_FLAGS_FORCE = &H1&
    '������� ������ ��������� ������ �������� �������
    ' � ��������� ������� �� ����� (��� X502_OutCycleSetup()) ��� ����������
    ' ��������� ������������ ������� (��� X502_OutCycleStop())
    X502_OUT_CYCLE_FLAGS_WAIT_DONE = &H2&
End Enum

'���� ������������ ������, ������� ����� �������������� ��� ��� � ����������� �� ���� ������, ������ �������� � �.�
Public Enum t_x502_features
    '��������� ��������� �������� ������� ������, ��������� �� X502_OUT_FREQ_DIV_DEFAULT
    X502_FEATURE_OUT_FREQ_DIV = 1
    '����������� ������ ������ ��������� ������ � ������� X502_OutGetStatusFlags()
    X502_FEATURE_OUT_STATUS_FLAGS = 2
End Enum

'����� ��������� ��� ����������� ������
Public Enum t_x502_out_status_flags
    '���� ���������, ��� � ��������� ����� ����� � ������ �� �������� ����
    X502_OUT_STATUS_FLAG_BUF_IS_EMPTY = &H1&
    '���� ���������, ��� ���� ����������� ������ �� ����� � ������ ������ �����������
    ' �����-������ ��� � ������� ���������� ������ ������� � �������
    ' X502_OutGetStatusFlags() (� ����������� �� ����, ��� ���� ���������)
    X502_OUT_STATUS_FLAG_BUF_WAS_EMPTY = &H2&
End Enum

'--------------- ���� ������ ---------------

'������ � ����������
Public Type t_x502_devrec
    sign As Long  '������� �������������� ���������
    DevName(0 To X502_DEVNAME_SIZE - 1) As Byte '�������� ����������
    serial(0 To X502_SERIAL_SIZE - 1) As Byte '�������� �����
    location(0 To X502_LOCATION_STR_SIZE - 1) As Byte '�������� ����������� (���� ����)
    flags As Long '����� �� #t_x502_dev_flags, ����������� ����������
    iface As Byte '���������, �� �������� ���������� ����������
    location_type As Byte  '����������, ��� ������ ��������� � ���� location (���� �������� �� #t_x502_location_type)
    res(0 To 122 - 1) As Byte '������
    internal As Long '������������ ��������� �� ��������� � �������������� �����������
End Type


' ������������� ������������ ���������
Public Type t_x502_cbr_coef
    offs As Double '�������� ����
    k As Double '����������� �����
End Type


'������������� ������������ ������
Public Type t_x502_cbr
    adc(0 To X502_ADC_RANGE_CNT - 1) As t_x502_cbr_coef '������������� ������������ ���
    res1(0 To 64 - 1) As Long '������
    dac(0 To X502_DAC_CH_CNT - 1) As t_x502_cbr_coef '������������� ������������ ���
    res2(0 To 20 - 1) As Long
End Type

' ���������� � ������ L502/E502
Public Type t_x502_info
    DevName(0 To X502_DEVNAME_SIZE - 1) As Byte '�������� ���������� ("L502" ��� "E502")
    serial(0 To X502_SERIAL_SIZE - 1) As Byte '�������� �����
    devflags As Long '����� �� #t_x502_dev_flags, ����������� ������� � ������ ������������ �����
    fpga_ver As Integer '������ ���� (������� ���� - ��������, ������� - ��������)
    plda_ver As Byte '������ ����, ������������ ���������� ������
    board_rev As Byte '������� �����
    mcu_firmware_ver As Long '������ �������� ����������� Cortex-M4. ������������� ������ ��� E502
    factory_mac(0 To X502_MAC_ADDR_SIZE - 1) As Byte '��������� MAC-����� --- ������������ ������ ���
                                                     '��������� � Ethernet-����������� */
    res(0 To 110 - 1) As Byte '������
    cbr As t_x502_cbr      '��������� ������������� ������������ (�� Flash-������)
End Type


Public Declare Function X502_Create Lib "x502api" () As Long
Public Declare Function X502_Free Lib "x502api" (ByVal hnd As Long) As Long

Public Declare Function X502_OpenByDevRecord Lib "x502api" (ByVal hnd As Long, ByRef devrec As t_x502_devrec) As Long
Private Declare Function tmp_X502_FreeDevRecordList Lib "x502api" Alias "X502_FreeDevRecordList" (ByRef list As t_x502_devrec, ByVal size As Long) As Long


Public Declare Function X502_Close Lib "x502api" (ByVal hnd As Long) As Long
Public Declare Function X502_GetDevInfo Lib "x502api" (ByVal hnd As Long, ByRef info As t_x502_info) As Long


Public Declare Function X502_Configure Lib "x502api" (ByVal hnd As Long, ByVal flags As Long) As Long
Public Declare Function X502_SetLChannel Lib "x502api" (ByVal hnd As Long, ByVal lch As Long, ByVal phy_ch As Long, ByVal mode As Long, ByVal range As Long, ByVal avg As Long) As Long
'��������� ���������� ���������� �������.
Public Declare Function X502_SetLChannelCount Lib "x502api" (ByVal hnd As Long, ByVal lch_cnt As Long) As Long
'��������� ���������� ���������� �������.
Public Declare Function X502_GetLChannelCount Lib "x502api" (ByVal hnd As Long, ByRef lch_cnt As Long) As Long
'��������� �������� ������� ����� ��� ���
Public Declare Function X502_SetAdcFreqDivider Lib "x502api" (ByVal hnd As Long, ByVal adc_freq_div As Long) As Long
'��������� �������� ����������� �������� ��� ���.
Public Declare Function X502_SetAdcInterframeDelay Lib "x502api" (ByVal hnd As Long, ByVal delay As Long) As Long
'��������� �������� ������� ����������� ����� � �������� �����.
Public Declare Function X502_SetDinFreqDivider Lib "x502api" (ByVal hnd As Long, ByVal din_freq_div As Long) As Long
'��������� �������� ������� ����������� ������
Public Declare Function X502_SetOutFreqDivider Lib "x502api" (ByVal hnd As Long, ByVal out_freq_div As Long) As Long
'��������� ������� ����� ���.
Public Declare Function X502_SetAdcFreq Lib "x502api" (ByVal hnd As Long, ByRef f_acq As Double, ByRef f_frame As Double) As Long
'��������� ������� ����������� ����� � �������� ������.
Public Declare Function X502_SetDinFreq Lib "x502api" (ByVal hnd As Long, ByRef f_din As Double) As Long
'��������� ������� ����������� ������.
Public Declare Function X502_SetOutFreq Lib "x502api" (ByVal hnd As Long, ByRef f_dout As Double) As Long
'�������� ������� �������� ������ ����� ���
Public Declare Function X502_GetAdcFreq Lib "x502api" (ByVal hnd As Long, ByRef f_acq As Double, ByRef f_frame As Double) As Long
'��������� �������� ������� ������� �������������.
Public Declare Function X502_SetRefFreq Lib "x502api" (ByVal hnd As Long, ByVal freq As Long) As Long
'��������� �������� ������� ������� ������� �������������
Public Declare Function X502_SetExtRefFreqValue Lib "x502api" (ByVal hnd As Long, ByVal freq As Double) As Long
'��������� �������� ������� ������� �������������
Public Declare Function X502_GetRefFreqValue Lib "x502api" (ByVal hnd As Long, ByRef freq As Double) As Long
'��������� ������ ��������� ������� �������������.
Public Declare Function X502_SetSyncMode Lib "x502api" (ByVal hnd As Long, ByVal sync_mode As Long) As Long
'��������� ������ ������� ������� �������������
Public Declare Function X502_SetSyncStartMode Lib "x502api" (ByVal hnd As Long, ByVal sync_start_mode As Long) As Long
'���������� ����� ������ ������.
Public Declare Function X502_SetMode Lib "x502api" (ByVal hnd As Long, ByVal mode As Long) As Long
'��������� �������� ������ ������ ������.
Public Declare Function X502_GetMode Lib "x502api" (ByVal hnd As Long, ByRef mode As Long) As Long
'���������� ������������ ��� ���������� �������� ���
Public Declare Function X502_SetAdcCoef Lib "x502api" (ByVal hnd As Long, ByVal range As Long, ByVal k As Double, ByVal offs As Double) As Long
'��������� ������� ������������� ������������� ���.
Public Declare Function X502_GetAdcCoef Lib "x502api" (ByVal hnd As Long, ByVal range As Long, ByRef k As Double, ByRef offs As Double) As Long
'���������� ������������ ��� ���������� �������� ���.
Public Declare Function X502_SetDacCoef Lib "x502api" (ByVal hnd As Long, ByVal ch As Long, ByVal k As Double, ByVal offs As Double) As Long
'��������� ������� ������������� ������������� ���
Public Declare Function X502_GetDacCoef Lib "x502api" (ByVal hnd As Long, ByVal ch As Long, ByRef k As Double, ByRef offs As Double) As Long

'����������� ����� ������ �� ����� ���.
Public Declare Function X502_AsyncOutDac Lib "x502api" (ByVal hnd As Long, ByVal ch As Long, ByVal data As Double, ByVal flags As Long) As Long
'����������� ����� ������ �� �������� ������.
Public Declare Function X502_AsyncOutDig Lib "x502api" (ByVal hnd As Long, ByVal Val As Long, ByVal msk As Long) As Long
'����������� ���� �������� � �������� ������.
Public Declare Function X502_AsyncInDig Lib "x502api" (ByVal hnd As Long, ByRef din As Long) As Long

Private Declare Function tmp_X502_AsyncGetAdcFrame Lib "x502api" Alias "X502_AsyncGetAdcFrame" (ByVal hnd As Long, ByVal flags As Long, ByVal tout As Long, ByRef data As Double) As Long

'���������� ���������� ������� �� ����/�����.
Public Declare Function X502_StreamsEnable Lib "x502api" (ByVal hnd As Long, ByVal streams As Long) As Long
'���������� ���������� ������� �� ����/�����.
Public Declare Function X502_StreamsDisable Lib "x502api" (ByVal hnd As Long, ByVal streams As Long) As Long
'�������� ��������, ����� ���������� ������ ���������
Public Declare Function X502_GetEnabledStreams Lib "x502api" (ByVal hnd As Long, ByRef streams As Long) As Long
'������ ���������� ������� �����/������.
Public Declare Function X502_StreamsStart Lib "x502api" (ByVal hnd As Long) As Long
'������� ���������� ������� �����/������.
Public Declare Function X502_StreamsStop Lib "x502api" (ByVal hnd As Long) As Long
'��������, ������� �� ���������� ����/�����
Public Declare Function X502_IsRunning Lib "x502api" (ByVal hnd As Long) As Long
'������ ������ ��� � �������� ������ �� ������.
Private Declare Function tmp_X502_Recv Lib "x502api" Alias "X502_Recv" (ByVal hnd As Long, ByRef buf As Long, ByVal size As Long, ByVal tout As Long) As Long
'�������� ��������� ������ ��� � �������� ������� � ������.
Private Declare Function tmp_X502_Send Lib "x502api" Alias "X502_Send" (ByVal hnd As Long, ByRef buf As Long, ByVal size As Long, ByVal tout As Long) As Long
'��������� �������� �������� ��� �� ������.
Private Declare Function tmp_X502_ProcessAdcData Lib "x502api" Alias "X502_ProcessAdcData" (ByVal hnd As Long, ByRef src As Long, ByRef dest As Double, ByRef size As Long, ByVal flags As Long) As Long
'��������� �������� �� ������ ������.
Private Declare Function tmp_X502_ProcessData Lib "x502api" Alias "X502_ProcessData" (ByVal hnd As Long, ByRef src As Long, ByVal flags As Long, ByRef adc_data As Double, ByRef adc_data_size As Long, ByRef din_data As Long, ByRef din_data_size As Long) As Long
'��������� �������� �� ������ ������ � ����������������� �������.
Private Declare Function tmp_X502_ProcessDataWithUserExt Lib "x502api" Alias "X502_ProcessDataWithUserExt" (ByVal hnd As Long, ByRef src As Long, ByVal size As Long, ByVal flags As Long, ByRef adc_data As Double, ByRef adc_data_size As Long, ByRef din_data As Long, ByRef din_data_size As Long, ByRef usr_data As Long, ByRef usr_data_size As Long) As Long
'���������� ������ ��� ������ � ������.
Private Declare Function tmp_X502_PrepareData Lib "x502api" Alias "X502_PrepareData" (ByVal hnd As Long, ByRef dac1 As Double, ByRef dac2 As Double, ByRef digout As Long, ByVal size As Long, ByVal flags As Long, ByRef out_buf As Long) As Long

'�������� ���������� �������� � ������ ������ �� ����.
Public Declare Function X502_GetRecvReadyCount Lib "x502api" (ByVal hnd As Long, ByRef rdy_cnt As Long) As Long
'�������� ������ ���������� ����� � ������ ������ �� �����.
Public Declare Function X502_GetSendReadyCount Lib "x502api" (ByVal hnd As Long, ByRef rdy_cnt As Long) As Long
'�������� ����� ���������� ���������� ����������� ������ ��� ���  ���������.
Public Declare Function X502_GetNextExpectedLchNum Lib "x502api" (ByVal hnd As Long, ByRef lch As Long) As Long
'������ ���������� ������ ���������� ������
Public Declare Function X502_PreloadStart Lib "x502api" (ByVal hnd As Long) As Long
'������ �������� ������������ ������� �� �����
Public Declare Function X502_OutCycleLoadStart Lib "x502api" (ByVal hnd As Long, ByVal size As Long) As Long
'��������� ����� ������������ ������������ ������� �� �����
Public Declare Function X502_OutCycleSetup Lib "x502api" (ByVal hnd As Long, ByVal flags As Long) As Long
'������� ������ ������������ �������
Public Declare Function X502_OutCycleStop Lib "x502api" (ByVal hnd As Long, ByVal flags As Long) As Long
'��������, ��������� �� ��������� ��� ������� ������������ �������
Public Declare Function X502_OutCycleCheckSetupDone Lib "x502api" (ByVal hnd As Long, ByRef done As Long) As Long
'������ ������ ������� ������
Public Declare Function X502_OutGetStatusFlags Lib "x502api" (ByVal hnd As Long, ByRef status As Long) As Long
'��������� ������� ������ � �������� ��� ����������� ����� ��� ������.
Public Declare Function X502_SetStreamBufSize Lib "x502api" (ByVal hnd As Long, ByVal dma_ch As Long, ByVal size As Long) As Long
'��������� ���� ���������� ��� �������� ������ �� DMA.
Public Declare Function X502_SetStreamStep Lib "x502api" (ByVal hnd As Long, ByVal dma_ch As Long, ByVal step As Long) As Long

'�������� �������� ����������� ���������� BlackFin.
Public Declare Function X502_BfLoadFirmware Lib "x502api" (ByVal hnd As Long, ByVal filename As String) As Long
'��������, ��������� �� �������� BlackFIn.
Public Declare Function X502_BfCheckFirmwareIsLoaded Lib "x502api" (ByVal hnd As Long, ByRef version As Long) As Long
'������ ����� ������ �� ������ ����������� ����������.
Private Declare Function tmp_X502_BfMemRead Lib "x502api" Alias "X502_BfMemRead" (ByVal hnd As Long, ByVal addr As Long, ByRef regs As Long, ByVal size As Long) As Long
'������ ����� ������ � ������ ����������� ����������.
Private Declare Function tmp_X502_BfMemWrite Lib "x502api" Alias "X502_BfMemWrite" (ByVal hnd As Long, ByVal addr As Long, ByRef regs As Long, ByVal size As Long) As Long
'�������� ����������� ������� ����������� ����������.
Private Declare Function tmp_X502_BfExecCmd Lib "x502api" Alias "X502_BfExecCmd" (ByVal hnd As Long, ByVal cmd_code As Integer, ByVal par As Long, ByRef snd_data As Long, ByVal snd_size As Long, ByRef rcv_data As Long, ByVal rcv_size As Long, ByVal tout As Long, ByRef recvd_size As Long) As Long

'������ ����� ������ �� Flash-������.
 Private Declare Function tmp_X502_FlashRead Lib "x502api" Alias "X502_FlashRead" (ByVal hnd As Long, ByVal addr As Long, ByRef data As Byte, ByVal size As Long) As Long
 '������ ����� ������ �� Flash-������ ������.
 Private Declare Function tmp_X502_FlashWrite Lib "x502api" Alias "X502_FlashWrite" (ByVal hnd As Long, ByVal addr As Long, ByRef data As Byte, ByVal size As Long) As Long
'�������� ����� �� Flash-������.
 Public Declare Function X502_FlashErase Lib "x502api" (ByVal hnd As Long, ByVal addr As Long, ByVal size As Long) As Long
 '���������� ������ � ���������������� ������� Flash-������.
 Public Declare Function X502_FlashWriteEnable Lib "x502api" (ByVal hnd As Long) As Long
 '������ ������ � ���������������� ������� Flash-������.
 Public Declare Function X502_FlashWriteDisable Lib "x502api" (ByVal hnd As Long) As Long
 
 '�������� ������ ����������.
Public Declare Function X502_GetLibraryVersion Lib "x502api" () As Long
'�������� ����������� �� �������� ������.
Public Declare Function X502_LedBlink Lib "x502api" (ByVal hnd As Long) As Long
'��������� ������������� ���������� �� ������� ������.
Public Declare Function X502_SetDigInPullup Lib "x502api" (ByVal hnd As Long, ByVal pullups As Long) As Long
'�������� ��������� ������� �������� �����������
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
