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
    hnd : t_x502_hnd; //��������� ������
    devrecList : array of t_x502_devrec;
    threadRunning : Boolean; //�������, ������� �� ����� ����� ������
    thread: TX502_ProcessThread; //����� ��� ����������� �����

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
  //������ �������� ������ �������� ���� �� ������� ����������
  btnRefreshDeviceList.Enabled := hnd=nil;
  cbbDevList.Enabled:= hnd=nil;

  if hnd = nil then
    btnOpen.Caption := '���������� ����� � �������'
  else
    btnOpen.Caption := '��������� ����� � �������';
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



{ ������� ��������� ������ �������� ������� ������������ ��������� }
procedure TMainForm.refreshDeviceList();
var
  //��������� �� ������ �������� �������, ������� ������� �����������
  res,i, old_index : LongInt;
  devcnt : LongWord;
  pci_devrecs, usb_devrecs : array of t_x502_devrec;
begin
  old_index := cbbDevList.ItemIndex;

  freeDevList;

  L502_GetDevRecordsList(pci_devrecs, 0, devcnt);
  if devcnt > 0 then
  begin
    //������������� ������ �������, ����� ��� ������� ��� ������� ���-�� �������
    SetLength(pci_devrecs, devcnt);
    //�������� �������� ������ �������
    res := L502_GetDevRecordsList(pci_devrecs, 0);
    if res > 0 then
      SetLength(pci_devrecs, res)
    else
      SetLength(pci_devrecs, 0);
  end;

  E502_UsbGetDevRecordsList(usb_devrecs, 0, devcnt);
  if devcnt > 0 then
  begin
    //������������� ������ �������, ����� ��� ������� ��� ������� ���-�� �������
    SetLength(usb_devrecs, devcnt);
    //�������� �������� ������ �������
    res := E502_UsbGetDevRecordsList(usb_devrecs, 0);
    if res > 0 then
      SetLength(usb_devrecs, res)
    else
      SetLength(usb_devrecs, 0);
  end;


  // � devrecList ��������� ��� ������� �������
  SetLength(devrecList, Length(pci_devrecs) + Length(usb_devrecs));
  if Length(devrecList) > 0 then
  begin
    for i:=0 to Length(pci_devrecs)-1 do
      devrecList[i] := pci_devrecs[i];
    for i:=0 to Length(usb_devrecs)-1 do
      devrecList[i+Length(pci_devrecs)] := usb_devrecs[i];


    // ��������� ��� ��������� ������ � ������ ComboBox'� ��� ������
    for i:=0 to Length(devrecList)-1 do
      cbbDevList.Items.Add(AnsiString(devrecList[i].devname) + ', ' + AnsiString(devrecList[i].serial));

    //���� �� ���������� ������ ��� ������ �������������� ������ - ��������������� ���
    if (old_index >= 0) and (old_index < Length(pci_devrecs)) then
      cbbDevList.ItemIndex := old_index
    else
      cbbDevList.ItemIndex := 0;
  end;


  updateControls;
end;



// ��������� ���������� ����� ������ � ������������ � ���������� GUI
function TMainForm.setupParams() : Integer;
var
  err : Integer;
  lch_cnt : LongWord;
const
  { ������� ������������ �������� � ComboBox � ����� ������ ��������� }
  mode_tbl: array[0..2] of LongWord = (X502_LCH_MODE_COMM, X502_LCH_MODE_DIFF, X502_LCH_MODE_ZERO);
  { ������� ������������ �������� � ComboBox � ����� ���������� ��� }
  range_tbl: array[0..5] of LongWord = (X502_ADC_RANGE_10, X502_ADC_RANGE_5, X502_ADC_RANGE_2,
                                        X502_ADC_RANGE_1, X502_ADC_RANGE_05, X502_ADC_RANGE_02);
  { ������� ������������ �������� � ComboBox � ����� ��������� ������������� }
  f_sync_tbl: array[0..5] of LongWord = (X502_SYNC_INTERNAL, X502_SYNC_EXTERNAL_MASTER,
                                    X502_SYNC_DI_SYN1_RISE, X502_SYNC_DI_SYN2_RISE,
                                    X502_SYNC_DI_SYN1_FALL, X502_SYNC_DI_SYN2_FALL);
