Attribute VB_Name = "e502api"
' модуль для объявления констант, структур и функций, специфичных для модуля E502

'События поиска сетевых сервисов
Public Enum t_e502_eth_svc_event
    E502_ETH_SVC_EVENT_NONE = 0     'Ни одного события не произошло
    E502_ETH_SVC_EVENT_ADD = 1      'Обнаружено появление нового сетевого сервиса
    E502_ETH_SVC_EVENT_REMOVE = 2   'Обнаружено исчезновение ранее доступного сетевого сервиса
    E502_ETH_SVC_EVENT_CHANGED = 3  'Изменение параметров ранее обнаруженного сетевого сервиса
End Enum



Private Declare Function tmp_E502_UsbGetSerialList Lib "e502api" Alias "E502_UsbGetSerialList" (ByVal serials As Long, ByVal size As Long, ByVal flags As Long, ByRef devcnt As Long) As Long

Public Declare Function E502_OpenUsb Lib "e502api" (ByVal hnd As Long, ByVal serial As String) As Long
Public Declare Function E502_OpenByIpAddr Lib "e502api" (ByVal hnd As Long, ByVal ip_addr As Long, ByVal flags As Long, ByVal tout As Long) As Long

Private Declare Function tmp_E502_UsbGetDevRecordsList Lib "e502api" Alias "E502_UsbGetDevRecordsList" (ByRef list As t_x502_devrec, ByVal size As Long, ByVal flags As Long, ByRef devcnt As Long) As Long

Public Declare Function E502_MakeDevRecordByIpAddr Lib "e502api" (ByRef devrec As t_x502_devrec, ByVal ip_addr As Long, ByVal flags As Long, ByVal tout As Long) As Long
Public Declare Function E502_EthDevRecordSetCmdPort Lib "e502api" (ByRef devrec As t_x502_devrec, ByVal cmd_port As Integer) As Long
Public Declare Function E502_EthDevRecordSetDataPort Lib "e502api" (ByRef devrec As t_x502_devrec, ByVal data_port As Integer) As Long
Public Declare Function E502_MakeDevRecordByEthSvc Lib "e502api" (ByRef devrec As t_x502_devrec, ByVal svc As Long, ByVal flags As Long, ByVal tout As Long) As Long

Public Declare Function E502_GetIpAddr Lib "e502api" (ByVal hnd As Long, ByRef ip_addr As Long) As Long


Public Declare Function E502_EthConfigCreate Lib "e502api" () As Long
Public Declare Function E502_EthConfigFree Lib "e502api" (ByVal cfg As Long) As Long
Public Declare Function E502_EthConfigRead Lib "e502api" (ByVal hnd As Long, ByVal cfg As Long) As Long
Public Declare Function E502_EthConfigWrite Lib "e502api" (ByVal hnd As Long, ByVal cfg As Long, ByVal passwd As String) As Long
Public Declare Function E502_EthConfigCopy Lib "e502api" (ByVal src_cfg As Long, ByVal dst_cfg As Long) As Long
Public Declare Function E502_EthConfigGetEnabled Lib "e502api" (ByVal cfg As Long, ByRef en As Long) As Long
Public Declare Function E502_EthConfigSetEnabled Lib "e502api" (ByVal cfg As Long, ByVal en As Long) As Long
Public Declare Function E502_EthConfigGetAutoIPEnabled Lib "e502api" (ByVal cfg As Long, ByRef en As Long) As Long
Public Declare Function E502_EthConfigSetAutoIPEnabled Lib "e502api" (ByVal cfg As Long, ByVal en As Long) As Long
Public Declare Function E502_EthConfigGetUserMACEnabled Lib "e502api" (ByVal cfg As Long, ByRef en As Long) As Long
Public Declare Function E502_EthConfigSetUserMACEnabled Lib "e502api" (ByVal cfg As Long, ByVal en As Long) As Long
Public Declare Function E502_EthConfigGetIPv4Addr Lib "e502api" (ByVal cfg As Long, ByRef ip_addr As Long) As Long
Public Declare Function E502_EthConfigSetIPv4Addr Lib "e502api" (ByVal cfg As Long, ByVal ip_addr As Long) As Long
Public Declare Function E502_EthConfigGetIPv4Mask Lib "e502api" (ByVal cfg As Long, ByRef mask As Long) As Long
Public Declare Function E502_EthConfigSetIPv4Mask Lib "e502api" (ByVal cfg As Long, ByVal mask As Long) As Long
Public Declare Function E502_EthConfigGetIPv4Gate Lib "e502api" (ByVal cfg As Long, ByRef gate As Long) As Long
Public Declare Function E502_EthConfigSetIPv4Gate Lib "e502api" (ByVal cfg As Long, ByVal gate As Long) As Long
Private Declare Function tmp_E502_EthConfigGetUserMac Lib "e502api" Alias "EthConfigGetUserMac" (ByVal cfg As Long, ByRef mac As Byte) As Long
Private Declare Function tmp_E502_EthConfigSetUserMac Lib "e502api" Alias "E502_EthConfigSetUserMac" (ByVal cfg As Long, ByRef mac As Byte) As Long
Private Declare Function tmp_E502_EthConfigGetFactoryMac Lib "e502api" Alias "E502_EthConfigGetFactoryMac" (ByVal cfg As Long, ByRef mac As Byte) As Long
Public Declare Function tmp_E502_EthConfigGetInstanceName Lib "e502api" Alias "E502_EthConfigGetInstanceName" (ByVal cfg As Long, ByRef name As Byte) As Long
Public Declare Function E502_EthConfigSetInstanceName Lib "e502api" (ByVal cfg As Long, ByVal name As String) As Long
Public Declare Function E502_EthConfigSetNewPassword Lib "e502api" (ByVal cfg As Long, ByVal new_passwd As String) As Long


