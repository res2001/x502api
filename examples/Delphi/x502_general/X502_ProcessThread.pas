unit X502_ProcessThread;

interface
uses Classes, Math, SyncObjs,StdCtrls,SysUtils, x502api;
  const RECV_BUF_SIZE = 8*1024*1024;
  const RECV_TOUT     = 250;

type TX502_ProcessThread = class(TThread)
  public
    LChEdits : array of TEdit;
    dinResEdit : TEdit;
    hnd: t_x502_hnd; //описатель модуля

    err : LongInt; //код ошибки при выполнении потока сбора
    stop : Boolean; //запрос на останов (устанавливается из основного потока)

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

  { обновление индикаторов формы результатами последнего измерения.
   Метод должен выполняться только через Syncronize, который нужен
   для доступа к элементам VCL не из основного потока }
  procedure TX502_ProcessThread.updateData;
  var
    lch_cnt, i : LongWord;
    err : LongInt;
  begin
    err := X502_GetLChannelCount(hnd, lch_cnt);
    if err=X502_ERR_OK then
    begin
      // устанавливаем в индикаторах значение первого отсчета из массива
      for  i:=0 to Min(lch_cnt, adcSize) do
        LChEdits[(firstLch + i) mod lch_cnt].Text := FloatToStrF(adcData[i], ffFixed, 4, 8);

      { если есть данные цифрового выхода, то устанавливаем индикатор
            в соответствии с первым значением }
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
    // запускаем синхронные потоки
    err:= X502_StreamsStart(hnd);
    if err = X502_ERR_OK then
    begin
      { выполняем прием пока не произойдет ошибка или
        не будет запроса на останов от основного приложения }
      while not stop and (err = X502_ERR_OK) do
      begin
        //принимаем данные синхронного ввода
        rcv_size := X502_Recv(hnd, rcv_buf, RECV_BUF_SIZE, RECV_TOUT);
        // значение меньше нуля означает ошибку...
        if rcv_size < 0 then
            err := rcv_size
        else if rcv_size>0 then
        begin
          //если больше нуля - значит приняли новые данные
          dinSize:=RECV_BUF_SIZE;
          adcSize:=RECV_BUF_SIZE;
          { получаем номер лог. какнала, соответствующий первому
            отсчету АЦП, так как до этого могли обработать
            некратное количество кадров }
          err:=X502_GetNextExpectedLchNum(hnd, firstLch);
          if err = X502_ERR_OK then
          begin
            { разбираем данные на синхронный ввод и отсчеты АЦП и
              переводим АЦП в Вольты }
            err := X502_ProcessData(hnd, rcv_buf, rcv_size, X502_PROC_FLAGS_VOLT,
                                    adcData, adcSize, dinData, dinSize);
            if err = X502_ERR_OK then
            begin
              // обновляем значения элементов управления
              Synchronize(updateData);
            end;
          end;
        end
      end;

      { По выходу из цикла отсанавливаем поток.
        Чтобы не сбросить код ошибки (если вышли по ошибке)
        результат останова сохраняем в отдельную переменную }
      stoperr:= X502_StreamsStop(hnd);
      if err = X502_ERR_OK then
        err:= stoperr;
    end;
  end;
end.
