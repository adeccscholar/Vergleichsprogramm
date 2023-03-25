unit Prozess;

interface
uses
   System.SysUtils, System.Variants, System.Classes, System.Math,
   System.Generics.Collections, System.Generics.Defaults, System.IOUtils,
   System.StrUtils,
   Catastre;
type

TProcesses = class(TObject)
  private
     Dictionary: TDictionary<String, String>;

  public
     constructor Create;
     destructor Destroy; override;

     function LoadCatastre(filename : String) : Int64;
     function SortCatastreStandard : Int64;
     function SortCatastrePlace : Int64;
     function SortCatastreDistance : Int64;
     function Calculate(latitude, longitude : Double) : Int64;
     function WriteToFile(fileName : String) : Int64;

     function DeleteDirectories(path : String) : Int64;
     function WriteToDir(path : String) : Int64;
     function ReadFromDir(path : String) : Int64;
     function FreeData : Int64;
     function Partition : Int64;
     function SortPartition : Int64;
     function WritePartOfCatastre(fileName : String; from_param, to_param : Int64) : int64;

  private
     function FindExtension(strText : String) : Integer;
     function MyCompareText(Left, Right : String) : Integer;

end;

var
   mCadastre: array of TCatastreData;


implementation

constructor TProcesses.Create;
begin
   Dictionary := TDictionary<String, String>.Create;
   Dictionary.Add('ä','ae');
   Dictionary.Add('Ä','Ae');
   Dictionary.Add('ö','oe');
   Dictionary.Add('Ö','Oe');
   Dictionary.Add('ü','ue');
   Dictionary.Add('Ü','Ue');
   Dictionary.Add('ß','ss');
   Dictionary.Add('á','a');
   Dictionary.Add('é','e');
   Dictionary.Add('è','e');
end;

destructor TProcesses.Destroy;
begin
   Dictionary.Free;
end;


function TProcesses.LoadCatastre(filename : String) : Int64;
var
   text : TStringList;
   row  : TStringList;
   fmt  : TFormatSettings;
   i    : Integer;
begin
   text := TStringList.Create;
   try
      text.LoadFromFile(filename);
      SetLength(mCadastre, text.Count);
      row := TStringList.Create;
      try
         fmt := TFormatSettings.Create;
         try
			      fmt.DecimalSeparator := '.';
            row.Delimiter := ';';
            row.StrictDelimiter := true;
            for i := 0 to text.Count-1 do
            begin
 				       if text[i].Length > 0 then
               begin
				          row.DelimitedText := text[i];
                  mCadastre[i] := TCatastreData.Create;
                  mCadastre[i].City         := row[0];
				          mCadastre[i].Street       := row[1];
				          mCadastre[i].StreetNumber := LowerCase(row[2]);
				          mCadastre[i].ZipCode      := row[3];
				          mCadastre[i].Urban_Unit   := row[4];
				          mCadastre[i].Old_Unit     := row[5];
				          mCadastre[i].District     := row[6];
				          mCadastre[i].Latitude     := StrToFloat(row[7], fmt);
				          mCadastre[i].Longitude    := StrToFloat(row[8], fmt);
               end;
            end;
         finally
            ;
         end;
      finally
         row.Free;
      end;
   finally
      text.Free
   end;
   Result := Length(mCadastre);
end;


// eventuell var / const
function TProcesses.FindExtension(strText : String) : Integer;
var
   i : Integer;
begin
   for i := Length(strText)  downto 1 do
   begin
      if CharInSet(strText[i], ['0' .. '9']) then
      begin
         Result := i;
         exit;
      end;
   end;
   Result := -1;
end;

function TProcesses.MyCompareText(Left, Right : String) : Integer;
  function Replace(text : String) : String;
  var
     pattern: String;
  begin
     for pattern in Dictionary.Keys do
        if ContainsText(text, pattern) then
           text := StringReplace(text, pattern, Dictionary.Items[pattern], [rfReplaceAll]);
     Result := text;
  end;

begin

  Left  := Replace(Left);
  Right := Replace(Right);
  if Left = Right then     Result := 0
  else
  begin
      if left < right then Result := -1
      else                 Result :=  1;
  end;

end;


