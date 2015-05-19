unit MainUnit;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls,
  l502api, x502api, e502api, X502_ProcessThread;

type
  TMainForm = class(TForm)
    btnRefreshDeviceList: TButton;
    cbbDevList: TComboBox;
    btnOpen: TButton;
    grpDevInfo: TGroupBox;
    chkDacPresent: TCheckBox;
    chkGalPresent: TCheckBox;
    chkDspPresent: TCheckBox;
    edtFpgaVer: TEdit;
    edtPldaVer: TEdit;
    lbl1: TLabel;
    lbl2: TLabel;
    btnAsyncDigIn: TButton;
    edtAsyncDigIn: TEdit;
    btnAsyncDigOut: TButton;
    edtAsyncDigOut: TEdit;
    grp1: TGroupBox;
    lbl3: TLabel;
    lbl4: TLabel;
    lbl5: TLabel;
    btnSetAdcFreq: TButton;
    edtAdcFreq: TEdit;
    edtAdcFreqLch: TEdit;
    edtDinFreq: TEdit;
    grp2: TGroupBox;
    lbl6: TLabel;
    lbl7: TLabel;
    lbl8: TLabel;
    lbl9: TLabel;
    lbl10: TLabel;
    cbbLCh1_Channel: TComboBox;
    cbbLCh1_Range: TComboBox;
    cbbLCh1_Mode: TComboBox;
    edtLCh1_Result: TEdit;
    cbbLCh2_Channel: TComboBox;
    cbbLCh2_Range: TComboBox;
    cbbLCh2_Mode: TComboBox;
    edtLCh2_Result: TEdit;
    cbbLCh3_Channel: TComboBox;
    cbbLCh3_Range: TComboBox;
    cbbLCh3_Mode: TComboBox;
    edtLCh3_Result: TEdit;
    chkSyncDin: TCheckBox;
    edtDin_Result: TEdit;
    cbbLChCnt: TComboBox;
    btnAsyncAdcFrame: TButton;
    btnStop: TButton;
    btnStart: TButton;
    btnAsyncDac1: TButton;
    edtAsyncDac1: TEdit;
    edtAsyncDac2: TEdit;
    btnAsyncDac2: TButton;
    GroupBox4: TGroupBox;
    Label10: TLabel;
    Label11: TLabel;
    cbbSyncMode: TComboBox;
    cbbSyncStartMode: TComboBox;
    chkEthPresent: TCheckBox;
    edtMcuVer: TEdit;
    lbl11: TLabel;
    btnOpenByIP: TButton;
    edtIpAddr: TEdit;
    procedure btnRefreshDeviceListClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure btnOpenClick(Sender: TObject);
    procedure btnAsyncDigInClick(Sender: TObject);
    procedure btnAsyncDigOutClick(Sender: TObject);
    procedure btnSetAdcFreqClick(Sender: TObject);
    procedure btnAsyncAdcFrameClick(Sender: TObject);
    procedure btnStartClick(Sender: TObject);
    procedure btnAsyncDac1Click(Sender: TObject);
    procedure btnAsyncDac2Click(Sender: TObject);
    procedure btnStopClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure chkSyncDinClick(Sender: TObject);
    procedure btnOpenByIPClick(Sender: TObject);
  private
    hnd : t_x502_hnd; //описатель модуля
    devrecList : array of t_x502_devrec;
    threadRunning : Boolean; //признак, запущен ли поток сбора данных
    thread: TX502_ProcessThread; //поток для синхронного ввода

    procedure updateControls();
    procedure freeDevList();
    procedure refreshDeviceList();
    procedure showDevInfo();
    procedure closeDevice();
    procedure OnThreadTerminate(par : TObject);
    function setAdcFreq() : LongInt;
    function setupParams() : LongInt;
    function setSyncDinStream() : LongInt;
    function parseIpAddr(addr : string) : LongWord;

    { Private declarations }
  public
    { Public declarations }
  end;

var
  MainForm: TMainForm;

implementation

{$R *.dfm}

