#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>  // ostream
#include <utility>   // pair

#define WITH_MOVE

class Result {
   friend std::ostream& operator << (std::ostream& out, Result const& data);
#if defined WITH_MOVE
   friend void swap(Result& lhs, Result& rhs) noexcept { lhs.swap(rhs); }
#endif
private:
   double flDistance;
   double flAngle;
public:
   Result(double const& a, double const& b);
   Result(void) : Result(0.0, 0.0) { }
   Result(Result const&) = default;
#if defined WITH_MOVE
   Result(Result&& ref) = default;
#endif
   Result& operator = (Result const& ref);
#if defined WITH_MOVE
   Result& operator = (Result&& ref) noexcept;
   void swap(Result& ref) noexcept;
#endif
   double const& Distance() const { return flDistance; }
   double const& Angle() const { return flAngle; }

   void Distance(double const& newVal) { flDistance = newVal; }
   void Angle(double const& newVal) { flAngle = newVal; }

};


class Location {
   friend std::ostream& operator << (std::ostream& out, Location const& data);
#if defined WITH_MOVE
   friend void swap(Location& lhs, Location& rhs) noexcept { lhs.swap(rhs); }
#endif
private:
   double flLatitude;
   double flLongitude;
public: 
   Location(double const& a, double const& b);
   Location(void) : Location(0.0, 0.0) { }
   Location(Location const&) = default;
#if defined WITH_MOVE
   Location(Location&& ref) = default;
#endif

   Location& operator = (Location const& ref);
#if defined WITH_MOVE
   Location& operator = (Location && ref) noexcept;
   void swap(Location& ref) noexcept;
#endif
   double const& Latitude() const { return flLatitude;  }
   double const& Longitude() const { return flLongitude; }

   void Latitude(double const& newVal) { flLatitude = newVal; }
   void Longitude(double const& newVal) { flLongitude = newVal; }

   double distance(Location pos) const;
   double bearing(Location pos) const;
private:
   static double deg2rad(double deg);
   static double rad2deg(double rad);
};




