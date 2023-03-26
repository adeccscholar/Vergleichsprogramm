#include "MyLocation.h"


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
