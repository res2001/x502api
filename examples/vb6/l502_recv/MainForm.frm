VERSION 5.00
Begin VB.Form MainForm 
   Caption         =   "L502. ������"
   ClientHeight    =   3240
   ClientLeft      =   120
   ClientTop       =   450
   ClientWidth     =   6810
   LinkTopic       =   "�����1"
   ScaleHeight     =   3240
   ScaleWidth      =   6810
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox ResultList 
      Height          =   1425
      ItemData        =   "MainForm.frx":0000
      Left            =   3720
      List            =   "MainForm.frx":0002
      TabIndex        =   3
      Top             =   1440
      Width           =   2775
   End
   Begin VB.CommandButton GetDevlistBtn 
      Caption         =   "�������� ������ ���������"
      Height          =   375
      Left            =   480
      TabIndex        =   2
      Top             =   480
      Width           =   2775
   End
   Begin VB.ComboBox DevListBox 
      Height          =   315
      Left            =   3720
      TabIndex        =   1
      Text            =   "DevListBox"
      Top             =   480
      Width           =   2655
   End
   Begin VB.CommandButton GetSamples 
      Caption         =   "����� ������"
      Height          =   375
      Left            =   480
      TabIndex        =   0
      Top             =   1080
      Width           =   2775
   End
   Begin VB.Label �����2 
      Caption         =   "��������� (������� �� �������)"
      Height          =   255
      Left            =   3720
      TabIndex        =   5
      Top             =   1080
      Width           =   2775
   End
   Begin VB.Label �����1 
      Caption         =   "������ ���������"
      Height          =   255
      Left            =   3720
      TabIndex        =   4
      Top             =   120
      Width           =   2775
   End
End
Attribute VB_Name = "MainForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub Form_Load()
    GetDevlistBtn_Click
End Sub

Private Sub GetDevlistBtn_Click()
   Dim ret As Long
   Dim serials() As String
      
    ret = TEST_SERIAL_SIZE
    DevListBox.Clear
    '�������� ������ �������� ������� ���������.
    '������ ������� ��������������� ������ �������
    '������������ �������� ���� < 0 - ��� ������, ����� - ���-�� ��������� ���������
    ret = L502_GetSerialList(serials, 0)
    If ret < 0 Then
        MsgBox ("������ ��������� ������ ��������: " + L502_GetErrorString(ret))
    Else
        If ret > 0 Then
            For i = 0 To ret - 1
                DevListBox.AddItem (serials(i))
            Next
            DevListBox.ListIndex = 0
        End If
    End If
    
End Sub


Private Sub GetSamples_Click()
    Dim err As Long
    Dim hnd As Long
    Dim adc_freq, frame_freq As Double
    
    '���������� ����� ��� ������ �� ������� ������
    Const ch_points = 1000
    '���������� ������������ �������
    Const ch_cnt = 3
    
    
    ResultList.Clear
       
    '�������� ��������� ������ ��� ������
    hnd = L502_Create()
    '������������� ����� � ������� �� ��� ��������� ������ (������� �� DevListBox)
    '���� ����� ������ ���������� ����� � ������ �������, �� ���������� � �������� ������ �������� ""
    err = L502_Open(hnd, DevListBox.Text)
    If err <> L502_ERR_OK Then
        MsgBox ("������ �������� ������: " + L502_GetErrorString(err))
    Else
        '������������� ���-�� ������������ �������
        err = L502_SetLChannelCount(hnd, ch_cnt)
        '��� ������� �� ������������ ������� ������ ����� ������, �����, ��������
        '��� ������� ���������� ������ ����� � ���. 10�, 3-�� � ����� ������ ����������� 1 �, � 32-�� � ����� ������ ����������� 0.2 �
        If err = L502_ERR_OK Then
            err = L502_SetLChannel(hnd, 0, 0, L502_LCH_MODE_DIFF, L502_ADC_RANGE_10, 0)
        End If
        If err = L502_ERR_OK Then
            err = L502_SetLChannel(hnd, 1, 2, L502_LCH_MODE_COMM, L502_ADC_RANGE_1, 0)
        End If
        If err = L502_ERR_OK Then
            err = L502_SetLChannel(hnd, 2, 31, L502_LCH_MODE_COMM, L502_ADC_RANGE_02, 0)
        End If
        
        If err = L502_ERR_OK Then
           '��������� �������. ������ ������� ��� = 2 ���, � ����. ������������ �������� �� ����� (2 ���/lch_cnt)
            f_adc = 2000000
            f_frame = 0
            err = L502_SetAdcFreq(hnd, f_adc, f_frame)
        End If
        
        
        If err = L502_ERR_OK Then
            '�������� ������������� ������������ � ������
            err = L502_Configure(hnd, 0)
        End If
        
        If err = L502_ERR_OK Then
            '�� ���������� ������� ��������� ������ ����� ���
            err = L502_StreamsEnable(hnd, L502_STREAM_ADC)
        End If
        
        If err <> L502_ERR_OK Then
             MsgBox ("������ ��������� ������: " + L502_GetErrorString(err))
        Else
           Dim recv_size As Long
           
           '��������� ��� ������� �� ������� ������ ch_points ��������,
           '�.�. ����� ����� ���� ch_cnt * ch_points
           '���������� CLng ��� �������� � Long - ����� vb ����������
           '���������� � Integer � ����� ������������ ��� ���-�� �����
           '������ 32*1024
           recv_size = CLng(ch_cnt) * ch_points
                     
                   
            '������ ����� ������
            err = L502_StreamsStart(hnd)
            If err <> L502_ERR_OK Then
                MsgBox ("������ ������� �����: " + L502_GetErrorString(err))
            Else
                Dim recv_wrds() As Long
                Dim recv_vals() As Double
                Dim recvd_cnt As Long
                
                ReDim recv_wrds(0 To recv_size - 1)
                ReDim recv_vals(0 To recv_size - 1)
                
                '����� ���� � ������� ������
                recvd_cnt = L502_Recv(hnd, recv_wrds, recv_size, 5000)
                '�������� ������ ���� ������������� ������
                If recvd_cnt < 0 Then
                    err = recvd_cnt
                    MsgBox ("������ ��������� ������: " + L502_GetErrorString(err))
                Else
                    '������� ������ ��� �����������
                    If recvd_cnt < recv_size Then
                        err = L502_ERR_RECV_INSUFFICIENT_WORDS
                        MsgBox ("������� ������������ ������")
                    End If
                End If
                
                If err = L502_ERR_OK Then
                    '��������� ������ - ������� � ������
                    err = L502_ProcessAdcData(hnd, recv_wrds, recv_vals, recvd_cnt, L502_PROC_FLAGS_VOLT)
                    If err <> L502_ERR_OK Then
                         MsgBox ("������ ��������� ������: " + L502_GetErrorString(err))
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
                L502_StreamsStop (hnd)
            End If
        End If
              
                            
                          
        '��������� ���������� � �������
        err = L502_Close(hnd)
    End If
    
     '����������� ������ ���������� L502_Create()
    L502_Free (hnd)
End Sub
