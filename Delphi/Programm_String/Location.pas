unit Location;

interface
uses
   System.SysUtils, System.Variants, System.Classes, System.Math;

type

TLocation = class(TObject)
   private
	   mLatitude  : Double;
	   mLongitude : Double;
	   mDistance  : Double;
	   mAngle     : Double;
   public
     constructor Create;
     // brauche ich einen Destructor

	   function GetLatitude  : Double;
	   function GetLongitude : Double;
	   function GetDistance  : Double;
	   function GetAngle     : Double;
     function GetRadius    : Double;

	   procedure SetLatitude(const newVal : Double);
	   procedure SetLongitude(const newVal : Double);
	   procedure SetDistance(const newVal : Double);
	   procedure SetAngle(const newVal : Double);

     property Latitude  : Double  read GetLatitude write SetLatitude;
     property Longitude : Double  read GetLongitude write SetLongitude;
     property Distance  : Double  read GetDistance write SetDistance;
     property Angle     : Double  read GetAngle write SetAngle;
     property Radius    : Double  read GetRadius;

     procedure Calculate(var pLatitude : Double; var pLongitude : Double);
end;

const
   mRadius : Double = 6371000.785;

implementation
constructor TLocation.Create;
begin
     // muss hier der Aufruf zur Basisklasse stehen?
	   mLatitude  := 0.0;
	   mLongitude := 0.0;
	   mDistance  := 0.0;
	   mAngle     := 0.0;
end;


function TLocation.GetLatitude  : Double;
begin
  Result := mLatitude;
end;

function TLocation.GetLongitude : Double;
begin
  Result := mLongitude;
end;

function TLocation.GetDistance  : Double;
begin
  Result := mDistance;
end;

function TLocation.GetAngle     : Double;
begin
  Result := mAngle;
end;

function TLocation.GetRadius    : Double;
begin
  Result := mRadius;
end;


procedure TLocation.SetLatitude(const newVal : Double);
begin
   mLatitude := newVal;
end;


procedure TLocation.SetLongitude(const newVal : Double);
begin
   mLongitude := newVal;
end;


procedure TLocation.SetDistance(const newVal : Double);
begin
   mDistance := newVal;
end;


procedure TLocation.SetAngle(const newVal : Double);
begin
   mAngle := newVal;
end;


procedure TLocation.Calculate(var pLatitude : Double; var pLongitude : Double);
var
   phiA, lambdaA, phiB, lambdaB, zeta, alpha, help1, help2, help3 : Extended;
begin
	phiA     := DegToRad(pLatitude);
	lambdaA  := DegToRad(pLongitude);
	phiB     := DegToRad(mLatitude);
	lambdaB  := DegToRad(mLongitude);
	zeta     := ArcCos(Sin(phiA) * Sin(phiB) + Cos(phiA) * Cos(phiB) * Cos(lambdaB - lambdaA));
  help1    := Sin(phiB) - Sin(phiA) * Cos(zeta);
  help2    := Cos(phiA) * Sin(zeta);
  help3    := help1 / help2;         { Probleme mit Ungenauigkeiten beim Rechnen !!! help3 = -1,00000000000352 }
  if help3 < -1.0 then help3 := -1.0
  else if help3 > 1.0 then help3 := 1.0;
  help3    := ArcCos(help3);
  { alpha    := ArcCos((Sin(phiB) - Sin(phiA) * Cos(zeta)) / (Cos(phiA) * Sin(zeta))); }
  alpha    := RadToDeg(help3);
	Distance := zeta * Radius;

	if lambdaA > lambdaB then
  begin
	   if phiA > 0 then
     begin
        Angle := 360.0 - alpha;
     end
	   else
     begin
        Angle := 180.0 + alpha;
     end;
  end
	else
  begin
	   if phiA > 0 then
     begin
         Angle := alpha;
     end
	   else
     begin
        Angle := 180.0 - alpha;
     end;
  end;
end;

end.
