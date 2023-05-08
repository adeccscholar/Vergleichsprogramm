#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>  // ostream
#include <utility>   // pair
#include <type_traits>
#include <tuple>
#if !defined __BORLANDC__
#include <numbers>
#endif

template <std::floating_point ty>
using myETRS89 = std::tuple<int, ty, ty>;

template <std::floating_point ty>
using myMercatorETRS89 = std::pair<ty, ty>;

template <std::floating_point ty>
inline std::ostream& operator << (std::ostream& out, myMercatorETRS89<ty> const& data) {
   out << "(" << data.first << ", " << data.second << ")";
   return out;
}


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

template <std::floating_point ty>
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

   Location(myETRS89<ty> const& loc) {
      auto val = ConvertETRS89ToWGS84(loc);
      this->first  = val.first;
      this->second = val.second;
      }

   Location(myMercatorETRS89<ty> const& loc) {
      auto val = ConvertMercatorETRS89ToWGS84(loc);
      this->first = val.first;
      this->second = val.second;
      return *this;
      }

   Location& operator = (Location const& ref) {
      this->first = ref.first;
      this->second = ref.second;
      return *this;
      }

   Location& operator = (myETRS89<ty> const& loc) {
      auto val = ConvertETRS89ToWGS84(loc);
      this->first  = val.first;
      this->second = val.second;
      return *this;
      }

   Location& operator = (myMercatorETRS89<ty> const& loc) {
      auto val = ConvertMercatorETRS89ToWGS84(loc);
      this->first = val.first;
      this->second = val.second;
      return *this;
      }
   

   void swap(Location& rhs) noexcept {
      std::pair<ty, ty>::swap(rhs);
      }


protected:
   static inline constinit ty const mean_earth_radius  = 6366197.724;
   static inline constinit ty const polar_earth_radius = 6399593.625;
   static inline constinit ty const flattening         = 0.006739496742;
   static inline constinit ty const scale_factor       = 0.9996;
   static inline constinit ty const scale_polar_radius = scale_factor * polar_earth_radius;
