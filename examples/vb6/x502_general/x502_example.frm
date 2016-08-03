VERSION 5.00
Begin VB.Form MainForm 
   Caption         =   "X502 Example"
   ClientHeight    =   3405
   ClientLeft      =   120
   ClientTop       =   450
   ClientWidth     =   7395
   LinkTopic       =   "Form1"
   ScaleHeight     =   3405
   ScaleWidth      =   7395
   StartUpPosition =   3  'Windows Default
   Begin VB.CheckBox checkUseIP 
      Caption         =   "����������� �� IP ������"
      Height          =   375
      Left            =   3480
      TabIndex        =   6
      Top             =   840
      Width           =   2775
   End
   Begin VB.TextBox edtIpAddr 
      Height          =   375
      Left            =   3480
      TabIndex        =   5
      Text            =   "192.168.12.233"
      Top             =   1200
      Width           =   2775
   End
   Begin VB.ListBox ResultList 
      Height          =   1425
      ItemData        =   "x502_example.frx":0000
      Left            =   240
      List            =   "x502_example.frx":0002
      TabIndex        =   3
      Top             =   1800
      Width           =   2775
   End
   Begin VB.CommandButton btnRecvData 
      Caption         =   "������� ���� ������"
      Height          =   375
      Left            =   240
      TabIndex        =   2
      Top             =   960
      Width           =   2655
   End
   Begin VB.ComboBox cbbDevList 
      Height          =   315
      Left            =   3480
      TabIndex        =   1
      Top             =   360
      Width           =   2895
   End
   Begin VB.CommandButton btnRefreshDeviceList 
      Caption         =   "�������� ������ ���������"
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   360
      Width           =   2655
   End
   Begin VB.Label �����2 
      Caption         =   "��������� (������� �� �������)"
      Height          =   255
      Left            =   240
      TabIndex        =   4
      Top             =   1440
      Width           =   2775
   End
End
Attribute VB_Name = "MainForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

'������, ��������������� ��������� ����������� �� USB � PCI
Dim devlist() As t_x502_devrec
'�������, ��� ������ devlist ������������
Dim devlist_valid As Boolean

Private Type ipBytes
    B1 As Byte
    B2 As Byte
    B3 As Byte
    B4 As Byte
End Type

Private Type ipLong
    Val As Long
End Type

' Convert a dotted quad IP string to a Long.
Public Function IP2Long(IP As String) As Long
    ReDim parse(0 To 3) As String
    Dim B As ipBytes
    Dim L As ipLong
    
    parse = Split(IP, ".")
        
    B.B4 = Val(parse(0))
    B.B3 = Val(parse(1))
    B.B2 = Val(parse(2))
    B.B1 = Val(parse(3))
    LSet L = B
    IP2Long = L.Val
End Function


'�������� ������� � ����������� � �������� ��������
Private Sub freeDevlist()
    If devlist_valid Then
        Dim devcnt As Long
        devcnt = UBound(devlist) + 1
        If devcnt <> 0 Then
            Call X502_FreeDevRecordList(devlist, devcnt)
        End If
        ReDim devlist(0)
        
        cbbDevList.Clear
        devlist_valid = False
     End If
End Sub

