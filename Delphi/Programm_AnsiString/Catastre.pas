unit Catastre;

interface

uses
   System.SysUtils, System.Variants, System.Classes, System.Math,
   System.AnsiStrings,
   Location;
type


TCatastreData = class(TLocation)
   private
	     mCity :         AnsiString;
	     mStreet :       AnsiString;
	     mStreetNumber : AnsiString;
	     mZipCode :      AnsiString;
	     mUrban_Unit :   AnsiString;
	     mOld_Unit :     AnsiString;
	     mDistrict :     AnsiString;
   public
     constructor Create;
     destructor  Destroy; override;

	   function GetCity :         AnsiString;
	   function GetStreet :       AnsiString;
	   function GetStreetNumber : AnsiString;
	   function GetZipCode :      AnsiString;
	   function GetUrban_Unit :   AnsiString;
	   function GetOld_Unit :     AnsiString;
	   function GetDistrict :     AnsiString;

	   procedure SetCity(const newVal : AnsiString);
	   procedure SetStreet(const newVal : AnsiString);
	   procedure SetStreetNumber(const newVal : AnsiString);
	   procedure SetZipCode(const newVal : AnsiString);
	   procedure SetUrban_Unit(const newVal : AnsiString);
	   procedure SetOld_Unit(const newVal : AnsiString);
	   procedure SetDistrict(const newVal : AnsiString);

	   property City : AnsiString read GetCity write SetCity;
	   property Street : AnsiString read GetStreet write SetStreet;
	   property StreetNumber : AnsiString read GetStreetNumber write SetStreetNumber;
	   property ZipCode : AnsiString read GetZipCode write SetZipCode;
	   property Urban_Unit : AnsiString read GetUrban_Unit write SetUrban_Unit;
	   property Old_Unit : AnsiString read GetOld_Unit write SetOld_Unit;
	   property District : AnsiString read GetDistrict write SetDistrict;

end;

implementation



constructor TCatastreData.Create;
begin
  TLocation.Create;
  mCity         := '';
  mStreet       := '';
  mStreetNumber := '';
  mZipCode      := '';
  mUrban_Unit   := '';
  mOld_Unit     := '';
  mDistrict     := '';
end;

destructor TCatastreData.Destroy;
begin
  // braucht es hier einen Aufruf zum vorhergehenden Destructor?
  mCity         := '';
  mStreet       := '';
  mStreetNumber := '';
  mZipCode      := '';
  mUrban_Unit   := '';
  mOld_Unit     := '';
  mDistrict     := '';
end;

function TCatastreData.GetCity : AnsiString;
begin
  Result := mCity;
end;

function TCatastreData.GetStreet : AnsiString;
begin
  Result := mStreet;
end;

function TCatastreData.GetStreetNumber : AnsiString;
begin
   Result := mStreetNumber;
end;

function TCatastreData.GetZipCode : AnsiString;
begin
   Result := mZipCode;
end;

function TCatastreData.GetUrban_Unit : AnsiString;
begin
   Result := mUrban_Unit;
end;

function TCatastreData.GetOld_Unit : AnsiString;
begin
   Result := mOld_Unit;
end;

function TCatastreData.GetDistrict : AnsiString;
begin
   Result := mDistrict;
end;

procedure TCatastreData.SetCity(const newVal : AnsiString);
begin
  mCity := newVal;
end;

procedure TCatastreData.SetStreet(const newVal : AnsiString);
begin
  mStreet := newVal;
end;

procedure TCatastreData.SetStreetNumber(const newVal : AnsiString);
begin
  mStreetNumber := newVal;
end;

procedure TCatastreData.SetZipCode(const newVal : AnsiString);
begin
  mZipCode := newVal;
end;

procedure TCatastreData.SetUrban_Unit(const newVal : AnsiString);
begin
  mUrban_Unit := newVal;
end;

procedure TCatastreData.SetOld_Unit(const newVal : AnsiString);
begin
  mOld_Unit := newVal;
end;

procedure TCatastreData.SetDistrict(const newVal : AnsiString);
begin
  mDistrict := newVal;
end;



end.