function TProcesses.SortCatastreStandard : Int64;
begin

   TArray.Sort<TCatastreData>(mCadastre , TComparer<TCatastreData>.Construct(
      function(const Left, Right: TCatastreData): integer
      var
         cmp, h1, h2, n1, n2 : Integer;
         z1, z2 : String;
      begin
         Result := 0;
         cmp := CompareText(Left.City, Right.City);
         if cmp <> 0 then Result := cmp
         else
           begin
            cmp := CompareText(Left.Urban_Unit, Right.Urban_Unit);
            if cmp <> 0 then Result := cmp
            else
            begin
               cmp := CompareText(Left.District, Right.District);
               if cmp <> 0 then Result := cmp
               else
               begin
                  cmp := CompareText(Left.Street, Right.Street);
                  if cmp <> 0 then Result := cmp
                  else
                  begin

                     h1 := FindExtension(Left.StreetNumber);
                     h2 := FindExtension(Right.StreetNumber);
                     if (h1 <> -1) AND (h2 <> -1) then
                     begin
                        n1 := StrToInt(LeftStr(Left.StreetNumber, h1));
                        n2 := StrToInt(LeftStr(Right.StreetNumber, h2));
                        cmp := CompareValue(n1, n2);
                        if cmp <> 0 then Result := cmp
                        else
                        begin
                           z1 := RightStr(Left.StreetNumber, Length(Left.StreetNumber) - h1);
                           z2 := RightStr(Right.StreetNumber, Length(Right.StreetNumber) - h2);
                           cmp := CompareText(z1, z2);
                           if cmp <> 0 then Result := cmp
                           else
                           begin
                              Result := CompareText(Left.ZipCode, Right.ZipCode);
                           end;
                        end;
                     end
                  end;
               end;
            end;
         end;
      end));
   Result := Length(mCadastre);
end;




function TProcesses.SortCatastrePlace : Int64;
begin

   TArray.Sort<TCatastreData>(mCadastre , TComparer<TCatastreData>.Construct(
      function(const Left, Right: TCatastreData): integer
      var
         cmp, h1, h2, n1, n2 : Integer;
         z1, z2 : String;
      begin
         cmp := MyCompareText(Left.City, Right.City);
         if cmp <> 0 then Result := cmp
         else
         begin
            cmp := MyCompareText(Left.Urban_Unit, Right.Urban_Unit);
            if cmp <> 0 then Result := cmp
            else
            begin
               cmp := MyCompareText(Left.District, Right.District);
               if cmp <> 0 then Result := cmp
               else
               begin
                  cmp := CompareText(Left.ZipCode, Right.ZipCode);
                  if cmp <> 0 then Result := cmp
                  else
                  begin
                     cmp := MyCompareText(Left.Street, Right.Street);
                     if cmp <> 0 then Result := cmp
                     else
                     begin
                        h1 := FindExtension(Left.StreetNumber);
                        h2 := FindExtension(Right.StreetNumber);
                        if (h1 <> -1) AND (h2 <> -1) then
                        begin
                           n1 := StrToInt(String(LeftStr(Left.StreetNumber, h1)));
                           n2 := StrToInt(String(LeftStr(Right.StreetNumber, h2)));
                           cmp := CompareValue(n1, n2);
                           if cmp <> 0 then Result := cmp
                           else
                           begin
                              z1 := RightStr(Left.StreetNumber, Length(Left.StreetNumber) - h1);
                              z2 := RightStr(Right.StreetNumber, Length(Right.StreetNumber) - h2);
                              cmp := myCompareText(z1, z2);
                              Result := cmp;
                           end;
                        end
                        else
                        begin
                           cmp := myCompareText(Left.StreetNumber, Right.StreetNumber);
                           Result := cmp;
                        end;
                     end;
                  end;
               end;
            end;
         end;
      end));
   Result := Length(mCadastre);
end;


// Teilbereich sortieren ?
function TProcesses.SortCatastreDistance : Int64;
begin

   TArray.Sort<TCatastreData>(mCadastre , TComparer<TCatastreData>.Construct(
      function(const Left, Right: TCatastreData): integer
      var
         cmp : Integer;
      begin
         cmp := CompareValue(Left.Distance, Right.Distance);
         if cmp <> 0 then Result := cmp
         else
         begin
            cmp := CompareValue(Left.Angle, Right.Angle);
            Result := cmp;
         end;
      end));
   Result := Length(mCadastre);