Private Sub refreshDeviceList()
    Dim usbrec_cnt As Long, pcirec_cnt As Long
    Dim usbreclist() As t_x502_devrec
    Dim pcireclist() As t_x502_devrec
    Dim devcnt As Long
    
    Dim old_idx As Long
    
    old_idx = cbbDevList.ListIndex
    
    
    
    Call freeDevlist
    
    '�������� ������� ������� � ��������� ����������� ��� USB � PCI
    usbrec_cnt = E502_UsbGetDevRecordsList(usbreclist, 0)
    If usbrec_cnt < 0 Then
        MsgBox ("������ ��������� ������ USB-��������: " + X502_GetErrorString(usbrec_cnt))
        usbrec_cnt = 0
    End If
    pcirec_cnt = L502_GetDevRecordsList(pcireclist, 0)
    If (pcirec_cnt < 0) Then
        MsgBox ("������ ��������� ������ PCI-��������: " + X502_GetErrorString(pcirec_cnt))
        pcirec_cnt = 0
    End If
        
    '������� ����� ������ �� ������ ���� ����
    devcnt = usbrec_cnt + pcirec_cnt
    
    If devcnt > 0 Then
        ReDim devlist(0 To devcnt - 1)
        
        For i = 0 To usbrec_cnt - 1
            devlist(i) = usbreclist(i)
        Next
        For i = 0 To pcirec_cnt - 1
            devlist(i + usbrec_cnt) = pcireclist(i)
        Next
        
        '��������� ��� ���������� � combobox
        For i = 0 To devcnt - 1
            cbbDevList.AddItem (X502_StrConvert(devlist(i).DevName) + ", " + X502_StrConvert(devlist(i).serial))
        Next
        
        devlist_valid = True
    
        '��������������� ��������� �� ������ ������� � ������, ���� ����� ����� ����������,
        '����� �������� ������ ����������
        
        If old_idx >= devcnt Or old_idx < 0 Then
            old_idx = 0
        End If
        cbbDevList.ListIndex = old_idx
    End If
End Sub

Private Sub adcSampleBlock(ByRef devrec As t_x502_devrec)
    Dim err As Long
    Dim hnd As Long
    Dim adc_freq, frame_freq As Double
    
    '���������� ����� ��� ������ �� ������� ������
    Const ch_points = 1000
    '���������� ������������ �������
    Const ch_cnt = 3
    
    
    ResultList.Clear
       
    '�������� ��������� ������ ��� ������
    hnd = X502_Create()
    '������������� ����� � �������, ��������� ���������� ������ � ������
    err = X502_OpenByDevRecord(hnd, devrec)
    
    If err <> X502_ERR_OK Then
        MsgBox ("������ �������� ������: " + X502_GetErrorString(err))
    Else
        '������������� ���-�� ������������ �������
        err = X502_SetLChannelCount(hnd, ch_cnt)
        '��� ������� �� ������������ ������� ������ ����� ������, �����, ��������
        '��� ������� ���������� ������ ����� � ���. 10�, 3-�� � ����� ������ ����������� 1 �, � 32-�� � ����� ������ ����������� 0.2 �
        If err = X502_ERR_OK Then
            err = X502_SetLChannel(hnd, 0, 0, X502_LCH_MODE_DIFF, X502_ADC_RANGE_10, 0)
        End If
        If err = X502_ERR_OK Then
            err = X502_SetLChannel(hnd, 1, 2, X502_LCH_MODE_COMM, X502_ADC_RANGE_1, 0)
        End If
        If err = X502_ERR_OK Then
            err = X502_SetLChannel(hnd, 2, 31, X502_LCH_MODE_COMM, X502_ADC_RANGE_02, 0)
        End If
        
        If err = X502_ERR_OK Then
           '��������� �������. ������ ������� ��� = 2 ���, � ����. ������������ �������� �� ����� (2 ���/lch_cnt)
            f_adc = 2000000
            f_frame = 0
            err = X502_SetAdcFreq(hnd, f_adc, f_frame)
        End If
        
        
        If err = X502_ERR_OK Then
            '�������� ������������� ������������ � ������
            err = X502_Configure(hnd, 0)
        End If
        
        If err = X502_ERR_OK Then
            '�� ���������� ������� ��������� ������ ����� ���
            err = X502_StreamsEnable(hnd, X502_STREAM_ADC)
        End If
        
        If err <> X502_ERR_OK Then
             MsgBox ("������ ��������� ������: " + X502_GetErrorString(err))
        Else
           Dim recv_size As Long
           
           '��������� ��� ������� �� ������� ������ ch_points ��������,
           '�.�. ����� ����� ���� ch_cnt * ch_points
           '���������� CLng ��� �������� � Long - ����� vb ����������
           '���������� � Integer � ����� ������������ ��� ���-�� �����
           '������ 32*1024
           recv_size = CLng(ch_cnt) * ch_points
                     
                   
            '������ ����� ������
            err = X502_StreamsStart(hnd)
            If err <> X502_ERR_OK Then
                MsgBox ("������ ������� �����: " + X502_GetErrorString(err))
            Else
                Dim recv_wrds() As Long
                Dim recv_vals() As Double
                Dim recvd_cnt As Long
                
                ReDim recv_wrds(0 To recv_size - 1)
                ReDim recv_vals(0 To recv_size - 1)
                
                '����� ���� � ������� ������
                recvd_cnt = X502_Recv(hnd, recv_wrds, recv_size, 5000)
                '�������� ������ ���� ������������� ������
                If recvd_cnt < 0 Then
                    err = recvd_cnt
                    MsgBox ("������ ������ ������ �� ������: " + X502_GetErrorString(err))
                Else
                    '������� ������ ��� �����������
                    If recvd_cnt < recv_size Then
                        err = X502_ERR_RECV_INSUFFICIENT_WORDS
                        MsgBox ("������� ������������ ������")
                    End If
                End If
                
                If err = X502_ERR_OK Then
                    '��������� ������ - ������� � ������
                    err = X502_ProcessAdcData(hnd, recv_wrds, recv_vals, recvd_cnt, X502_PROC_FLAGS_VOLT)
                    If err <> X502_ERR_OK Then
                         MsgBox ("������ ��������� ������: " + X502_GetErrorString(err))
                    Else
                        '��� ������� ��� ������� ������ ������������ ������� �� �������� ������
                        For ch = 0 To ch_cnt - 1
                            Dim avg As Double
                            avg = 0
                            '������� � ���������� ������� ���� �� ������� ���������, �.�. ������
                            '������ ������ ������� ������, ������ ������ ������� ....   ������ ������ ������� ...
                            '�.�. ������ i-�� ������� ������ �� ������������ ��� i*ch_cnt + ch
                            For i = 0 To recvd_cnt / ch_cnt - 1
                                avg = avg + recv_vals(i * ch_cnt + ch)
                            Next
                            avg = avg / (recvd_cnt / ch_cnt)
                            
                            '��������� ��������� � ������
                            ResultList.AddItem (FormatNumber(avg, 6) + " �")
                        Next
                    End If
                End If
                
                '������� ������ �����
                X502_StreamsStop (hnd)
            End If
        End If
        
        
        
        X502_Close (hnd)
    End If
    X502_Free (hnd)
