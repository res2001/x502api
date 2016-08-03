Attribute VB_Name = "l502api"
' модуль для объявления констант, структур и функций, специфичных для модуля L502


Private Declare Function tmp_L502_GetSerialList Lib "l502api" Alias "L502_GetSerialList" (ByVal serials As Long, ByVal size As Long, ByVal flags As Long, ByRef devcnt As Long) As Long

Public Declare Function L502_Open Lib "l502api" (ByVal hnd As Long, ByVal serial As String) As Long

Private Declare Function tmp_L502_GetDevRecordsList Lib "l502api" Alias "L502_GetDevRecordsList" (ByRef list As t_x502_devrec, ByVal size As Long, ByVal flags As Long, ByRef devcnt As Long) As Long

Public Declare Function L502_GetDriverVersion Lib "l502api" (ByVal hnd As Long, ByRef ver As Long) As Long

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
            tmp = GlobalAlloc(0, X502_SERIAL_SIZE * cnt)
            If tmp = 0 Then
                err = X502_ERR_MEMORY_ALLOC
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
    L502_GetSerialList = err
End Function

Public Function L502_GetDevRecordsList(ByRef list() As t_x502_devrec, ByVal flags As Long) As Long
    Dim ret_cnt As Long
    Dim err As Long
    Dim devcnt As Long
    Dim tmp As t_x502_devrec
    Dim dummy As Long
    
    ret_cnt = tmp_L502_GetDevRecordsList(tmp, 0, flags, devcnt)
    If ret_cnt < 0 Then
        err = ret_cnt
        ReDim list(0)
    Else
        If devcnt = 0 Then
            ReDim list(0)
        Else
            ReDim list(0 To devcnt - 1)
            ret_cnt = tmp_L502_GetDevRecordsList(list(0), devcnt, flags, dummy)
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
    L502_GetDevRecordsList = err
End Function
