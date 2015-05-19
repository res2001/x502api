unit X502_ProcessThread;

interface
uses Classes, Math, SyncObjs,StdCtrls,SysUtils, x502api;
  const RECV_BUF_SIZE = 8*1024*1024;
  const RECV_TOUT     = 250;

type TX502_ProcessThread = class(TThread)
  public
    LChEdits : array of TEdit;
    dinResEdit : TEdit;
    hnd: t_x502_hnd; //��������� ������

    err : LongInt; //��� ������ ��� ���������� ������ �����
    stop : Boolean; //������ �� ������� (��������������� �� ��������� ������)

    constructor Create(SuspendCreate : Boolean);
    destructor Free();

  private
     { Private declarations }
    adcData : array [0..RECV_BUF_SIZE-1] of double;
    dinData : array [0..RECV_BUF_SIZE-1] of LongWord;
    adcSize, dinSize, firstLch : LongWord;


    procedure updateData;
  protected
    procedure Execute; override;
  end;
implementation


  constructor TX502_ProcessThread.Create(SuspendCreate : Boolean);
  begin
     Inherited Create(SuspendCreate);
     stop:=False;
     hnd:=Nil;
     err:=X502_ERR_OK;
  end;

  destructor TX502_ProcessThread.Free();
  begin
      Inherited Free();
  end;

  { ���������� ����������� ����� ������������ ���������� ���������.
   ����� ������ ����������� ������ ����� Syncronize, ������� �����
   ��� ������� � ��������� VCL �� �� ��������� ������ }
  procedure TX502_ProcessThread.updateData;
  var
    lch_cnt, i : LongWord;
    err : LongInt;
  begin
    err := X502_GetLChannelCount(hnd, lch_cnt);
    if err=X502_ERR_OK then
    begin
      // ������������� � ����������� �������� ������� ������� �� �������
      for  i:=0 to Min(lch_cnt, adcSize) do
        LChEdits[(firstLch + i) mod lch_cnt].Text := FloatToStrF(adcData[i], ffFixed, 4, 8);

      { ���� ���� ������ ��������� ������, �� ������������� ���������
            � ������������ � ������ ��������� }
      if dinSize <> 0 then
        dinResEdit.Text := IntToHex(dinData[0] and $3FFFF, 5)
      else
        dinResEdit.Text := '';

    end;
  end;

  procedure TX502_ProcessThread.Execute;
  var
    stoperr, rcv_size : LongInt;
    rcv_buf : array of LongWord;
  begin
    setlength(rcv_buf, RECV_BUF_SIZE);
    // ��������� ���������� ������
    err:= X502_StreamsStart(hnd);
    if err = X502_ERR_OK then
    begin
      { ��������� ����� ���� �� ���������� ������ ���
        �� ����� ������� �� ������� �� ��������� ���������� }
      while not stop and (err = X502_ERR_OK) do
      begin
        //��������� ������ ����������� �����
        rcv_size := X502_Recv(hnd, rcv_buf, RECV_BUF_SIZE, RECV_TOUT);
        // �������� ������ ���� �������� ������...
        if rcv_size < 0 then
            err := rcv_size
        else if rcv_size>0 then
        begin
          //���� ������ ���� - ������ ������� ����� ������
          dinSize:=RECV_BUF_SIZE;
          adcSize:=RECV_BUF_SIZE;
          { �������� ����� ���. �������, ��������������� �������
            ������� ���, ��� ��� �� ����� ����� ����������
            ��������� ���������� ������ }
          err:=X502_GetNextExpectedLchNum(hnd, firstLch);
          if err = X502_ERR_OK then
          begin
            { ��������� ������ �� ���������� ���� � ������� ��� �
              ��������� ��� � ������ }
            err := X502_ProcessData(hnd, rcv_buf, rcv_size, X502_PROC_FLAGS_VOLT,
                                    adcData, adcSize, dinData, dinSize);
            if err = X502_ERR_OK then
            begin
              // ��������� �������� ��������� ����������
              Synchronize(updateData);
            end;
          end;
        end
      end;

      { �� ������ �� ����� ������������� �����.
        ����� �� �������� ��� ������ (���� ����� �� ������)
        ��������� �������� ��������� � ��������� ���������� }
      stoperr:= X502_StreamsStop(hnd);
      if err = X502_ERR_OK then
        err:= stoperr;
    end;
  end;
end.