public:
   /*
    * Diese Funktion verwendet den Algorithmus der  Java Methode "UTM2Deg", die unter der CC BY-SA-Lizenz steht.
    * Gem�� den Bedingungen dieser Lizenz wird diese Funktion ebenfalls unter CC BY-SA freigegeben
    * und der Urheberrechtsvermerk f�r die C++ Methode "ConvertETRS89ToWGS84" wird beibehalten.
    * Quelle: https://stackoverflow.com/questions/176137/java-convert-lat-lon-to-utm
    * � 2023 Stack Exchange Inc; user contributions licensed under CC BY-SA, User: user2548538
    *
    * zus�tzlich spezielle Funktion, letter nur wichtig f�r Halbkugel Letter > 'M' -> Nord, wenn S�d, dann  Northing -= 10000000
    */
   static auto ConvertETRS89ToWGS84(int const& zone, ty const& Easting, ty const& Northing) {
      static constexpr auto my_round = [](ty const& val) {
         return std::round(val * 10000000.0) / 10000000.0;
         };

      Location<ty> loc;

      ty const N1 = Northing / mean_earth_radius / scale_factor;
      ty const cosN1 = std::cos(N1);
      ty const sin2N1 = std::sin(2 * N1);
      ty const pow_cosN1_2 = std::pow(cosN1, 2);
      ty const factor2 = std::sqrt((1 + flattening * pow_cosN1_2));
      ty const factor3 = flattening * std::pow(Easting / (scale_polar_radius / factor2), 2);

      Easting -= 500000;

      loc.first = (N1 + (1 + flattening * pow_cosN1_2 - flattening * std::sin(N1) * cosN1 *
         (std::atan(std::cos(std::atan((std::exp(Easting / (scale_polar_radius / factor2) * (1 - factor3 / 2 * pow_cosN1_2 / 3)) -
            std::exp(-Easting / (scale_polar_radius / factor2) * (1 - factor3 / 2 * pow_cosN1_2 / 3))) / 2 / std::cos((Northing - scale_polar_radius *
               (N1 - flattening * 3 / 4 * (N1 + sin2N1 / 2) + std::pow(flattening * 3 / 4, 2) * 5 / 3 *
                  (3 * (N1 + sin2N1 / 2) + sin2N1 * pow_cosN1_2) / 4 - std::pow(flattening * 3 / 4, 3) * 35 / 27 * (5 * (3 * (N1 +
                     sin2N1 / 2) + sin2N1 * pow_cosN1_2) / 4 + sin2N1 * pow_cosN1_2 * pow_cosN1_2) / 3)) /
               (scale_polar_radius / factor2) * (1 - factor3 / 2 * pow_cosN1_2) + N1))) * std::tan((Northing - scale_polar_radius *
                 (N1 - flattening * 3 / 4 * (N1 + sin2N1 / 2) + std::pow(flattening * 3 / 4, 2) * 5 / 3 * (3 * (N1 + sin2N1 / 2) +
                     sin2N1 * pow_cosN1_2) / 4 - std::pow(flattening * 3 / 4, 3) * 35 / 27 * (5 * (3 * (N1 + sin2N1 / 2) + sin2N1 *
                       pow_cosN1_2) / 4 + sin2N1 * pow_cosN1_2 * pow_cosN1_2) / 3)) / (scale_polar_radius / factor2) *
                       (1 - factor3 / 2 * pow_cosN1_2) + N1)) - N1) * 3 / 2) *
         (std::atan(std::cos(std::atan((std::exp(Easting / (scale_polar_radius / factor2) *
            (1 - flattening * std::pow(Easting / (scale_polar_radius / std::sqrt((1 + flattening *
               pow_cosN1_2))), 2) / 2 * pow_cosN1_2 / 3)) - std::exp(-Easting / (scale_polar_radius / factor2) * (1 - factor3 / 2 *
               pow_cosN1_2 / 3))) / 2 / std::cos((Northing - scale_polar_radius * (N1 - flattening * 3 / 4 * (N1 +
                  sin2N1 / 2) + std::pow(flattening * 3 / 4, 2) * 5 / 3 *
                  (3 * (N1 + sin2N1 / 2) + sin2N1 * pow_cosN1_2) / 4 -
                  std::pow(flattening * 3 / 4, 3) * 35 / 27 * (5 * (3 * (N1 +
                     sin2N1 / 2) + sin2N1 * pow_cosN1_2) / 4 + sin2N1 * pow_cosN1_2 * pow_cosN1_2) / 3)) /
                  (scale_polar_radius / factor2) * (1 - factor3 / 2 * pow_cosN1_2) +
                  N1))) * std::tan((Northing - scale_polar_radius *
                     (N1 - flattening * 3 / 4 * (N1 + sin2N1 / 2) + std::pow(flattening * 3 / 4, 2) * 5 / 3 *
                        (3 * (N1 + sin2N1 / 2) + sin2N1 * pow_cosN1_2) / 4 -
                        std::pow(flattening * 3 / 4, 3) * 35 / 27 * (5 * (3 * (N1 +
                           sin2N1 / 2) + sin2N1 * pow_cosN1_2) / 4 + sin2N1 *
                           pow_cosN1_2 * pow_cosN1_2) / 3)) / (scale_polar_radius / factor2) *
                     (1 - factor3 / 2 * pow_cosN1_2) + N1)) - N1)) * 180 / std::numbers::pi;

      loc.first = my_round(loc.first);
      
      loc.second = std::atan((std::exp(Easting / (scale_polar_radius / factor2) *
         (1 - factor3 / 2 *
            pow_cosN1_2 / 3)) - std::exp(-Easting / (scale_polar_radius / factor2) * (1 - flattening *
               std::pow(Easting / (scale_polar_radius / factor2), 2) / 2 *
               pow_cosN1_2 / 3))) / 2 /
         std::cos((Northing - scale_polar_radius * (N1 - flattening * 3 / 4 *
            (N1 + sin2N1 / 2) +
            std::pow(flattening * 3 / 4, 2) * 5 / 3 * (3 * (N1 +
               sin2N1 / 2) + sin2N1 * pow_cosN1_2) / 4 - std::pow(flattening * 3 / 4, 3) * 35 / 27 * (5 * (3 * (N1 +
                  sin2N1 / 2) + sin2N1 * pow_cosN1_2) / 4 + sin2N1 * pow_cosN1_2 * pow_cosN1_2) / 3)) /
            (scale_polar_radius / factor2) * (1 - factor3 /
               2 * pow_cosN1_2) + N1)) * 180 /
         std::numbers::pi + zone * 6 - 183;

      loc.second = my_round(loc.second);
      return loc;
      }

   static auto ConvertMercatorETRS89ToWGS84(myMercatorETRS89<ty> const& mercator) {
      /* Schutz vor falschen Koordinaten*/
      if (mercator.first < -20037508.3427892 || mercator.first  > 20037508.3427892 ||
         mercator.second < -20037508.3427892 || mercator.second > 20037508.3427892) {
         throw std::runtime_error("invalid params for function \"ConvertMercatorToWGS84\"");
         }

      Location<ty> loc;
      auto& [x, y] = mercator;
      double x1 = x / 6378137.0 * 57.295779513082323;
      loc.second = x1 - (std::floor((ty)((x1 + 180.0) / 360.0)) * 360.0);
      loc.first  = (1.5707963267948966 - (2.0 * std::atan(std::exp((-1.0 * y) / 6378137.0)))) * 57.295779513082323;
      return loc;
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
   static constexpr double pi = std::numbers::pi_v<double>;
   static const double w1 = pi / 180.0;
   static const double w2 = 180.0 / pi;

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


/*
 * Diese Funktion verwendet den Algorithmus der  Java Methode "UTM2Deg", die unter der CC BY-SA-Lizenz steht.
 * Gem�� den Bedingungen dieser Lizenz wird diese Funktion ebenfalls unter CC BY-SA freigegeben
 * und der Urheberrechtsvermerk f�r die C++ Methode "ConvertETRS89ToWGS84" wird beibehalten.
 * Quelle: https://stackoverflow.com/questions/176137/java-convert-lat-lon-to-utm
 * � 2023 Stack Exchange Inc; user contributions licensed under CC BY-SA, User: user2548538
 *
 * zus�tzlich spezielle Funktion, letter nur wichtig f�r Halbkugel Letter > 'M' -> Nord, wenn S�d, dann  Northing -= 10000000
 *
private class UTM2Deg
{
   double latitude;
   double longitude;
   private  UTM2Deg(String UTM)
   {
      String[] parts = UTM.split(" ");
      int Zone = Integer.parseInt(parts[0]);
      char Letter = parts[1].toUpperCase(Locale.ENGLISH).charAt(0);
      double Easting = Double.parseDouble(parts[2]);
      double Northing = Double.parseDouble(parts[3]);
      double Hem;
      if (Letter > 'M')
         Hem = 'N';
      else
         Hem = 'S';
      double north;
      if (Hem == 'S')
         north = Northing - 10000000;
      else
         north = Northing;

      latitude = (north / 6366197.724 / 0.9996 + (1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) - 0.006739496742 * Math.sin(north / 6366197.724 / 0.9996) * Math.cos(north / 6366197.724 / 0.9996) * (Math.atan(Math.cos(Math.atan((Math.exp(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) / 3)) - Math.exp(-Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) / 3))) / 2 / Math.cos((north - factor1 * (north / 6366197.724 / 0.9996 - 0.006739496742 * 3 / 4 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.pow(0.006739496742 * 3 / 4, 2) * 5 / 3 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 - Math.pow(0.006739496742 * 3 / 4, 3) * 35 / 27 * (5 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 3)) / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) + north / 6366197.724 / 0.9996))) * Math.tan((north - factor1 * (north / 6366197.724 / 0.9996 - 0.006739496742 * 3 / 4 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.pow(0.006739496742 * 3 / 4, 2) * 5 / 3 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 - Math.pow(0.006739496742 * 3 / 4, 3) * 35 / 27 * (5 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 3)) / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) + north / 6366197.724 / 0.9996)) - north / 6366197.724 / 0.9996) * 3 / 2) * (Math.atan(Math.cos(Math.atan((Math.exp(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) / 3)) - Math.exp(-Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) / 3))) / 2 / Math.cos((north - factor1 * (north / 6366197.724 / 0.9996 - 0.006739496742 * 3 / 4 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.pow(0.006739496742 * 3 / 4, 2) * 5 / 3 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 - Math.pow(0.006739496742 * 3 / 4, 3) * 35 / 27 * (5 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 3)) / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) + north / 6366197.724 / 0.9996))) * Math.tan((north - factor1 * (north / 6366197.724 / 0.9996 - 0.006739496742 * 3 / 4 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.pow(0.006739496742 * 3 / 4, 2) * 5 / 3 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 - Math.pow(0.006739496742 * 3 / 4, 3) * 35 / 27 * (5 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 3)) / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) + north / 6366197.724 / 0.9996)) - north / 6366197.724 / 0.9996)) * 180 / Math.PI;
      latitude = Math.round(latitude * 10000000);
      latitude = latitude / 10000000;
      longitude = Math.atan((Math.exp(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) / 3)) - Math.exp(-Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) / 3))) / 2 / Math.cos((north - factor1 * (north / 6366197.724 / 0.9996 - 0.006739496742 * 3 / 4 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.pow(0.006739496742 * 3 / 4, 2) * 5 / 3 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 - Math.pow(0.006739496742 * 3 / 4, 3) * 35 / 27 * (5 * (3 * (north / 6366197.724 / 0.9996 + Math.sin(2 * north / 6366197.724 / 0.9996) / 2) + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 4 + Math.sin(2 * north / 6366197.724 / 0.9996) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2) * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) / 3)) / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))) * (1 - 0.006739496742 * Math.pow(Easting / (factor1 / Math.sqrt((1 + 0.006739496742 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)))), 2) / 2 * Math.pow(Math.cos(north / 6366197.724 / 0.9996), 2)) + north / 6366197.724 / 0.9996)) * 180 / Math.PI + Zone * 6 - 183;
      longitude = Math.round(longitude * 10000000);
      longitude = longitude / 10000000;
   }
}

WGS84 steht f�r World Geodetic System 1984 und ist ein globales geod�tisches Referenzsystem. Es wurde vom US-amerikanischen 
Verteidigungsministerium entwickelt und im Jahr 1984 ver�ffentlicht. WGS84 ist heute eines der am h�ufigsten verwendeten 
geod�tischen Referenzsysteme weltweit und wird von vielen Navigationssystemen wie GPS und GLONASS verwendet.

WGS84 ist ein dreidimensionales Koordinatensystem, das die Koordinaten von Punkten auf der Erdoberfl�che durch die Verwendung 
von geod�tischen Messungen und Satellitendaten beschreibt. Es ist eine gemeinsame Referenz f�r die globale Vermessung und 
die Navigation.

WGS84 ist wichtig f�r eine Vielzahl von Anwendungen, wie zum Beispiel f�r GPS-Navigation, Luft- und Raumfahrt, Geologie, 
Kartographie und Geod�sie. Es erm�glicht eine pr�zise Bestimmung der Position und H�he von Objekten auf der Erdoberfl�che 
und erleichtert so die Navigation und Vermessung auf globaler Ebene.

WGS84 ist ein dynamisches System, das regelm��ig aktualisiert wird, um Ver�nderungen in der Erdoberfl�che und der Rotation 
der Erde zu ber�cksichtigen. Neue Versionen werden ver�ffentlicht, um die Genauigkeit zu verbessern und das System auf dem 
neuesten Stand zu halten.



ETRS89 steht f�r European Terrestrial Reference System 1989 und ist ein geod�tisches Referenzsystem f�r Europa. Es wurde entwickelt, 
um eine einheitliche Grundlage f�r die Vermessung und Kartierung von L�ndern in Europa bereitzustellen. Das System wurde 1989 
festgelegt und ist seither als Standard f�r geod�tische Vermessungen in Europa anerkannt.

ETRS89 ist ein dreidimensionales geod�tisches Referenzsystem, das die Koordinaten von Punkten auf der Erdoberfl�che durch die 
Verwendung von geod�tischen Messungen und Satellitendaten beschreibt. Es basiert auf einem globalen Koordinatensystem namens 
International Terrestrial Reference System (ITRS), das durch ein Netzwerk von Bodenstationen und Satelliten bestimmt wird.

ETRS89 wird in vielen Anwendungen wie der Geod�sie, Kartographie, Navigation und Luft- und Raumfahrt verwendet. Es dient als 
Grundlage f�r die Erstellung von topographischen Karten, geografischen Informationssystemen (GIS), GPS-Navigation und f�r die 
Planung von Bauprojekten.

In Europa wird ETRS89 als gemeinsames Referenzsystem f�r geod�tische Vermessungen und f�r den Austausch von geod�tischen Daten 
zwischen verschiedenen L�ndern verwendet. Es erm�glicht eine pr�zise Berechnung von Koordinaten auf der Erdoberfl�che und tr�gt 
damit zur Vereinfachung von grenz�berschreitenden Vermessungen und zur Verbesserung der Genauigkeit bei.




Die Mercator-Projektion von ETRS89 ist eine Methode, um geografische Koordinaten auf der gekr�mmten Erdoberfl�che des 
geod�tischen Referenzsystems ETRS89 auf eine flache Karte zu projizieren.

Die Mercator-Projektion von ETRS89 nutzt dieselbe Methode wie die klassische Mercator-Projektion, um die Erdoberfl�che 
auf eine flache Karte zu projizieren. Dabei wird ein Zylinder um die Erde gelegt und die Erdoberfl�che auf diesen 
Zylinder projiziert.

Die Meridianlinien werden in der Mercator-Projektion von ETRS89 als parallele Geraden dargestellt, die senkrecht zum 
�quator verlaufen. Die Breitenkreise werden als parallele Geraden dargestellt, die senkrecht zu den Meridianlinien 
stehen. Dadurch wird die Karte leichter lesbar und vereinfacht die Navigation.

Die Mercator-Projektion von ETRS89 ist jedoch auch mit Verzerrungen verbunden, insbesondere in h�heren Breiten. 
Die Karte kann in diesen Regionen stark verzerrt sein, was zu falschen Entfernungen und Verzerrungen von Fl�chen 
f�hren kann. Aus diesem Grund werden heute in der Regel andere Projektionen wie die UTM-Projektion oder Lambert-Projektion 
verwendet, die diese Verzerrungen reduzieren.


flattering
Die Konstante 0.006739496742 ist ein Wert, der bei der Umrechnung von Geokoordinaten in kartesische Koordinaten oder 
umgekehrt verwendet wird. Genauer gesagt handelt es sich um den Wert der "elliptischen Abplattung" oder 
"Abplattung des Erdellipsoids", die ein Ma� f�r die Abweichung der Erde von der idealen Kugelform darstellt.

Wenn man die Erde als Kugel betrachtet, hat sie einen Radius von etwa 6.371 Kilometern. In der Realit�t ist die 
Erde jedoch an den Polen etwas abgeflacht und am �quator etwas aufgebl�ht, was auf die Rotation der Erde zur�ckzuf�hren 
ist. Die elliptische Abplattung beschreibt dieses Ph�nomen und gibt an, wie stark die Erde an den Polen abgeflacht ist.

Die Konstante 0.006739496742 wird in Formeln zur Berechnung von geod�tischen Entfernungen verwendet, die die 
elliptische Abplattung ber�cksichtigen. Eine solche Formel ist zum Beispiel die Haversine-Formel, die den Abstand 
zwischen zwei Punkten auf der Erdoberfl�che berechnet. Durch Verwendung dieser Konstante in der Haversine-Formel kann 
man also genauere Entfernungen zwischen zwei geografischen Koordinaten berechnen, als wenn man die Erde als perfekte 
Kugel betrachten w�rde.



scale_factor
Die Konstante 0.9996 wird in der Vermessungstechnik und Kartografie als Ma�stabfaktor verwendet. Der Ma�stabfaktor 
ist ein Verh�ltnis zwischen der tats�chlichen Gr��e eines Objekts und seiner Darstellung auf einer Karte oder einem Plan.

In der Vermessungstechnik wird die Erdoberfl�che aufgrund ihrer kugelf�rmigen Form in verschiedene geod�tische 
Koordinatensysteme unterteilt. Das bekannteste davon ist das WGS84-System, das von GPS-Ger�ten verwendet wird. 
Um eine Karte oder einen Plan zu erstellen, muss die gekr�mmte Erdoberfl�che jedoch auf eine flache Ebene projiziert 
werden, was eine Verzerrung der Abmessungen und Formen von Fl�chen und Objekten zur Folge hat.

Der Ma�stabfaktor 0.9996 wird speziell in der Projektion UTM (Universal Transverse Mercator) verwendet, die die 
Erdoberfl�che in 60 Zonen unterteilt und jede dieser Zonen in eine flache Ebene projiziert. Der Ma�stabfaktor von 
0.9996 wird verwendet, um die Verzerrungen, die durch die Projektion der gekr�mmten Erdoberfl�che auf die flache Ebene 
entstehen, auszugleichen. Dadurch wird eine relativ genaue Abbildung der Fl�chen und Objekte auf der Karte erreicht.

In der Praxis bedeutet dies, dass eine Entfernung von 1.000 Metern auf der Erdoberfl�che auf der UTM-Karte mit einem 
Ma�stab von 0.9996 multipliziert wird, um die korrekte Entfernung auf der Karte zu erhalten. In diesem Fall ergibt sich 
eine Entfernung von 999,6 Metern.


mean_earth_radius
Die Zahl 6366197.724 ist ein Wert f�r den mittleren Erdradius. Der mittlere Erdradius ist der Radius einer fiktiven 
Kugel, die das Volumen und die Masse der Erde repr�sentiert. Die Erde ist jedoch keine perfekte Kugel, sondern aufgrund 
ihrer Rotation an den Polen abgeflacht und am �quator aufgebl�ht. Daher gibt es f�r die Erde verschiedene Radien, die 
je nach Anwendung verwendet werden.

Die Zahl 6366197.724 Meter wird h�ufig in der Geod�sie, Geophysik und Luft- und Raumfahrt verwendet, um den mittleren 
Erdradius zu repr�sentieren. Sie wird auch in vielen Anwendungen verwendet, bei denen die Entfernungen auf der 
Erdoberfl�che berechnet werden, wie zum Beispiel bei GPS oder geod�tischen Vermessungen.

Es ist wichtig zu beachten, dass dies nur ein N�herungswert ist und dass die tats�chliche Form der Erde aufgrund 
ihrer Abplattung und unregelm��igen Oberfl�che komplexer ist. Wenn eine h�here Genauigkeit ben�tigt wird, m�ssen 
komplexere Modelle und Messungen verwendet werden, um den Radius an jedem Punkt auf der Erdoberfl�che zu bestimmen.


polar_earth_radius
Die Zahl 6399593.625 ist der Polarradius der Erde, auch bekannt als der polare Erdradius. Dieser Wert gibt an, wie gro� 
der Radius der Erde an den Polen ist, wo die Erde am st�rksten abgeflacht ist. Im Gegensatz dazu ist der �quatorradius 
der Erde gr��er aufgrund der Ausbuchtung am �quator.

Der Polarradius der Erde ist ein wichtiger Parameter in der Geod�sie und Geophysik, da er bei der Bestimmung von Messungen, 
die auf der Erdoberfl�che durchgef�hrt werden, ber�cksichtigt werden muss. Zum Beispiel kann die genaue Bestimmung der 
Erdrotation oder die Berechnung der Schwerkraft auf der Erdoberfl�che nur durch Ber�cksichtigung des Polarradius erreicht werden.

Es ist wichtig zu beachten, dass die Erde aufgrund von Faktoren wie tektonischen Aktivit�ten, Ver�nderungen in der 
Masseverteilung und Gezeitenkr�ften ein dynamisches System ist. Daher kann sich der Polarradius der Erde im Laufe der 
Zeit geringf�gig �ndern. Der derzeit anerkannte Wert f�r den Polarradius der Erde betr�gt etwa 6.399.593,625 Meter.


Umrechnung von Mercator-Projektion von ETRS89 zu WGS84
Die Multiplikation von 6378137.0 mit 57.295779513082323 dient der Umrechnung der Einheit Meter auf Grad, 
da ETRS89 auf Meter basiert, w�hrend WGS84 auf Grad basiert.

Die Zahl 6378137.0 ist der mittlere Erdradius in Metern, der f�r die Mercator-Projektion von ETRS89 verwendet wird. 
Die Zahl 57.295779513082323 ist der Faktor f�r die Umrechnung von Radian in Grad.

In der Mercator-Projektion von ETRS89 werden L�ngen- und Breitengrade in Metern gemessen. Wenn Sie jedoch die 
Koordinaten in das geod�tische Referenzsystem WGS84 umrechnen m�chten, m�ssen Sie sie in Grad umwandeln. Dies wird 
durch Multiplikation der L�ngen- und Breitengrade mit dem Faktor 57.295779513082323 erreicht, der den Winkel in Grad 
anstelle von Radian angibt.

Daher bedeutet 6378137.0 * 57.295779513082323 in der Umrechnung der Mercator-Projektion von ETRS89 zu WGS84 die 
Umwandlung der Koordinaten von Metern in Grad, die f�r die WGS84-Projektion erforderlich sind.
*/