procedure TMainForm.updateControls();
begin
  //кнопка обновить список доступна пока не открыли устройство
  btnRefreshDeviceList.Enabled := hnd=nil;
  cbbDevList.Enabled:= hnd=nil;

  if hnd = nil then
    btnOpen.Caption := 'Установить связь с модулем'
  else
    btnOpen.Caption := 'Разорвать связь с модулем';
  btnOpen.Enabled := (hnd<>nil) or (cbbDevList.ItemIndex >= 0);
  btnOpenByIP.Enabled := hnd=nil;
  edtIpAddr.Enabled := hnd=nil;

  chkSyncDin.Enabled := (hnd<>nil) and not threadRunning;

  btnStart.Enabled := (hnd<>nil) and not threadRunning;
  btnStop.Enabled  := (hnd<>nil) and threadRunning;

  btnSetAdcFreq.Enabled := (hnd<>nil) and not threadRunning;

  btnAsyncDigOut.Enabled := (hnd<>nil);
  btnAsyncDigIn.Enabled := (hnd<>nil);
  btnAsyncDac1.Enabled := (hnd<>nil) and chkDacPresent.Checked;
  btnAsyncDac2.Enabled := (hnd<>nil) and chkDacPresent.Checked;
  btnAsyncAdcFrame.Enabled := (hnd<>nil) and not threadRunning;
end;

procedure TMainForm.freeDevList();
begin
  if Length(devrecList) <> 0 then
  begin
    X502_FreeDevRecordList(devrecList, Length(devrecList));
    SetLength(devrecList, 0);
  end;
  cbbDevList.Items.Clear;
end;



{ Функция обновляет список серийных номеров подключенных устройств }
procedure TMainForm.refreshDeviceList();
var
  //указатель на массив серийных номеров, который выделим динамически
  res,i, old_index : LongInt;
  devcnt : LongWord;
  pci_devrecs, usb_devrecs : array of t_x502_devrec;
begin
  old_index := cbbDevList.ItemIndex;

  freeDevList;

  L502_GetDevRecordsList(pci_devrecs, 0, devcnt);
  if devcnt > 0 then
  begin
    //устанавливаем размер массива, чтобы его хватало для нужного кол-ва номеров
    SetLength(pci_devrecs, devcnt);
    //получаем серийные номера модулей
    res := L502_GetDevRecordsList(pci_devrecs, 0);
    if res > 0 then
      SetLength(pci_devrecs, res)
    else
      SetLength(pci_devrecs, 0);
  end;

  E502_UsbGetDevRecordsList(usb_devrecs, 0, devcnt);
  if devcnt > 0 then
  begin
    //устанавливаем размер массива, чтобы его хватало для нужного кол-ва номеров
    SetLength(usb_devrecs, devcnt);
    //получаем серийные номера модулей
    res := E502_UsbGetDevRecordsList(usb_devrecs, 0);
    if res > 0 then
      SetLength(usb_devrecs, res)
    else
      SetLength(usb_devrecs, 0);
  end;


  // В devrecList сохраняем оба массива записей
  SetLength(devrecList, Length(pci_devrecs) + Length(usb_devrecs));
  if Length(devrecList) > 0 then
  begin
    for i:=0 to Length(pci_devrecs)-1 do
      devrecList[i] := pci_devrecs[i];
    for i:=0 to Length(usb_devrecs)-1 do
      devrecList[i+Length(pci_devrecs)] := usb_devrecs[i];


    // добавляем все найденные записи в список ComboBox'а для выбора
    for i:=0 to Length(devrecList)-1 do
      cbbDevList.Items.Add(AnsiString(devrecList[i].devname) + ', ' + AnsiString(devrecList[i].serial));

    //если до обновления списка был выбран действительный индекс - восстанавливаем его
    if (old_index >= 0) and (old_index < Length(pci_devrecs)) then
      cbbDevList.ItemIndex := old_index
    else
      cbbDevList.ItemIndex := 0;
  end;


  updateControls;
end;



// Установка пераметров сбора модуля в соответствии с элементами GUI
function TMainForm.setupParams() : Integer;
var
  err : Integer;
  lch_cnt : LongWord;