end;



function TProcesses.Calculate(latitude, longitude : Double) : Int64;
var
   i : Integer;
begin
   for i := 0 to Length(mCadastre) - 1 do
   begin
      mCadastre[i].Calculate(latitude, longitude);
   end;
   Result := Length(mCadastre);
end;


function TProcesses.WriteToFile(fileName : String) : Int64;
var
   text : TextFile;
   i : Int64;
begin
   AssignFile(text, String(fileName));
   Rewrite(text);
   try
      for i := 0 to Length(mCadastre) - 1 do
      begin
         Writeln(text, Format('%-5s %-75s%-65s=%15.3nm / %7.3n°',
                         [ mCadastre[i].ZipCode,
                           mCadastre[i].City + ' - ' + mCadastre[i].District + ', ',
                           mCadastre[i].Street + ' ' + mCadastre[i].StreetNumber,
                           mCadastre[i].Distance,
                           mCadastre[i].Angle ] ));
      end;
   finally
      CloseFile(text);
   end;
   Result := Length(mCadastre);
end;



function TProcesses.DeleteDirectories(path : String) : Int64;
var
   SearchResult: TSearchRec;
begin
   if FindFirst(String(path + '\*'), faAnyFile, SearchResult) = 0 then
   begin
      try
         repeat
            if (SearchResult.Attr and faDirectory) = faDirectory then
            begin
               if (SearchResult.Name <> '.') and (SearchResult.Name <> '..') then
                  TDirectory.Delete(String(path) + '\' + SearchResult.Name, True)
            end
         until FindNext(SearchResult) <> 0;
      finally
         FindClose(SearchResult);
      End;
   end;
   Result := 1;
end;


function TProcesses.WriteToDir(path : String) : Int64;
   procedure create_directories(newpath : String);
   begin
      if not ForceDirectories(newpath) then
      begin
         raise Exception.Create('Cannot create directory: ' + newpath);
      end;
   end;

   procedure write_data(var text : TextFile; var fmt : TFormatSettings; var data :TCatastreData);
   begin
      Writeln(text, Format('%s;%s;%s;%.9n;%.9n;%.3n;%.3n',
                         [ data.StreetNumber, data.ZipCode, data.Old_Unit,
                           data.Latitude, data.Longitude,
                           data.Distance, data.Angle ], fmt ));
   end;

   function open_file(var text : TextFile; filename : String) : Boolean;
   begin
      try
         Assign(text, String(filename));
         Rewrite(text);
         Result := true;
      except
         on Exception do
         begin
            Result := false;
         end;
      end;
   end;

   function close_file(var text : TextFile) : Boolean;
   begin
     CloseFile(text);
     Result := false;
   end;

var
   strCity, strDistrict, strUrban_Unit, strStreet : String;
   strNewPath, strFile : String;
   text : TextFile;
   is_open : Boolean;
   i : Int64;
   fmt  : TFormatSettings;

begin
   is_open := false;
   try
      is_open := false;
      if not DirectoryExists(String(path)) then
      begin
         raise Exception.Create('basis directory dont exists');
      end
      else
      begin
         fmt := TFormatSettings.Create;
			   fmt.DecimalSeparator  := '.';
         fmt.ThousandSeparator := #0;
         SortCatastreStandard;
         is_open       := false;
         strCity       := '';
         strDistrict   := '';
         strUrban_Unit := '';
         strStreet     := '';
         SetCurrentDir(String(path));
         //j := 0;
         for i := 0 to Length(mCadastre) - 1 do
         begin
            if strCity <>  mCadastre[i].City then
            begin
               strCity       := mCadastre[i].City;
               strUrban_Unit := mCadastre[i].Urban_Unit;
               strDistrict   := mCadastre[i].District;
               strStreet     := mCadastre[i].Street;
               //j := 0;
               strNewPath := path+ '\'+ strCity +'\'+ strUrban_Unit +'\'+strDistrict;
               create_directories(String(strNewPath));
               strFile := strNewPath + '\' + strStreet + '.csv';
               if is_open then is_open := close_file(text);
               is_open := open_file(text, strFile);
            end
            else if strUrban_Unit <> mCadastre[i].Urban_Unit then
            begin
               strUrban_Unit := mCadastre[i].Urban_Unit;
               strDistrict   := mCadastre[i].District;
               strStreet     := mCadastre[i].Street;
               //j := 0;
               strNewPath := path+ '\'+ strCity +'\'+ strUrban_Unit +'\'+strDistrict;
               create_directories(String(strNewPath));
               strFile := strNewPath + '\' + strStreet + '.csv';
               if is_open then is_open := close_file(text);
               is_open := open_file(text, strFile);
            end
            else if strDistrict <> mCadastre[i].District then
            begin
               strDistrict   := mCadastre[i].District;
               strStreet     := mCadastre[i].Street;
               //j := 0;
               strNewPath := path+ '\'+ strCity +'\'+ strUrban_Unit +'\'+strDistrict;
               create_directories(String(strNewPath));
               strFile := strNewPath + '\' + strStreet + '.csv';
               if is_open then is_open := close_file(text);
               is_open := open_file(text, strFile);
            end
            else if strStreet <> mCadastre[i].Street then
            begin
               strStreet := mCadastre[i].Street;
               //j := 0;
               strFile   := strNewPath + String('\') + strStreet + String('.csv');
               if is_open then is_open := close_file(text);
               is_open := open_file(text, strFile);
            end;
            write_data(text, fmt, mCadastre[i]);
            //Inc(j);
         end;
      end;
   finally
      if is_open then close_file(text);
   end;
   Result := Length(mCadastre);
end;

function TProcesses.FreeData : Int64;
var
   cnt : Int64;
begin
    cnt := Length(mCadastre);
    //mCadastre;
    Delete(mCadastre, 0, cnt);
    SetLength(mCadastre, 0);
    Result := Cnt;
end;


function TProcesses.ReadFromDir(path : String) : Int64;
   function GetItems(path : String; params : array of String; boDirectory : Boolean) : TStringList;
   var
      SearchFunc : TSearchRec;
      compl_path : String;
      FileAttrs : Integer;
      i : Integer;
   begin
      Result := TStringList.Create;
      compl_path := path;
      for i := 0 to Length(params) - 1 do
      begin
         compl_path := String(TPath.Combine(String(compl_path), String(params[i])));
      end;
      SetCurrentDir(String(compl_path));
      if boDirectory then FileAttrs := faDirectory
      else                FileAttrs := faAnyFile-faDirectory;
      if FindFirst(String(compl_path) + '\*', FileAttrs, SearchFunc) = 0 then
      begin
         Try
            repeat
               if(boDirectory and  ((SearchFunc.Attr and faDirectory) = faDirectory)) then
               begin
                  if (SearchFunc.Name <> '.') and (SearchFunc.Name <> '..') then
                  begin
                     Result.Add(SearchFunc.Name);
                  end
               end;
               if(not boDirectory) then
               begin
                  if (SearchFunc.Name <> '.') and (SearchFunc.Name <> '..') then
                  begin
                     Result.Add(TPath.GetFileNameWithoutExtension(SearchFunc.Name));
                  end
               end;
            until FindNext(SearchFunc) <> 0;
         Finally
            FindClose(SearchFunc);
         End;
      end;
   end;

   function ReadItems(path, city, urban_unit, district, street : String) : Int64;
   var
     text        : TStringList;
     row         : TStringList;
     filename    : String;
     fmt         : TFormatSettings;
     i, cnt, old : Integer;
   begin
      if path[Length(path)] <> '\' then path := path + '\';

      filename := TPath.Combine(String(path),String(city));
      filename := TPath.Combine(String(filename), String(urban_unit));
      filename := TPath.Combine(String(filename), String(district));
      filename := TPath.Combine(String(filename), String(street));
      filename := filename + '.csv';          // Change Extension don't work because '.' in some names
      text := TStringList.Create;
      try
         text.LoadFromFile(String(filename));
         old := Length(mCadastre);
         cnt := text.Count;
         SetLength(mCadastre, Length(mCadastre) + cnt);
         row := TStringList.Create;
         try
            fmt := TFormatSettings.Create;
            try
			         fmt.DecimalSeparator := '.';
               row.Delimiter := ';';
               row.StrictDelimiter := true;
               for i := 0 to text.Count-1 do
               begin
 				          if text[i].Length > 0 then
                  begin
				             row.DelimitedText := text[i];
                     mCadastre[old + i] := TCatastreData.Create;
                     mCadastre[old + i].City         := city;
				             mCadastre[old + i].Street       := street;
				             mCadastre[old + i].StreetNumber := LowerCase(row[0]);
				             mCadastre[old + i].ZipCode      := row[1];
				             mCadastre[old + i].Urban_Unit   := urban_unit;
				             mCadastre[old + i].Old_Unit     := row[2];
				             mCadastre[old + i].District     := district;
				             mCadastre[old + i].Latitude     := StrToFloat(row[3], fmt);
				             mCadastre[old + i].Longitude    := StrToFloat(row[4], fmt);
				             mCadastre[old + i].Distance     := StrToFloat(row[5], fmt);
				             mCadastre[old + i].Angle        := StrToFloat(row[6], fmt);
                  end;
               end;
            finally
               ;
            end;
         finally
            row.Free;
         end;
      finally
         text.Free
      end;
      Result := cnt;
   end;

var
  Cities, Urban_Units, Districts, Streets : TStringList;
  strCity, strUrban_Unit, strDistrict, strStreet : String;
  h, i, j : Integer;
  k: Integer;
begin

   Cities := GetItems(path, [], true);
   try
      for h := 0 to Cities.Count - 1 do
      begin
         strCity := String(Cities[h]);
         Urban_Units := GetItems(path, [strCity], true);
         try
            for i := 0 to Urban_Units.Count - 1 do
            begin
               strUrban_Unit := String(Urban_Units[i]);
               Districts := GetItems(path, [strCity, strUrban_Unit], true);
               try
                  for j := 0 to Districts.Count - 1 do
                  begin
                     strDistrict := String(Districts[j]);
                     Streets := GetItems(path, [strCity,strUrban_Unit,strDistrict],false);
                     try
                        for k := 0 to Streets.Count-1 do
                        begin
                           strStreet := String(Streets[k]);
                           ReadItems(path, strCity, strUrban_unit, strDistrict, strStreet);
                        end;
                     finally
                        Streets.Free;
                     end;
                  end;
               finally
                  Districts.Free;
               end;
            end;
         finally
            Urban_Units.Free;
         end;
      end;
   finally
      Cities.Free;
   end;
   Result :=  Length(mCadastre);
end;


function TProcesses.Partition : Int64;
var
   i : Int64;
begin
   TArray.Sort<TCatastreData>(mCadastre , TComparer<TCatastreData>.Construct(
      function(const Left, Right: TCatastreData): integer
      var
         cmp : Integer;
      begin
         cmp := CompareValue(Left.Distance, Right.Distance);
         if cmp <> 0 then Result := cmp
         else
         begin
            cmp := CompareValue(Left.Angle, Right.Angle);
            Result := cmp;
         end;
      end));
   i := 0;
   while ((i < Length(mCadastre)) AND (mCadastre[i].Distance < 1000.0)) do Inc(i);
   Result := i;
end;

function TProcesses.SortPartition : Int64;
begin
   Result := 0;
end;


function TProcesses.WritePartOfCatastre(fileName : String; from_param, to_param : Int64) : int64;
var
   text : TextFile;
   i : Int64;
begin
   AssignFile(text, String(fileName));
   Rewrite(text);
   if (from_param < 0) or (from_param > Length(mCadastre) - 1)  then
   begin
     raise Exception.Create(Format('from Param %d should between (%d, %d)', [from_param, 0, Length(mCadastre) - 1]));
   end;
   if (to_param <= from_param) or (from_param > Length(mCadastre) - 1)  then
   begin
     raise Exception.Create(Format('from Param %d should between (%d, %d)', [to_param, from_param, Length(mCadastre) - 1]));
   end;

   try
      for i := from_param to to_param - 1 do
      begin
         Writeln(text, Format('%-5s %-75s%-65s=%15.3nm /%7.3n°',
                         [ mCadastre[i].ZipCode,
                           mCadastre[i].City + ' - ' + mCadastre[i].District + ', ',
                           mCadastre[i].Street + ' ' + mCadastre[i].StreetNumber,
                           mCadastre[i].Distance,
                           mCadastre[i].Angle ] ));
         // Result := i; // normally behind the loop, but avoid undefined state of I after the loop
      end;
   finally
      CloseFile(text);
   end;
   Result := to_param - from_param;
end;











end.
