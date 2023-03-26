#include "MyLocation.h"

#define PI 3.14159265358979323846


std::ostream& operator << (std::ostream& out, Result const& data) {
   out << "(" << data.flDistance << "m, " << data.flAngle << "°";
   return out;
}

Result::Result(double const& a, double const& b) {
   flDistance = a;
   flAngle = b;
}

Result& Result::operator = (Result const& ref) {
   flDistance = ref.flDistance;
   flAngle = ref.flAngle;
   return *this;
}
#if defined WITH_MOVE
Result& Result::operator = (Result&& ref) noexcept {
   swap(ref);
   return *this;
}


void Result::swap(Result& ref) noexcept {
   using std::swap;
   swap(flDistance, ref.flDistance);
   swap(flAngle, ref.flAngle);
}
#endif



std::ostream& operator << (std::ostream& out, Location const& data) {
   out << "(" << data.flLatitude << ", " << data.flLongitude << ")";
   return out;
   }

Location::Location(double const& a, double const& b) {
   flLatitude = a;
   flLongitude = b;
   }

Location& Location::operator = (Location const& ref) {
   flLatitude  = ref.flLatitude;
   flLongitude = ref.flLongitude;
   return *this;
   }

#if defined WITH_MOVE
Location& Location::operator = (Location&& ref) noexcept {
   swap(ref);
   return *this;
   }


void Location::swap(Location& ref) noexcept {
   using std::swap;
   swap(flLatitude, ref.flLatitude);
   swap(flLongitude, ref.flLongitude);
   }
#endif




// Funktion zur Konvertierung von Grad in Radian
double Location::deg2rad(double deg) {
   return (deg * PI / 180);
}
double Location::rad2deg(double rad) {
   return (rad * 180 / PI);
}

// Funktion zur Berechnung des Abstands zwischen zwei Positionen in Metern
double Location::distance(Location pos) const {
   double r = 6371e3; // Erdradius in Metern
   double lat1 = deg2rad(flLatitude); // Umwandlung in Radiant
   double lat2 = deg2rad(pos.flLatitude);
   double dLat = deg2rad(pos.flLatitude - flLatitude);
   double dLon = deg2rad(pos.flLongitude - flLongitude);

   double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
   double c = 2 * atan2(sqrt(a), sqrt(1 - a));
   double d = r * c;
   return d;
}

// Funktion zur Berechnung des Kurswinkels von Position 1 zu Position 2
double Location::bearing(Location pos) const {
   double lat1 = deg2rad(flLatitude); // Umwandlung in Radiant
   double lat2 = deg2rad(pos.flLatitude);
   double dLon = deg2rad(pos.flLongitude - flLongitude);

   double y = sin(dLon) * cos(lat2);
   double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);

   double bearing = rad2deg(atan2(y, x));

   // Korrektur des Winkels auf den Bereich von 0 bis 360 Grad
   bearing = fmod(bearing + 360.0, 360.0);

   return bearing;
}