End Sub


Private Sub btnRecvData_Click()
    '���� �������, �� ������������� ���������� �� Ethernet �� IP-������
    If checkUseIP.Value = vbChecked Then
        Dim ip_addr As Long
        Dim devrec As t_x502_devrec
        Dim err As Long
        
        ip_addr = IP2Long(edtIpAddr.Text)
        err = E502_MakeDevRecordByIpAddr(devrec, ip_addr, 0, 5000)
        If err = X502_ERR_OK Then
            Call adcSampleBlock(devrec)
            '����������� ������� ������
            '������ ������� - ������� ��� X502_FreeDevRecordList ��� ������ ��������
            Call X502_FreeDevRecord(devrec)
        End If
    Else
        '����� ���������� ��������� ��������� �� ����������������
        Dim devidx As Long
                
        devidx = cbbDevList.ListIndex
        If devlist_valid Then
            If devidx >= 0 And devidx <= UBound(devlist) Then
                Call adcSampleBlock(devlist(devidx))
            Else
                MsgBox ("�� ������� ���������")
            End If
        Else
            MsgBox ("�� �������� �� ������ ���������")
        End If
    End If
End Sub


Private Sub btnRefreshDeviceList_Click()
   Call refreshDeviceList
End Sub




Private Sub Form_Load()
    devlist_valid = False
    Call refreshDeviceList
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call freeDevlist
End Sub