Public Declare Function E502_SwitchToBootloader Lib "e502api" (ByVal hnd As Long) As Long
Public Declare Function E502_ReloadFPGA Lib "e502api" (ByVal hnd As Long) As Long
Private Declare Function tmp_E502_CortexExecCmd Lib "e502api" Alias "E502_CortexExecCmd" (ByVal hnd As Long, ByVal cmd_code As Long, ByVal par As Long, ByRef snd_data As Byte, ByVal snd_size As Long, ByRef rcv_data As Byte, ByVal rcv_size As Long, ByVal tout As Long, ByRef recvd_size As Long) As Long


Public Declare Function E502_EthSvcBrowseStart Lib "e502api" (ByVal pcontext As Long, ByVal flags As Long) As Long
Public Declare Function E502_EthSvcBrowseGetEvent Lib "e502api" (ByVal context As Long, ByVal svc As Long, ByRef svcevent As Long, ByRef flags As Long, ByVal tout As Long) As Long
Public Declare Function E502_EthSvcBrowseStop Lib "e502api" (ByVal context As Long) As Long
Public Declare Function E502_EthSvcRecordFree Lib "e502api" (ByVal svc As Long) As Long

Private Declare Function tmp_E502_EthSvcRecordGetInstanceName Lib "e502api" Alias "E502_EthSvcRecordGetInstanceName" (ByVal svc As Long, ByRef name As Byte) As Long
Public Declare Function E502_EthSvcRecordGetDevSerial Lib "e502api" (ByVal svc As Long, ByVal serial As String) As Long
Public Declare Function E502_EthSvcRecordResolveIPv4Addr Lib "e502api" (ByVal svc As Long, ByRef addr As Long, ByVal tout As Long) As Long
Public Declare Function E502_EthSvcRecordIsSameInstance Lib "e502api" (ByVal svc1 As Long, ByVal svc2 As Long) As Long

                                                     
Private Declare Function GlobalAlloc Lib "kernel32" (ByVal wFlags As Long, ByVal dwBytes As Long) As Long
Private Declare Function GlobalFree Lib "kernel32" (ByVal hMem As Long) As Long
Private Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (Destination As Any, Source As Any, ByVal Length As Long)
                                                   

                                                     
                                                     
Public Function E502_EthConfigGetUserMac(ByVal cfg As Long, ByRef mac() As Byte) As Long
    E502_EthConfigGetUserMac = tmp_E502_EthConfigGetUserMac(cfg, mac(0))
End Function
Public Function E502_EthConfigSetUserMac(ByVal cfg As Long, ByRef mac() As Byte) As Long
    E502_EthConfigSetUserMac = tmp_E502_EthConfigSetUserMac(cfg, mac(0))
End Function
Public Function E502_EthConfigGetFactoryMac(ByVal cfg As Long, ByRef mac() As Byte) As Long
    E502_EthConfigGetFactoryMac = tmp_E502_EthConfigGetFactoryMac(cfg, mac(0))
