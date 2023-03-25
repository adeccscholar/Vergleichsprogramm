#pragma once
#define _USE_MATH_DEFINES
#include <memory>
#include <cmath>
#include <cfenv>
#include <algorithm>
#include <execution>

using pairLocation = std::pair<double, double>;
using pairDistance = std::pair<double, double>;

template <typename ty>
inline void CalculateFor(ty start, ty end, pairLocation const& pointA) {
	static constexpr auto my_round = [](double const& val) {
		//std::fesetround(FE_UPWARD);
		//return std::nearbyint(val * 1000.0) / 1000.0;
		return std::round(val * 1000.0) / 1000.0;
	   };

	static const double r = 6371000.785;                     //< mean radius of the earth
	static const double w1 = M_PI / 180.0;
	static const double w2 = 180.0 / M_PI;
	const double phiA     = pointA.first  * w1; /// 180.0 * M_PI;
	const double lambdaA  = pointA.second * w1; /// 180.0 * M_PI;
   const double sin_phiA = std::sin(phiA);
   const double cos_phiA = std::cos(phiA);
   std::for_each(std::execution::par, start, end, [phiA, lambdaA, sin_phiA, cos_phiA](auto& val ) {
	   double phiB = val.Latitude() * w1; /// 180.0 * M_PI;
		double lambdaB = val.Longitude() * w1; /// 180.0 * M_PI;
		double zeta = std::acos(sin_phiA * std::sin(phiB) + cos_phiA * std::cos(phiB) * std::cos(lambdaB - lambdaA));
		double tmp  = (std::sin(phiB) - sin_phiA * std::cos(zeta)) / (cos_phiA * std::sin(zeta));
		double alpha = tmp < -1 ? 180.0 : tmp > 1 ? 0 : std::acos(tmp) * w2;
		//double alpha = std::acos( (std::sin(phiB) - sin_phiA * std::cos(zeta)) / (cos_phiA * std::sin(zeta)))* w2;
		/*
		if (std::isnan(alpha)) {
			alpha = (std::sin(phiB) - sin_phiA * std::cos(zeta)) / (cos_phiA * std::sin(zeta)) < -1.0 ? 180.0 : 0.0;
		   }
		else alpha *= w2;
      */
		val.Distance(my_round(zeta * r));
      val.Angle(my_round(lambdaA > lambdaB ? phiA > 0 ? 360.0 - alpha : 180.0 + alpha : phiA > 0 ? alpha : 180.0 - alpha));
      });
   }

inline auto Calculate(pairLocation const& pointA, pairLocation const& pointB) {
	static constexpr auto my_round = [](double const& val) {
      return std::round(val * 1000.0) / 1000.0;
	   };

	static const double r = 6371000.785;                     //< mean radius of the earth
	static const double w1 = M_PI / 180.0;
	static const double w2 = 180.0 / M_PI;
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