const
  { таблица соответствия индексов в ComboBox и кодов режима измерения }
  mode_tbl: array[0..2] of LongWord = (X502_LCH_MODE_COMM, X502_LCH_MODE_DIFF, X502_LCH_MODE_ZERO);
  { таблица соответствия индексов в ComboBox и кодов диапазонов АЦП }
  range_tbl: array[0..5] of LongWord = (X502_ADC_RANGE_10, X502_ADC_RANGE_5, X502_ADC_RANGE_2,
                                        X502_ADC_RANGE_1, X502_ADC_RANGE_05, X502_ADC_RANGE_02);
  { таблица соответствия индексов в ComboBox и кодов источника синхронизации }
  f_sync_tbl: array[0..5] of LongWord = (X502_SYNC_INTERNAL, X502_SYNC_EXTERNAL_MASTER,
                                    X502_SYNC_DI_SYN1_RISE, X502_SYNC_DI_SYN2_RISE,
                                    X502_SYNC_DI_SYN1_FALL, X502_SYNC_DI_SYN2_FALL);
begin
  lch_cnt := cbbLChCnt.ItemIndex + 1;
  // Устанавливаем кол-во логических каналов
  err := X502_SetLChannelCount(hnd, lch_cnt);

  // Настраниваем таблицу логических каналов
  if err = X502_ERR_OK then
  begin
    err := X502_SetLChannel(hnd,0, cbbLCh1_Channel.ItemIndex,
            mode_tbl[cbbLCh1_Mode.ItemIndex], range_tbl[cbbLCh1_Range.ItemIndex],0);
  end;
  if (err = X502_ERR_OK) and (lch_cnt>=2) then
  begin
    err := X502_SetLChannel(hnd,1, cbbLCh2_Channel.ItemIndex,
            mode_tbl[cbbLCh2_Mode.ItemIndex], range_tbl[cbbLCh2_Range.ItemIndex],0);
  end;
  if (err = X502_ERR_OK) and (lch_cnt>=3) then
  begin
    err := X502_SetLChannel(hnd,2, cbbLCh3_Channel.ItemIndex,
            mode_tbl[cbbLCh3_Mode.ItemIndex], range_tbl[cbbLCh3_Range.ItemIndex],0);
  end;

  // Настраиваем источник частоты синхронизации и запуска сбора
	if err = X502_ERR_OK then
		err := X502_SetSyncMode(hnd, f_sync_tbl[cbbSyncMode.ItemIndex]);
  if err = X502_ERR_OK then
    err := X502_SetSyncStartMode(hnd, f_sync_tbl[cbbSyncStartMode.ItemIndex]);

  // настраиваем частоту сбора с АЦП
  if err = X502_ERR_OK then
    err := setAdcFreq();

  // Записываем настройки в модуль
  if err = X502_ERR_OK then
    err := X502_Configure(hnd, 0);

  setupParams:= err;
end;



procedure TMainForm.closeDevice();
begin
  if hnd<>nil then
  begin
    //если запущен поток сбора данных - то останавливаем его
    if threadRunning then
    begin
      thread.stop:=True;
      thread.WaitFor;
    end;
    // закрытие связи с модулем
    X502_Close(hnd);
    // освобождение описателя
    X502_Free(hnd);
    hnd := nil;
  end;
end;

//установка частоты АЦП в соответствии со значениями
//элементов управления, и обновление их в соответствии
//с реально установленными
function TMainForm.setAdcFreq() : Integer;
var f_acq, f_lch, f_din: Double;
    err : Integer;
begin
    f_acq := StrToFloat(edtAdcFreq.Text);
    f_lch := StrToFloat(edtAdcFreqLch.Text);
    f_din := StrToFloat(edtDinFreq.Text);
    // устанавливаем требуемую частоту сбора.
    err := X502_SetAdcFreq(hnd, f_acq, f_lch);
    if err = X502_ERR_OK then
    begin
      // обновляем значение индикатора, для отображения
      // реально установившейся частоты
      edtAdcFreq.Text := FloatToStr(f_acq);
      edtAdcFreqLch.Text := FloatToStr(f_lch);
      // Устанавливаем частоту синхронного сбора
      err := X502_SetDinFreq(hnd, f_din);
      if err = X502_ERR_OK then
        edtDinFreq.Text := FloatToStr(f_din);
    end;
    setAdcFreq:= err;
