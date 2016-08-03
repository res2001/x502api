VERSION 5.00
Begin VB.Form MainForm 
   Caption         =   "L502. Пример"
   ClientHeight    =   3240
   ClientLeft      =   120
   ClientTop       =   450
   ClientWidth     =   6810
   LinkTopic       =   "Форма1"
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
      Caption         =   "Получить список устройств"
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
      Caption         =   "Прием данных"
      Height          =   375
      Left            =   480
      TabIndex        =   0
      Top             =   1080
      Width           =   2775
   End
   Begin VB.Label Метка2 
      Caption         =   "Результат (среднее по каналам)"
      Height          =   255
      Left            =   3720
      TabIndex        =   5
      Top             =   1080
      Width           =   2775
   End
   Begin VB.Label Метка1 
      Caption         =   "Список устройств"
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
    'Получаем список серийных номеров устройств.
    'размер массива устанавливается внутри функции
    'возвращаемое значение если < 0 - код ошибки, иначе - кол-во найденных устройств
    ret = L502_GetSerialList(serials, 0)
    If ret < 0 Then
        MsgBox ("Ошибка получения списка устройтв: " + L502_GetErrorString(ret))
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
    
    'количество точек для приема по каждому каналу
    Const ch_points = 1000
    'количество опрашиваемых каналов
    Const ch_cnt = 3
    
    
    ResultList.Clear
       
    'создание описателя модуля для работу
    hnd = L502_Create()
    'устанавливаем связь с модулем по его серийному номеру (берется из DevListBox)
    'если нужно просто установить связь с первым модулем, то достаточно в качестве номера передать ""
    err = L502_Open(hnd, DevListBox.Text)
    If err <> L502_ERR_OK Then
        MsgBox ("Ошибка открытия модуля: " + L502_GetErrorString(err))
    Else
        'устанавливаем кол-во опрашиваемых каналов
        err = L502_SetLChannelCount(hnd, ch_cnt)
        'для каждого из опрашиваемых каналов задаем номер канала, режим, диапазон
        'для примера используем первый канал в диф. 10В, 3-ий с общей землей поддиапазон 1 В, и 32-ой с общей землей поддиапазон 0.2 В
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
           'настройка частоты. Задаем частоту АЦП = 2 МГц, с макс. получившейся частотой на канал (2 МГц/lch_cnt)
            f_adc = 2000000
            f_frame = 0
            err = L502_SetAdcFreq(hnd, f_adc, f_frame)
        End If
        
        
        If err = L502_ERR_OK Then
            'передаем установленную конфигурацию в модуль
            err = L502_Configure(hnd, 0)
        End If
        
        If err = L502_ERR_OK Then
            'из синхронных потоков разрешаем только поток АЦП
            err = L502_StreamsEnable(hnd, L502_STREAM_ADC)
        End If
        
        If err <> L502_ERR_OK Then
             MsgBox ("Ошибка настройки модуля: " + L502_GetErrorString(err))
        Else
           Dim recv_size As Long
           
           'принимаем для примера по каждому каналу ch_points отсчетов,
           'т.е. общее число слов ch_cnt * ch_points
           'Используем CLng для перевода в Long - иначе vb использует
           'арифметику с Integer и будет переполнение при кол-ве точек
           'больше 32*1024
           recv_size = CLng(ch_cnt) * ch_points
                     
                   
            'запуск сбора данных
            err = L502_StreamsStart(hnd)
            If err <> L502_ERR_OK Then
                MsgBox ("Ошибка запуска сбора: " + L502_GetErrorString(err))
            Else
                Dim recv_wrds() As Long
                Dim recv_vals() As Double
                Dim recvd_cnt As Long
                
                ReDim recv_wrds(0 To recv_size - 1)
                ReDim recv_vals(0 To recv_size - 1)
                
                'прием слов в формате модуля
                recvd_cnt = L502_Recv(hnd, recv_wrds, recv_size, 5000)
                'значение меньше нуля соответствует ошибке
                If recvd_cnt < 0 Then
                    err = recvd_cnt
                    MsgBox ("Ошибка настройки модуля: " + L502_GetErrorString(err))
                Else
                    'приняли меньше чем запрашивали
                    If recvd_cnt < recv_size Then
                        err = L502_ERR_RECV_INSUFFICIENT_WORDS
                        MsgBox ("Принято недостаточно данных")
                    End If
                End If
                
                If err = L502_ERR_OK Then
                    'обработка данных - перевод в Вольты
                    err = L502_ProcessAdcData(hnd, recv_wrds, recv_vals, recvd_cnt, L502_PROC_FLAGS_VOLT)
                    If err <> L502_ERR_OK Then
                         MsgBox ("Ошибка обработки данных: " + L502_GetErrorString(err))
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
                L502_StreamsStop (hnd)
            End If
        End If
              
                            
                          
        'закрываем соединение с модулем
        err = L502_Close(hnd)
    End If
    
     'освобождаем память выделенную L502_Create()
    L502_Free (hnd)
End Sub
