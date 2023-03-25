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

   friend void swap(Location& lhs, Location&rhs) noexcept {
      //swap(static_cast<std::pair<ty, ty>&>(lhs), static_cast<std::pair<ty, ty>&>(rhs));
      lhs.swap(rhs);
      }

   Location(ty const& a, ty const& b) {
      static_assert(std::is_floating_point<ty>::value == true, "wrong type for Location");
      this->first = a; this->second = b;
      }

   Location(void) : Location(0.0, 0.0) { }
   Location(Location const&) = default;
   Location(Location&& ref) noexcept { swap(ref); };

   Location& operator = (Location const& ref) {
      this->first = ref.first;
      this->second = ref.second;
      return *this;
      }

   void swap(Location& rhs) noexcept {
      std::pair<ty, ty>::swap(rhs);
      }

};


template <typename ty, std::enable_if_t<std::is_floating_point<ty>::value, bool> = true>
using Result = std::pair<ty, ty>;





template< typename ty>
inline Result<ty> Calculate(Location<ty> const& pointA, Location<ty> const& pointB) {
   static constexpr auto my_round = [](double const& val) {
      return std::round(val * 1000.0) / 1000.0;
   };

   static const double r = 6371000.785;                     //< mean radius of the earth
#if defined __BORLANDC__
   static const double w1 = M_PI / 180.0;
   static const double w2 = 180.0 / M_PI;
#else
 //  static constexpr double pi = 3.14159265358979323846;
   static constexpr double pi = std::numbers::pi_v<double>;
   static const double w1 = pi / 180.0;
   static const double w2 = 180.0 / pi;
#endif
   double phiA = pointA.first * w1; /// 180.0 * M_PI;
   double lambdaA = pointA.second * w1; /// 180.0 * M_PI;
   double phiB = pointB.first * w1; /// 180.0 * M_PI;
   double lambdaB = pointB.second * w1; /// 180.0 * M_PI;
   double zeta = std::acos(std::sin(phiA) * std::sin(phiB) + std::cos(phiA) * std::cos(phiB) * std::cos(lambdaB - lambdaA));
   double alpha = std::acos((std::sin(phiB) - std::sin(phiA) * std::cos(zeta)) / (std::cos(phiA) * std::sin(zeta)));
   if (std::isnan(alpha)) {
      alpha = (std::sin(phiB) - std::sin(phiA) * std::cos(zeta)) / (std::cos(phiA) * std::sin(zeta)) < -1.0 ? 180.0 : 0.0;
   }
   else alpha = my_round(alpha * w2);
   return std::make_pair(my_round(zeta * r), lambdaA > lambdaB ? phiA > 0 ? 360.0 - alpha : 180.0 + alpha : phiA > 0 ? alpha : 180.0 - alpha);
}