begin
  lch_cnt := cbbLChCnt.ItemIndex + 1;
  // ������������� ���-�� ���������� �������
  err := X502_SetLChannelCount(hnd, lch_cnt);

  // ������������ ������� ���������� �������
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

  // ����������� �������� ������� ������������� � ������� �����
	if err = X502_ERR_OK then
		err := X502_SetSyncMode(hnd, f_sync_tbl[cbbSyncMode.ItemIndex]);
  if err = X502_ERR_OK then
    err := X502_SetSyncStartMode(hnd, f_sync_tbl[cbbSyncStartMode.ItemIndex]);

  // ����������� ������� ����� � ���
  if err = X502_ERR_OK then
    err := setAdcFreq();

  // ���������� ��������� � ������
  if err = X502_ERR_OK then
    err := X502_Configure(hnd, 0);

  setupParams:= err;
end;



procedure TMainForm.closeDevice();
begin
  if hnd<>nil then
  begin
    //���� ������� ����� ����� ������ - �� ������������� ���
    if threadRunning then
    begin
      thread.stop:=True;
      thread.WaitFor;
    end;
    // �������� ����� � �������
    X502_Close(hnd);
    // ������������ ���������
    X502_Free(hnd);
    hnd := nil;
  end;
end;

//��������� ������� ��� � ������������ �� ����������
//��������� ����������, � ���������� �� � ������������
//� ������� ��������������
function TMainForm.setAdcFreq() : Integer;
var f_acq, f_lch, f_din: Double;
    err : Integer;
begin
    f_acq := StrToFloat(edtAdcFreq.Text);
    f_lch := StrToFloat(edtAdcFreqLch.Text);
    f_din := StrToFloat(edtDinFreq.Text);
    // ������������� ��������� ������� �����.
    err := X502_SetAdcFreq(hnd, f_acq, f_lch);
    if err = X502_ERR_OK then
    begin
      // ��������� �������� ����������, ��� �����������
      // ������� �������������� �������
      edtAdcFreq.Text := FloatToStr(f_acq);
      edtAdcFreqLch.Text := FloatToStr(f_lch);
      // ������������� ������� ����������� �����
      err := X502_SetDinFreq(hnd, f_din);
      if err = X502_ERR_OK then
        edtDinFreq.Text := FloatToStr(f_din);
    end;
    setAdcFreq:= err;
end;

function TMainForm.setSyncDinStream : LongInt;
var err : LongInt;
begin
    { ��������� ��� ��������� ����� ����������� �����
       � ����������� �� ��������� ������������� }
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
  //�������� ���������� � ����������
  err := X502_GetDevInfo(hnd,info);
  if err <> X502_ERR_OK then
  begin
    MessageDlg('������ ��������� ���������� � ����������: ' + X502_GetErrorString(err),
               mtError, [mbOK], 0);
    //��� ������ ��������� ����� � �����������
    X502_Close(hnd);
    X502_Free(hnd);
    hnd := nil;
  end
  else
  begin
    //������� ������������ ���������� �� ����������
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
    // �������� ������ ���������� ������
    idx := cbbDevList.ItemIndex;
    if (idx >= 0) and (idx < Length(devrecList)) then
    begin
      //������� ���������
      hnd := X502_Create();
      if hnd=nil then
        MessageDlg('������ �������� ��������� ������',mtError, [mbOK], 0)
      else
      begin
        // ��������� ������, ��������������� ������ � ��������� ��������
        err := X502_OpenByDevRecord(hnd, devrecList[idx]);
        if err <> X502_ERR_OK then
        begin
          // �� ������ ������� => ����������� ���������
          MessageDlg('������ �������� ������: ' + X502_GetErrorString(err),mtError, [mbOK], 0);
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
     //���� ���������� ���� ������� - ���������
     closeDevice;
  end;

  updateControls;
end;