end;

function TMainForm.setSyncDinStream : LongInt;
var err : LongInt;
begin
    { разрешаем или запрещаем поток синхронного ввода
       в зависимости от состояния переключателя }
    if chkSyncDin.Checked then
        err := X502_StreamsEnable(hnd, X502_STREAM_DIN)
    else
        err := X502_StreamsDisable(hnd, X502_STREAM_DIN);

   setSyncDinStream:= err;
end;

procedure TMainForm.btnRefreshDeviceListClick(Sender: TObject);
begin
  refreshDeviceList;
end;

procedure TMainForm.FormCreate(Sender: TObject);
begin
  hnd := nil;
  refreshDeviceList;
end;


procedure TMainForm.showDevInfo();
var
  err : LongInt;
  info : t_x502_info;
begin
  //получаем информацию о устройстве
  err := X502_GetDevInfo(hnd,info);
  if err <> X502_ERR_OK then
  begin
    MessageDlg('Ошибка получения информации о устройстве: ' + X502_GetErrorString(err),
               mtError, [mbOK], 0);
    //при ошибке закрываем связь с устройством
    X502_Close(hnd);
    X502_Free(hnd);
    hnd := nil;
  end
  else
  begin
    //выводим проичитанную информацию на индикаторы
    chkDacPresent.Checked := (info.devflags and X502_DEVFLAGS_DAC_PRESENT) <> 0;
    chkGalPresent.Checked := (info.devflags and X502_DEVFLAGS_GAL_PRESENT) <> 0;
    chkDspPresent.Checked := (info.devflags and X502_DEVFLAGS_BF_PRESENT) <> 0;
    chkEthPresent.Checked := (info.devflags and X502_DEVFLAGS_IFACE_SUPPORT_ETH) <> 0;

    edtPldaVer.Text := IntToStr(info.plda_ver);
    edtFpgaVer.Text := IntToStr((info.fpga_ver shr 8) and $FF) + '.' +
                      IntToStr(info.fpga_ver and $FF);
    if info.mcu_firmware_ver <> 0 then
    begin
      edtMcuVer.Text := IntToStr((info.mcu_firmware_ver shr 24) and $FF) + '.' +
                        IntToStr((info.mcu_firmware_ver shr 16) and $FF) + '.' +
                        IntToStr((info.mcu_firmware_ver shr 8) and $FF) + '.' +
                        IntToStr((info.mcu_firmware_ver) and $FF);
    end
    else
    begin
      edtMcuVer.Text := '';
    end;
  end;
end;

procedure TMainForm.btnOpenClick(Sender: TObject);
var
  idx : integer;
  err : LongInt;
begin
  if hnd=nil then
  begin
    // получаем индекс выбранного модуля
    idx := cbbDevList.ItemIndex;
    if (idx >= 0) and (idx < Length(devrecList)) then
    begin
      //создаем описатель
      hnd := X502_Create();
      if hnd=nil then
        MessageDlg('Ошибка создания описателя модуля',mtError, [mbOK], 0)
      else
      begin
        // открываем модуль, соответствующий записи с выбранным индексом
        err := X502_OpenByDevRecord(hnd, devrecList[idx]);
        if err <> X502_ERR_OK then
        begin
          // Не смогли открыть => освобождаем описатель
          MessageDlg('Ошибка открытия модуля: ' + X502_GetErrorString(err),mtError, [mbOK], 0);
          X502_Free(hnd);
          hnd := nil;
        end
        else
        begin
          showDevInfo;
        end;
      end;
    end;
  end
  else
  begin
     //если устройство было открыто - закрываем
     closeDevice;
  end;

  updateControls;
end;

{ преобразование строки с ip-адресом в DWORD }
function TMainForm.parseIpAddr(addr : string) : LongWord;
var
  S: TStrings;
begin
  S := TStringList.Create;
  S.Delimiter := '.';
  S.DelimitedText := addr;

  // do preeliminary check. The IP4 Address string must consists of 4 parts. Less or more than that would be invalid values
  if S.Count<>4 then
  begin
    S.Free;
    raise Exception.Create('Invalid IP4 Address string');
  end;

  Result := (StrToInt(S[0]) shl 24) + (StrToInt(S[1]) shl 16) +
            (StrToInt(S[2]) shl 8)  + (StrToInt(S[3]));

  S.Free;
