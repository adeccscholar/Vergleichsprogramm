#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>  // ostream
#include <utility>   // pair
#include <type_traits>
#if !defined __BORLANDC__
#include <numbers>
#endif


/**
 * @brief Floating point data type to be used for storing the geographic position.
 * @tparam ty floating point type for the geografic location
*/
/*
template <typename ty, std::enable_if_t<std::is_floating_point<ty>::value, bool> = true>
using Location = std::pair<ty, ty>;

template <typename ty>
inline std::ostream& operator << (std::ostream& out, Location<ty> const& data) {
   out << "(" << data.first << ", " << data.second << ")";
   return out;
}

*/

class Location {
   friend std::ostream& operator << (std::ostream& out, Location const& data);
   friend void swap(Location& lhs, Location& rhs) noexcept { lhs.swap(rhs); }

private:
   double flLatitude;
   double flLongitude;

public:
   
   Location(double const& a, double const& b);

   Location(void) : Location(0.0, 0.0) { }
   Location(Location const&) = default;
   Location(Location&& ref) = default;

   Location& operator = (Location const& ref);
   void swap(Location& ref) noexcept;

   double const& Latitude() const { return flLatitude;  }
   double const& Longitude() const { return flLongitude; }

   void Latitude(double const& newVal) { flLatitude = newVal; }
   void Longitude(double const& newVal) { flLongitude = newVal; }

};


using Result = std::pair<double, double>;


