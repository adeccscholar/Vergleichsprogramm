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

Location& Location::operator = (Location&& ref) {
   swap(ref);
   return *this;
   }


void Location::swap(Location& ref) noexcept {
   using std::swap;
   swap(flLatitude, ref.flLatitude);
   swap(flLongitude, ref.flLongitude);
   }