End Function



Public Function E502_EthConfigGetInstanceName(ByVal cfg As Long, ByRef name As String) As Long
    Dim str_arr(0 To X502_INSTANCE_NAME_SIZE) As Byte
    Dim err As Long
    err = tmp_E502_EthConfigGetInstanceName(cfg, str_arr(0))
    'текущая реализация обрабатывает только ASCII символы
    'корректно сделать преобразование из UTF-8 через MultiByteToWideChar
    If err = X502_ERR_OK Then
        name = StrConv(str_arr, vbUnicode)
    End If
    E502_EthConfigGetInstanceName = err
End Function

Public Function E502_EthSvcRecordGetInstanceName(ByVal svc As Long, ByRef name As String) As Long
    Dim str_arr(0 To X502_INSTANCE_NAME_SIZE) As Byte
    Dim err As Long
    err = tmp_E502_EthSvcRecordGetInstanceName(svc, str_arr(0))
    'текущая реализация обрабатывает только ASCII символы
    'корректно сделать преобразование из UTF-8 через MultiByteToWideChar
    If err = X502_ERR_OK Then
        name = StrConv(str_arr, vbUnicode)
    End If
    E502_EthConfigGetInstanceName = err
End Function


Public Function E502_CortexExecCmd(ByVal hnd As Long, ByVal cmd_code As Long, ByVal par As Long, ByRef snd_data() As Byte, ByVal snd_size As Long, ByRef rcv_data() As Byte, ByVal rcv_size As Long, ByVal tout As Long, ByRef recvd_size As Long) As Long
    E502_CortexExecCmd = tmp_E502_CortexExecCmd(hnd, cmd_code, par, snd_data(0), snd_size, rcv_data(0), rcv_size, tout, recvd_size)
End Function


Public Function E502_UsbGetSerialList(ByRef serials() As String, ByVal flags As Long) As Long
    Dim tmp As Long
    Dim cnt As Long
    Dim ret_cnt As Long
    Dim dummy As Long
    Dim err As Long
    
    ret_cnt = 0
    tmp = 0
    
    ret_cnt = tmp_E502_UsbGetSerialList(tmp, 0, flags, cnt)
    If ret_cnt < 0 Then
        err = ret_cnt
        ReDim serials(0)
    Else
        If cnt = 0 Then
            ReDim serials(0)
        Else
            tmp = GlobalAlloc(0, X502_SERIAL_SIZE * cnt)
            If tmp = 0 Then
                err = X502_ERR_MEMORY_ALLOC
            Else
                ret_cnt = tmp_E502_UsbGetSerialList(tmp, cnt, flags, dummy)
                If ret_cnt < 0 Then
                   err = ret_cnt
                   ReDim serials(0)
                Else
                    If ret_cnt = 0 Then
                        ReDim serials(0)
                    Else
                        ReDim serials(0 To ret_cnt - 1)
                        For i = 0 To ret_cnt - 1
                            serials(i) = X502_StrFromPtr(tmp + i * X502_SERIAL_SIZE)
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
    E502_UsbGetSerialList = err
End Function

Public Function E502_UsbGetDevRecordsList(ByRef list() As t_x502_devrec, ByVal flags As Long) As Long
    Dim ret_cnt As Long
    Dim err As Long
    Dim devcnt As Long
    Dim tmp As t_x502_devrec
    Dim dummy As Long
    
    ret_cnt = tmp_E502_UsbGetDevRecordsList(tmp, 0, flags, devcnt)
    If ret_cnt < 0 Then
        err = ret_cnt
        ReDim list(0)
    Else
        If devcnt = 0 Then
            ReDim list(0)
        Else
            ReDim list(0 To devcnt - 1)
            ret_cnt = tmp_E502_UsbGetDevRecordsList(list(0), devcnt, flags, dummy)
            If ret_cnt < 0 Then
               err = ret_cnt
               ReDim list(0)
            Else
                If ret_cnt = 0 Then
                    ReDim list(0)
                ElseIf ret_cnt <> devcnt Then
                    ReDim Preserve list(0 To ret_cnt - 1)
                End If
            End If
       End If
    End If
    
    If err = L502_ERR_OK Then
        err = ret_cnt
    End If
    E502_UsbGetDevRecordsList = err
End Function
