program Delphi_Vergleichsprogramm_AnsiString;

uses
  Vcl.Forms,
  MainForm in 'MainForm.pas' {Form1},
  Catastre in 'Catastre.pas',
  Location in 'Location.pas',
  Prozess in 'Prozess.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