end;

procedure TMainForm.btnOpenByIPClick(Sender: TObject);
var
  devrec : t_x502_devrec;
  err : LongInt;
begin
  // Создаем запись, соответствующую заданному адресу
  err := E502_MakeDevRecordByIpAddr(devrec, parseIpAddr(edtIpAddr.Text), 0, 5000);
  if err <> X502_ERR_OK then
    MessageDlg('Ошибка создания записи: ' + X502_GetErrorString(err),mtError, [mbOK], 0)
  else
  begin
    // Создание описателя модуля
    hnd := X502_Create;
    if hnd=nil then
      MessageDlg('Ошибка создания описателя модуля',mtError, [mbOK], 0)
    else
    begin
      // устанавливаем связь по созданной записи
      err := X502_OpenByDevRecord(hnd, devrec);
      if err <> X502_ERR_OK then
      begin
        // Не смогли открыть => освобождаем описатель
        MessageDlg('Ошибка открытия модуля: ' + X502_GetErrorString(err),mtError, [mbOK], 0);
        X502_Free(hnd);
        hnd := nil;
      end
      else
      begin
        showDevInfo;
      end;
    end;

    X502_FreeDevRecordList(devrec);
  end;
  updateControls();
end;

procedure TMainForm.btnAsyncDigInClick(Sender: TObject);
var din: Cardinal;
var err: Integer;
begin
  if hnd<>nil then
  begin
    err := X502_AsyncInDig(hnd, din);
    if err<>X502_ERR_OK then
        MessageDlg('Ошибка асинхронного ввода с цифровых линий: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0)
    else
        edtAsyncDigIn.Text := IntToHex(din and $3FFFF, 5);
  end;                                                     
end;

procedure TMainForm.btnAsyncDigOutClick(Sender: TObject);
var dout: Cardinal;
var err: Integer;
begin
  if hnd<>nil then
  begin
    dout:= StrToInt(edtAsyncDigOut.Text);
    err := X502_AsyncOutDig(hnd, dout, 0);
    if err <> X502_ERR_OK then
       MessageDlg('Ошибка асинхронного вывода цифровых линий: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0)
  end;
end;

procedure TMainForm.btnSetAdcFreqClick(Sender: TObject);
var err : Integer;
begin
  { устанавливаем кол-во логических каналов, чтобы правильно
    расчитать частоту на канал }
  err := X502_SetLChannelCount(hnd, cbbLChCnt.ItemIndex+1);
  if err = X502_ERR_OK then
    err := setAdcFreq();
  if err <> X502_ERR_OK then
    MessageDlg('Ошибка установки частоты: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0)
end;

procedure TMainForm.btnAsyncAdcFrameClick(Sender: TObject);
var err : Integer;
    adc_data : array of double;
    lch_cnt : LongWord;

begin
    if hnd <> nil then
    begin
      // устанавливаем параметры модуля
      err := setupParams();
      if err = X502_ERR_OK then
        err := X502_GetLChannelCount(hnd, lch_cnt);

      if err = X502_ERR_OK then
      begin
        // устанавливаем размер массива равным количеству логических каналов
        SetLength(adc_data, lch_cnt);
        err := X502_AsyncGetAdcFrame(hnd, X502_PROC_FLAGS_VOLT, 1000, adc_data);

        if err = X502_ERR_OK then
        begin
          //выводим результат
          edtLCh1_Result.Text := FloatToStrF(adc_data[0], ffFixed, 4, 8);
          if lch_cnt >= 2 then
            edtLCh2_Result.Text := FloatToStrF(adc_data[1], ffFixed, 4, 8)
          else
            edtLCh2_Result.Text := '';

          if lch_cnt >= 3 then
            edtLCh3_Result.Text := FloatToStrF(adc_data[2], ffFixed, 4, 8)
          else
            edtLCh3_Result.Text := '';

        end
        else
        begin
           MessageDlg('Ошибка приема кадра АЦП: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0);
        end
      end
      else
      begin
        MessageDlg('Ошибка настройки параметров АЦП: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0);
      end;
    end;
end;

procedure TMainForm.btnStartClick(Sender: TObject);
var
  err : LongInt;
begin
  //настраиваем все параметры в соответствии с элементами управления
  err := setupParams();
  //разрешаем синхронный ввод АЦП 
  if err = X502_ERR_OK then
     err := X502_StreamsEnable(hnd, X502_STREAM_ADC);

  // разрешаем синхронный ввод с цифровых линий в зависимости от переключателя
  if err = X502_ERR_OK then
    err := setSyncDinStream();

  if err <> X502_ERR_OK then
  begin
    MessageDlg('Ошибка настройки параметров модуля: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0);
  end
  else
  begin
    if thread <> nil then
    begin
      FreeAndNil(thread);
    end;

    thread := TX502_ProcessThread.Create(True);
    thread.hnd := hnd;
    SetLength(thread.LChEdits, 3);
    thread.LChEdits[0] := edtLCh1_Result;
    thread.LChEdits[1] := edtLCh2_Result;
    thread.LChEdits[2] := edtLCh3_Result;
    thread.dinResEdit  := edtDin_Result;

    edtLCh1_Result.Text:= '';
    edtLCh2_Result.Text:= '';
    edtLCh3_Result.Text:= '';
    edtDin_Result.Text:= '';
    { устанавливаем функцию на событие завершения потока (в частности
    чтобы отследить, если поток завершился сам из-за ошибки при сборе
    данных) }
    thread.OnTerminate := OnThreadTerminate;
    thread.Resume; //для Delphi 2010 и выше рекомендуется использовать Start
    threadRunning := True;

    updateControls;
  end;

end;

//функция, вызываемая по завершению потока сбора данных
//разрешает старт нового, устанавливает thread_run
procedure TMainForm.OnThreadTerminate(par : TObject);
begin
    if thread.err <> X502_ERR_OK then
    begin
        MessageDlg('Сбор данных завершен с ошибкой: ' + X502_GetErrorString(thread.err),
                  mtError, [mbOK], 0);
    end;

    threadRunning := false; 
    updateControls();
end;

procedure TMainForm.btnAsyncDac1Click(Sender: TObject);
var
  err : LongInt;
  val : Double;
begin
  if hnd<>nil then
  begin
    val := StrToFloat(edtAsyncDac1.Text);;
    err := X502_AsyncOutDac(hnd, X502_DAC_CH1, val, X502_DAC_FLAGS_CALIBR or
                                                        X502_DAC_FLAGS_VOLT);
    if err <> X502_ERR_OK then
    begin
       MessageDlg('Ошибка вывода на ЦАП: ' + X502_GetErrorString(err),
                  mtError, [mbOK], 0);
    end;
  end;
end;

procedure TMainForm.btnAsyncDac2Click(Sender: TObject);
var
  err : LongInt;
  val : Double;
begin
  if hnd<>nil then
  begin
    val := StrToFloat(edtAsyncDac2.Text);;
    err := X502_AsyncOutDac(hnd, X502_DAC_CH2, val, X502_DAC_FLAGS_CALIBR or
                                                        X502_DAC_FLAGS_VOLT);
    if err <> X502_ERR_OK then
    begin
       MessageDlg('Ошибка вывода на ЦАП: ' + X502_GetErrorString(err),
                  mtError, [mbOK], 0);
    end;
  end;
end;

procedure TMainForm.btnStopClick(Sender: TObject);
begin
    // устанавливаем запрос на завершение потока
    if threadRunning then
        thread.stop:=True;
    btnStop.Enabled:= False;
end;

procedure TMainForm.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  closeDevice;
  freeDevList;
  if thread <> nil then
    FreeAndNil(thread);
end;

procedure TMainForm.chkSyncDinClick(Sender: TObject);
var err : LongInt;
begin
  if hnd<>nil then
  begin
    err := setSyncDinStream();
    if err <> X502_ERR_OK then
    begin
      MessageDlg('Ошибка разрешения/запрета синхронного ввода: ' + X502_GetErrorString(err),
                  mtError, [mbOK], 0);      
    end;
  end;
end;




end.
