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
      Caption         =   "Подключение по IP адресу"
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
      Caption         =   "Принять блок данных"
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
      Caption         =   "Обновить список устройств"
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   360
      Width           =   2655
   End
   Begin VB.Label Метка2 
      Caption         =   "Результат (среднее по каналам)"
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

'записи, соответствующие найденным устройствам по USB и PCI
Dim devlist() As t_x502_devrec
'признак, что список devlist действителен
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


'удаление записей о устройствах с очисткой ресурсов
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
    
    'Получаем списоки записей о найденных устройствах для USB и PCI
    usbrec_cnt = E502_UsbGetDevRecordsList(usbreclist, 0)
    If usbrec_cnt < 0 Then
        MsgBox ("Ошибка получения списка USB-устройтв: " + X502_GetErrorString(usbrec_cnt))
        usbrec_cnt = 0
    End If
    pcirec_cnt = L502_GetDevRecordsList(pcireclist, 0)
    If (pcirec_cnt < 0) Then
        MsgBox ("Ошибка получения списка PCI-устройтв: " + X502_GetErrorString(pcirec_cnt))
        pcirec_cnt = 0
    End If
        
    'создаем общий список на основе этих двух
    devcnt = usbrec_cnt + pcirec_cnt
    
    If devcnt > 0 Then
        ReDim devlist(0 To devcnt - 1)
        
        For i = 0 To usbrec_cnt - 1
            devlist(i) = usbreclist(i)
        Next
        For i = 0 To pcirec_cnt - 1
            devlist(i + usbrec_cnt) = pcireclist(i)
        Next
        
        'добавляем все устройства в combobox
        For i = 0 To devcnt - 1
            cbbDevList.AddItem (X502_StrConvert(devlist(i).DevName) + ", " + X502_StrConvert(devlist(i).serial))
        Next
        
        devlist_valid = True
    
        'восстанавливаем выбранный по номеру элемент в списке, если такой номер сохранился,
        'иначе выбираем первое устройство
        
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
    
    'количество точек для приема по каждому каналу
    Const ch_points = 1000
    'количество опрашиваемых каналов
    Const ch_cnt = 3
    
    
    ResultList.Clear
       
    'создание описателя модуля для работу
    hnd = X502_Create()
    'устанавливаем связь с модулем, используя переданную запись о модуле
    err = X502_OpenByDevRecord(hnd, devrec)
    
    If err <> X502_ERR_OK Then
        MsgBox ("Ошибка открытия модуля: " + X502_GetErrorString(err))
    Else
        'устанавливаем кол-во опрашиваемых каналов
        err = X502_SetLChannelCount(hnd, ch_cnt)
        'для каждого из опрашиваемых каналов задаем номер канала, режим, диапазон
        'для примера используем первый канал в диф. 10В, 3-ий с общей землей поддиапазон 1 В, и 32-ой с общей землей поддиапазон 0.2 В
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
           'настройка частоты. Задаем частоту АЦП = 2 МГц, с макс. получившейся частотой на канал (2 МГц/lch_cnt)
            f_adc = 2000000
            f_frame = 0
            err = X502_SetAdcFreq(hnd, f_adc, f_frame)
        End If
        
        
        If err = X502_ERR_OK Then
            'передаем установленную конфигурацию в модуль
            err = X502_Configure(hnd, 0)
        End If
        
        If err = X502_ERR_OK Then
            'из синхронных потоков разрешаем только поток АЦП
            err = X502_StreamsEnable(hnd, X502_STREAM_ADC)
        End If
        
        If err <> X502_ERR_OK Then
             MsgBox ("Ошибка настройки модуля: " + X502_GetErrorString(err))
        Else
           Dim recv_size As Long
           
           'принимаем для примера по каждому каналу ch_points отсчетов,
           'т.е. общее число слов ch_cnt * ch_points
           'Используем CLng для перевода в Long - иначе vb использует
           'арифметику с Integer и будет переполнение при кол-ве точек
           'больше 32*1024
           recv_size = CLng(ch_cnt) * ch_points
                     
                   
            'запуск сбора данных
            err = X502_StreamsStart(hnd)
            If err <> X502_ERR_OK Then
                MsgBox ("Ошибка запуска сбора: " + X502_GetErrorString(err))
            Else
                Dim recv_wrds() As Long
                Dim recv_vals() As Double
                Dim recvd_cnt As Long
                
                ReDim recv_wrds(0 To recv_size - 1)
                ReDim recv_vals(0 To recv_size - 1)
                
                'прием слов в формате модуля
                recvd_cnt = X502_Recv(hnd, recv_wrds, recv_size, 5000)
                'значение меньше нуля соответствует ошибке
                If recvd_cnt < 0 Then
                    err = recvd_cnt
                    MsgBox ("Ошибка приема данных от модуля: " + X502_GetErrorString(err))
                Else
                    'приняли меньше чем запрашивали
                    If recvd_cnt < recv_size Then
                        err = X502_ERR_RECV_INSUFFICIENT_WORDS
                        MsgBox ("Принято недостаточно данных")
                    End If
                End If
                
                If err = X502_ERR_OK Then
                    'обработка данных - перевод в Вольты
                    err = X502_ProcessAdcData(hnd, recv_wrds, recv_vals, recvd_cnt, X502_PROC_FLAGS_VOLT)
                    If err <> X502_ERR_OK Then
                         MsgBox ("Ошибка обработки данных: " + X502_GetErrorString(err))
                    Else
                        'для примера для каждого канала рассчитываем среднее по принятым точкам
                        For ch = 0 To ch_cnt - 1
                            Dim avg As Double
                            avg = 0
                            'отсчеты в полученном массиве идут по порядку узмерения, т.е. сперва
                            'первый отсчет первого канала, первый остчет второго ....   второй отсчет первого ...
                            'т.е. индекс i-го отсчета канала ср определяется как i*ch_cnt + ch
                            For i = 0 To recvd_cnt / ch_cnt - 1
                                avg = avg + recv_vals(i * ch_cnt + ch)
                            Next
                            avg = avg / (recvd_cnt / ch_cnt)
                            
                            'добавляем результат в список
                            ResultList.AddItem (FormatNumber(avg, 6) + " В")
                        Next
                    End If
                End If
                
                'останов потока сбора
                X502_StreamsStop (hnd)
            End If
        End If
        
        
        
        X502_Close (hnd)
    End If
    X502_Free (hnd)
End Sub


Private Sub btnRecvData_Click()
    'если выбрано, то устанавливаем соединение по Ethernet по IP-адресу
    If checkUseIP.Value = vbChecked Then
        Dim ip_addr As Long
        Dim devrec As t_x502_devrec
        Dim err As Long
        
        ip_addr = IP2Long(edtIpAddr.Text)
        err = E502_MakeDevRecordByIpAddr(devrec, ip_addr, 0, 5000)
        If err = X502_ERR_OK Then
            Call adcSampleBlock(devrec)
            'освобождаем ресурсы записи
            'данная фукнция - обертка над X502_FreeDevRecordList для одного элемента
            Call X502_FreeDevRecord(devrec)
        End If
    Else
        'иначе используем выбранное устойство из автоопределенных
        Dim devidx As Long
                
        devidx = cbbDevList.ListIndex
        If devlist_valid Then
            If devidx >= 0 And devidx <= UBound(devlist) Then
                Call adcSampleBlock(devlist(devidx))
            Else
                MsgBox ("Не выбрано устойство")
            End If
        Else
            MsgBox ("Не найденно ни одного устойства")
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
