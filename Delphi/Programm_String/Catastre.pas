unit Catastre;

interface

uses
   System.SysUtils, System.Variants, System.Classes, System.Math,
   Location;
type


TCatastreData = class(TLocation)
   private
	     mCity :         String;
	     mStreet :       String;
	     mStreetNumber : String;
	     mZipCode :      String;
	     mUrban_Unit :   String;
	     mOld_Unit :     String;
	     mDistrict :     String;
   public
     constructor Create;
     destructor  Destroy; override;

	   function GetCity :         String;
	   function GetStreet :       String;
	   function GetStreetNumber : String;
	   function GetZipCode :      String;
	   function GetUrban_Unit :   String;
	   function GetOld_Unit :     String;
	   function GetDistrict :     String;

	   procedure SetCity(const newVal : String);
	   procedure SetStreet(const newVal : String);
	   procedure SetStreetNumber(const newVal : String);
	   procedure SetZipCode(const newVal : String);
	   procedure SetUrban_Unit(const newVal : String);
	   procedure SetOld_Unit(const newVal : String);
	   procedure SetDistrict(const newVal : String);

	   property City : String read GetCity write SetCity;
	   property Street : String read GetStreet write SetStreet;
	   property StreetNumber : String read GetStreetNumber write SetStreetNumber;
	   property ZipCode : String read GetZipCode write SetZipCode;
	   property Urban_Unit : String read GetUrban_Unit write SetUrban_Unit;
	   property Old_Unit : String read GetOld_Unit write SetOld_Unit;
	   property District : String read GetDistrict write SetDistrict;

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
  // braucht es nicht wirklich
  mCity         := '';
  mStreet       := '';
  mStreetNumber := '';
  mZipCode      := '';
  mUrban_Unit   := '';
  mOld_Unit     := '';
  mDistrict     := '';
end;

function TCatastreData.GetCity : String;
begin
  Result := mCity;
end;

function TCatastreData.GetStreet : String;
begin
  Result := mStreet;
end;

function TCatastreData.GetStreetNumber : String;
begin
   Result := mStreetNumber;
end;

function TCatastreData.GetZipCode : String;
begin
   Result := mZipCode;
end;

function TCatastreData.GetUrban_Unit : String;
begin
   Result := mUrban_Unit;
end;

function TCatastreData.GetOld_Unit : String;
begin
   Result := mOld_Unit;
end;

function TCatastreData.GetDistrict : String;
begin
   Result := mDistrict;
end;

procedure TCatastreData.SetCity(const newVal : String);
begin
  mCity := newVal;
end;

procedure TCatastreData.SetStreet(const newVal : String);
begin
  mStreet := newVal;
end;

procedure TCatastreData.SetStreetNumber(const newVal : String);
begin
  mStreetNumber := newVal;
end;

procedure TCatastreData.SetZipCode(const newVal : String);
begin
  mZipCode := newVal;
end;

procedure TCatastreData.SetUrban_Unit(const newVal : String);
begin
  mUrban_Unit := newVal;
end;

procedure TCatastreData.SetOld_Unit(const newVal : String);
begin
  mOld_Unit := newVal;
end;

procedure TCatastreData.SetDistrict(const newVal : String);
begin
  mDistrict := newVal;
end;



end.