{ �������������� ������ � ip-������� � DWORD }
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
  // ������� ������, ��������������� ��������� ������
  err := E502_MakeDevRecordByIpAddr(devrec, parseIpAddr(edtIpAddr.Text), 0, 5000);
  if err <> X502_ERR_OK then
    MessageDlg('������ �������� ������: ' + X502_GetErrorString(err),mtError, [mbOK], 0)
  else
  begin
    // �������� ��������� ������
    hnd := X502_Create;
    if hnd=nil then
      MessageDlg('������ �������� ��������� ������',mtError, [mbOK], 0)
    else
    begin
      // ������������� ����� �� ��������� ������
      err := X502_OpenByDevRecord(hnd, devrec);
      if err <> X502_ERR_OK then
      begin
        // �� ������ ������� => ����������� ���������
        MessageDlg('������ �������� ������: ' + X502_GetErrorString(err),mtError, [mbOK], 0);
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
        MessageDlg('������ ������������ ����� � �������� �����: ' + X502_GetErrorString(err),
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
       MessageDlg('������ ������������ ������ �������� �����: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0)
  end;
end;

procedure TMainForm.btnSetAdcFreqClick(Sender: TObject);
var err : Integer;
begin
  { ������������� ���-�� ���������� �������, ����� ���������
    ��������� ������� �� ����� }
  err := X502_SetLChannelCount(hnd, cbbLChCnt.ItemIndex+1);
  if err = X502_ERR_OK then
    err := setAdcFreq();
  if err <> X502_ERR_OK then
    MessageDlg('������ ��������� �������: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0)
end;

procedure TMainForm.btnAsyncAdcFrameClick(Sender: TObject);
var err : Integer;
    adc_data : array of double;
    lch_cnt : LongWord;

begin
    if hnd <> nil then
    begin
      // ������������� ��������� ������
      err := setupParams();
      if err = X502_ERR_OK then
        err := X502_GetLChannelCount(hnd, lch_cnt);

      if err = X502_ERR_OK then
      begin
        // ������������� ������ ������� ������ ���������� ���������� �������
        SetLength(adc_data, lch_cnt);
        err := X502_AsyncGetAdcFrame(hnd, X502_PROC_FLAGS_VOLT, 1000, adc_data);

        if err = X502_ERR_OK then
        begin
          //������� ���������
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
           MessageDlg('������ ������ ����� ���: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0);
        end
      end
      else
      begin
        MessageDlg('������ ��������� ���������� ���: ' + X502_GetErrorString(err),
                     mtError, [mbOK], 0);
      end;
    end;
end;

procedure TMainForm.btnStartClick(Sender: TObject);
var
  err : LongInt;
begin
  //����������� ��� ��������� � ������������ � ���������� ����������
  err := setupParams();
  //��������� ���������� ���� ��� 
  if err = X502_ERR_OK then
     err := X502_StreamsEnable(hnd, X502_STREAM_ADC);

  // ��������� ���������� ���� � �������� ����� � ����������� �� �������������
  if err = X502_ERR_OK then
    err := setSyncDinStream();

  if err <> X502_ERR_OK then
  begin
    MessageDlg('������ ��������� ���������� ������: ' + X502_GetErrorString(err),
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
    { ������������� ������� �� ������� ���������� ������ (� ���������
    ����� ���������, ���� ����� ���������� ��� ��-�� ������ ��� �����
    ������) }
    thread.OnTerminate := OnThreadTerminate;
    thread.Resume; //��� Delphi 2010 � ���� ������������� ������������ Start
    threadRunning := True;

    updateControls;
  end;

end;

//�������, ���������� �� ���������� ������ ����� ������
//��������� ����� ������, ������������� thread_run
procedure TMainForm.OnThreadTerminate(par : TObject);
begin
    if thread.err <> X502_ERR_OK then
    begin
        MessageDlg('���� ������ �������� � �������: ' + X502_GetErrorString(thread.err),
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
       MessageDlg('������ ������ �� ���: ' + X502_GetErrorString(err),
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
       MessageDlg('������ ������ �� ���: ' + X502_GetErrorString(err),
                  mtError, [mbOK], 0);
    end;
  end;
end;

procedure TMainForm.btnStopClick(Sender: TObject);
begin
    // ������������� ������ �� ���������� ������
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
      MessageDlg('������ ����������/������� ����������� �����: ' + X502_GetErrorString(err),
                  mtError, [mbOK], 0);      
    end;
  end;
end;




end.
