unit MainForm;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, System.DateUtils, Vcl.ExtCtrls,
  Catastre, Prozess;

type
  TForm1 = class(TForm)
    Button1: TButton;
    Panel1: TPanel;
    Memo1: TMemo;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button20: TButton;
    Button5: TButton;
    Button6: TButton;
    Panel2: TPanel;
    Edit1: TEdit;
    Button8: TButton;
    Button7: TButton;
    Button9: TButton;
    Button10: TButton;
    Button11: TButton;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button20Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Button6Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Button8Click(Sender: TObject);
    procedure Button7Click(Sender: TObject);
    procedure Button9Click(Sender: TObject);
    procedure Button10Click(Sender: TObject);
    procedure Button11Click(Sender: TObject);
  private
    { Private-Deklarationen }
    strFormat : String;
  public
    { Public-Deklarationen }
  end;

var
  Form1: TForm1;
  Proc : TProcesses;

implementation

{$R *.dfm}


procedure TForm1.Button1Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   Memo1.Lines.Clear;
   dsStart := Now;
   cnt := Proc.LoadCatastre(AnsiString(Edit1.Text + 'berlin_infos.dat'));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['Reading sequential data ...', cnt, time]);
   Memo1.Lines.Add(out);
end;

procedure TForm1.Button2Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   dsStart := Now;
   cnt := Proc.SortCatastrePlace;
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['sorting dynamic array ...', cnt, time]);
   Memo1.Lines.Add(out);
end;

procedure TForm1.Button3Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   dsStart := Now;
   cnt := Proc.Calculate(52.520803, 13.40945);
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['calculating dynamic array ...', cnt, time]);
   Memo1.Lines.Add(out);
end;

procedure TForm1.Button4Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   dsStart := Now;
   cnt := Proc.WriteToFile(AnsiString(Edit1.Text + 'output_streets.txt'));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['Writing sequential data ...', cnt, time]);
   Memo1.Lines.Add(out);
end;

procedure TForm1.Button5Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   dsStart := Now;
   cnt := Proc.SortCatastreDistance;
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['distance sorting dynamic array ...', cnt, time]);
   Memo1.Lines.Add(out);
end;

procedure TForm1.Button6Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   dsStart := Now;
   cnt := Proc.SortCatastreStandard;
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['standard sorting dynamic array ...', cnt, time]);
   Memo1.Lines.Add(out);
end;

procedure TForm1.Button7Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time : Int64;
   out : String;
begin
   dsStart := Now;
   Proc.DeleteDirectories(AnsiString(Edit1.Text));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['deleting directory structure ...', 0, time]);
   Memo1.Lines.Add(out);
end;

procedure TForm1.Button8Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   dsStart := Now;
   cnt := Proc.WriteToDir(AnsiString(Edit1.Text));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['creating directory structure ...', cnt, time]);
   Memo1.Lines.Add(out);
end;

procedure TForm1.Button9Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   dsStart := Now;
   cnt := Proc.FreeData;
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['delete dynamic memory ...', cnt, time]);
   Memo1.Lines.Add(out);
end;


procedure TForm1.Button10Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   dsStart := Now;
   cnt := Proc.ReadFromDir(AnsiString(Edit1.Text));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['read from directory ...', cnt, time]);
   Memo1.Lines.Add(out);
end;

procedure TForm1.Button11Click(Sender: TObject);
var
   dsStart, dsEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   dsStart := Now;
   cnt := Proc.Partition;
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['partitioning data ...', cnt, time]);
   Memo1.Lines.Add(out);
end;


procedure TForm1.Button20Click(Sender: TObject);
var
   dsStart, dsEnd, dsAllStart, dsAllEnd : TDateTime;
   time , cnt : Int64;
   out : String;
begin
   Memo1.Lines.Clear;
   { reading sequential file with catastre data }
   dsAllStart := now;
   dsStart := Now;
   cnt := Proc.LoadCatastre(AnsiString(Edit1.Text + 'berlin_infos.dat'));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['Reading sequential data ...', cnt, time]);
   Memo1.Lines.Add(out);

   { calculating distance and direction angle from each item to the television tower }
   dsStart := Now;
   cnt := Proc.Calculate(52.520803, 13.40945);
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['calculating dynamic array ...', cnt, time]);
   Memo1.Lines.Add(out);

   { sort data for city, urban_unit, district .. with DIN norm }
   dsStart := Now;
   cnt := Proc.SortCatastrePlace;
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['sorting dynamic array ...', cnt, time]);
   Memo1.Lines.Add(out);

   { write data to a new sequential file with calculations }
   dsStart := Now;
   cnt := Proc.WriteToFile(AnsiString(Edit1.Text + 'output_streets.txt'));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['Writing sequential data ...', cnt, time]);
   Memo1.Lines.Add(out);

   { delete the old directory structure with data }
   dsStart := Now;
   Proc.DeleteDirectories(AnsiString(Edit1.Text));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['deleting directory structure ...', 0, time]);
   Memo1.Lines.Add(out);

   { creating directory structure and write data street-wise in this }
   dsStart := Now;
   cnt := Proc.WriteToDir(AnsiString(Edit1.Text));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['creating directory structure ...', cnt, time]);
   Memo1.Lines.Add(out);

   { deleting the dynamic memory, Delphi get faulted because reference counting
     WASTE memory and time to support unqualified programmers }
   dsStart := Now;
   cnt := Proc.FreeData;
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['delete dynamic memory (get faulted) !!! ...', cnt, time]);
   Memo1.Lines.Add(out);

   { read catastre data back from directories, DELPHI is under observation because
     memory didn't deleted before according to the rules }
   dsStart := Now;
   cnt := Proc.ReadFromDir(AnsiString(Edit1.Text));
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['read from directories (under observation) !!! ...', cnt, time]);
   Memo1.Lines.Add(out);

   { partitioning of data with a distance < 1000 meters from television tower
     sorting and seek value with delphi because there isn't a partition algorith }
   dsStart := Now;
   cnt := Proc.Partition;
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['partitioning data (workaround with sorting) ...', cnt, time]);
   Memo1.Lines.Add(out);

   { write the partioned and sorted data to a file }
   dsStart := Now;
   cnt := Proc.WritePartOfCatastre(AnsiString(Edit1.Text + 'output_distance.txt'), 0, cnt);
   dsEnd  := Now;
	 time := MilliSecondsBetween(dsEnd, dsStart);
   out := Format(strFormat, ['write partition of data ...', cnt, time]);
   Memo1.Lines.Add(out);

   dsAllEnd := Now;
   Memo1.Lines.Add('');
   time := MilliSecondsBetween(dsAllEnd, dsAllStart);
   out := Format(strFormat, ['time for all steps ...', 0, time]);
   Memo1.Lines.Add(out);

end;

procedure TForm1.FormCreate(Sender: TObject);
begin
   if DirectoryExists('C:\Projekte\Extreme\Data\') then
      Edit1.Text := 'C:\Projekte\Extreme\Data\'
   else
      Edit1.Text := 'D:\Test\';
   FormatSettings.ThousandSeparator := '.';
   strFormat := '%-50s Size = %7d, in %7d ms';
   Proc := TProcesses.Create;
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
   Proc.Free;
end;

end.
