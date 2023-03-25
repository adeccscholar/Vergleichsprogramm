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

template <typename ty>
struct Location : public std::pair<ty, ty> {
   friend std::ostream& operator << (std::ostream& out, Location<ty> const& data) {
      out << "(" << data.first << ", " << data.second << ")";
      return out;
      }

   friend void swap(Location& lhs, Location&rhs) {
      swap(static_cast<std::pair<ty, ty>&>(lhs), static_cast<std::pair<ty, ty>&>(rhs));
      }

   Location(ty const& a, ty const& b) {
      static_assert(std::is_floating_point<ty>::value == true, "wrong type for Location");
      this->first = a; this->second = b;
      }

   Location(void) : Location(0.0, 0.0) { }
   Location(Location const&) = default;
   Location(Location&& ref) = default;

   Location& operator = (Location const& ref) {
      this->first = ref.first;
      this->second = ref.second;
      return *this;
      }

   ty Latitude() const { return this->first;  }
   ty Longitude() const { return this->second; }
};


template <typename ty, std::enable_if_t<std::is_floating_point<ty>::value, bool> = true>
using Result = std::pair<ty, ty>;


