program x502_example;

uses
  Forms,
  MainUnit in 'MainUnit.pas' {MainForm},
  X502_ProcessThread in 'X502_ProcessThread.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
