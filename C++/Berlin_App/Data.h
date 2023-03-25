//---------------------------------------------------------------------------
#ifndef DataH
#define DataH
//---------------------------------------------------------------------------
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include "GeoCalc.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <memory>
#include <tuple>
#include <type_traits>
#include <algorithm>
#include <locale>
#include <string_view>
#include <charconv>


using tplStreets   = std::tuple<std::string, std::string, std::string, std::string>;

using tplData      = std::tuple<std::string,  //  0 - City
                                std::string,  //  1 - Street
                                std::string,  //  2 - StreetNumber
                                std::string,  //  3 - ZipCode
                                std::string,  //  4 - Urban_Unit
                                std::string,  //  5 - Old_Unit
                                std::string,  //  6 - District
                                double,       //  7 - Latitude
                                double,       //  8 - Longitude
                                double,       //  9 - Distance
                                double>;      // 10 - Angle
                              

struct TMyNum : public std::numpunct<char> {
	char_type   do_decimal_point() const { return ','; }
	char_type   do_thousands_sep() const { return '.'; }
	std::string do_grouping()      const { return "\3"; }
   };



class TData {
	friend std::ostream& operator << (std::ostream& out, TData const& data) {
		out << data.ZipCode() << " " << data.City() << " - " << data.District() << ", "
          << data.Street() << " " << data.StreetNumber();
      return out;
	}

	friend void swap(TData &a, TData &b) {
		std::swap(a.strCity,         b.strCity);
		std::swap(a.strStreet,       b.strStreet);
		std::swap(a.strStreetNumber, b.strStreetNumber);
		std::swap(a.strZipCode,      b.strZipCode);
		std::swap(a.strUrban_Unit,   b.strUrban_Unit);
		std::swap(a.strOld_Unit,     b.strOld_Unit);
		std::swap(a.strDistrict,     b.strDistrict);
		std::swap(a.flLatitude,      b.flLatitude);
		std::swap(a.flLongitude,     b.flLongitude);
		std::swap(a.flDistance,      b.flDistance);
		std::swap(a.flAngle,         b.flAngle);
	   }

   private:
	  std::string strCity         = "";
	  std::string strStreet       = "";
	  std::string strStreetNumber = "";
	  std::string strZipCode      = "";
	  std::string strUrban_Unit   = "";
	  std::string strOld_Unit     = "";
	  std::string strDistrict     = "";
	  double      flLatitude      = double();
	  double      flLongitude     = double();
	  double      flDistance      = double();
	  double      flAngle         = double();

   public:
	  TData(void) = default;

	  TData(TData const& ref) {
		_copy(ref);
		 }

	  TData(TData && ref) noexcept {
		  /*
		  std::exchange(strCity, ref.strCity);
		  std::exchange(strStreet, ref.strStreet);
		  std::exchange(strStreetNumber, ref.strStreetNumber);
		  std::exchange(strZipCode, ref.strZipCode);
		  std::exchange(strUrban_Unit, ref.strUrban_Unit);
		  std::exchange(strOld_Unit, ref.strOld_Unit);
		  std::exchange(strDistrict, ref.strDistrict);
		  std::exchange(flLatitude, ref.flLatitude);
		  std::exchange(flLongitude, ref.flLongitude);
		  std::exchange(flDistance, ref.flDistance);
		  std::exchange(flAngle, ref.flAngle);
		  */
		  ///*
		  std::swap(strCity,         ref.strCity);
		  std::swap(strStreet,       ref.strStreet);
		  std::swap(strStreetNumber, ref.strStreetNumber);
		  std::swap(strZipCode,      ref.strZipCode);
		  std::swap(strUrban_Unit,   ref.strUrban_Unit);
		  std::swap(strOld_Unit,     ref.strOld_Unit);
		  std::swap(strDistrict,     ref.strDistrict);
		  std::swap(flLatitude,      ref.flLatitude);
		  std::swap(flLongitude,     ref.flLongitude);
		  std::swap(flDistance,      ref.flDistance);
		  std::swap(flAngle,         ref.flAngle);
		  //*/
		  }

	  virtual ~TData(void) { }

	  TData& operator = (TData const& ref) noexcept {
		 _copy(ref);
		 return *this;
		 }

	  TData& operator = (TData && ref) noexcept {
		  /*
		  std::exchange(strCity, ref.strCity);
		  std::exchange(strStreet, ref.strStreet);
		  std::exchange(strStreetNumber, ref.strStreetNumber);
		  std::exchange(strZipCode, ref.strZipCode);
		  std::exchange(strUrban_Unit, ref.strUrban_Unit);
		  std::exchange(strOld_Unit, ref.strOld_Unit);
		  std::exchange(strDistrict, ref.strDistrict);
		  std::exchange(flLatitude, ref.flLatitude);
		  std::exchange(flLongitude, ref.flLongitude);
		  std::exchange(flDistance, ref.flDistance);
		  std::exchange(flAngle, ref.flAngle);
		  */
		  ///*
		  std::swap(strCity,         ref.strCity);
		  std::swap(strStreet,       ref.strStreet);
		  std::swap(strStreetNumber, ref.strStreetNumber);
		  std::swap(strZipCode,      ref.strZipCode);
		  std::swap(strUrban_Unit,   ref.strUrban_Unit);
		  std::swap(strOld_Unit,     ref.strOld_Unit);
		  std::swap(strDistrict,     ref.strDistrict);
		  std::swap(flLatitude,      ref.flLatitude);
		  std::swap(flLongitude,     ref.flLongitude);
		  std::swap(flDistance,      ref.flDistance);
		  std::swap(flAngle,         ref.flAngle);
		  //*/
		 return *this;
		 }

	  TData& operator = (pairLocation val) {
		  flDistance = val.first;
		  flAngle = val.second;
		  return *this;
	     }

     operator pairLocation () const { return std::make_pair(flLatitude, flLongitude); }

	  // Selectors
	  std::string const& City(void) const         { return strCity; }
	  std::string const& Street(void) const       { return strStreet; }
	  std::string const& StreetNumber(void) const { return strStreetNumber; }
	  std::string const& ZipCode(void) const      { return strZipCode; }
	  std::string const& Urban_Unit(void) const   { return strUrban_Unit; }
	  std::string const& Old_Unit(void) const     { return strOld_Unit; }
	  std::string const& District(void) const     { return strDistrict; }
	  double      const& Latitude(void) const     { return flLatitude; }
	  double      const& Longitude(void) const    { return flLongitude; }
	  double      const& Distance(void) const     { return flDistance; }
	  double      const& Angle(void) const        { return flAngle; }

	  // Manipulators
	  std::string const& City(std::string const& newVal) { return strCity = newVal; }
	  std::string const& Street(std::string const& newVal) { return strStreet = newVal; }
	  std::string const& StreetNumber(std::string const& newVal) { return strStreetNumber = newVal; }
	  std::string const& ZipCode(std::string const& newVal) { return strZipCode = newVal; }
	  std::string const& Urban_Unit(std::string const& newVal) { return strUrban_Unit = newVal; }
	  std::string const& Old_Unit(std::string const& newVal) { return strOld_Unit = newVal; }
	  std::string const& District(std::string const& newVal) { return strDistrict = newVal; }
	  double      const& Latitude(double& newVal) { return flLatitude = newVal; }
	  double      const& Longitude(double& newVal) { return flLongitude = newVal; }
	  double      const& Distance(double const& newVal) { return flDistance = newVal; }
	  double      const& Angle(double const& newVal) { return flAngle = newVal; }

	  double      const& Latitude(std::string const& newVal) {  return flLatitude = stod(newVal); }
	  double      const& Longitude(std::string const& newVal) {  return flLongitude = std::stod(newVal);  }
	  //double      const& Latitude(std::string const& newVal) { return flLatitude = std::atof(newVal.c_str()); }
	  //double      const& Longitude(std::string const& newVal) { return flLongitude = std::atof(newVal.c_str()); }


	  void City(std::string_view const& newVal)         { strCity         = move(std::string { newVal.data(), newVal.size() }); }
	  void Street(std::string_view const& newVal)       { strStreet       = move(std::string { newVal.data(), newVal.size() }); }
	  void StreetNumber(std::string_view const& newVal) { strStreetNumber = move(std::string { newVal.data(), newVal.size() }); }
	  void ZipCode(std::string_view const& newVal)      { strZipCode      = move(std::string { newVal.data(), newVal.size() }); }
	  void Urban_Unit(std::string_view const& newVal)   { strUrban_Unit   = move(std::string { newVal.data(), newVal.size() }); }
	  void Old_Unit(std::string_view const& newVal)     { strOld_Unit     = move(std::string { newVal.data(), newVal.size() }); }
	  void District(std::string_view const& newVal)     { strDistrict     = move(std::string { newVal.data(), newVal.size() }); }
	  void Latitude(std::string_view const& newVal)     { 
		  auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), flLatitude);
        }
	  void Longitude(std::string_view const& newVal)    { 
		  auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), flLongitude);
		  }
	  void Distance(std::string_view const& newVal)     { 
		  auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), flDistance);
		  }
	  void Angle(std::string_view const& newVal)        { 
	     auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), flAngle);
	     }

	  std::string const& City(std::string && newVal) { return strCity = std::forward<std::string>(newVal); }
	  std::string const& Street(std::string && newVal) { return strStreet = std::forward<std::string>(newVal); }
	  std::string const& StreetNumber(std::string && newVal) { return strStreetNumber = std::forward<std::string>(newVal); }
	  std::string const& ZipCode(std::string && newVal) { return strZipCode = std::forward<std::string>(newVal); }
	  std::string const& Urban_Unit(std::string && newVal) { return strUrban_Unit = std::forward<std::string>(newVal); }
	  std::string const& Old_Unit(std::string && newVal) { return strOld_Unit = std::forward<std::string>(newVal); }
	  std::string const& District(std::string && newVal) { return strDistrict = std::forward<std::string>(newVal); }
	  //double      const& Latitude(std::string && newVal) { return flLatitude = std::stod(newVal); }
	  //double      const& Longitude(std::string && newVal) { return flLongitude = std::stod(newVal); }

	  double      const& Latitude(std::string&& newVal) {
		  auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), flLatitude);  // eventuell error handling about result
		  return flLatitude;
	      }
	  double      const& Longitude(std::string&& newVal) {
		  auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), flLongitude);  // eventuell error handling about result
		  return flLongitude;
	      }

	  double      const& Distance(std::string && newVal) { return flDistance = std::stod(newVal); }
	  double      const& Angle(std::string && newVal) { return flAngle = std::stod(newVal); }

	  #if defined __BORLANDC__
	  bool operator == (TData const& ref) const { return _compare(ref) == 0; }
	  bool operator != (TData const& ref) const { return _compare(ref) != 0; }
	  bool operator <  (TData const& ref) const { return _compare(ref) <  0; }
	  bool operator <= (TData const& ref) const { return _compare(ref) <= 0; }
	  bool operator >  (TData const& ref) const { return _compare(ref) >  0; }
	  bool operator >= (TData const& ref) const { return _compare(ref) >= 0; }
	  #else
	  int operator <=> (TData const& ref) const {
		  static auto compare_val = [](auto left, auto right) {
			  return left != right ? left < right ? -1 : 1 : 0;
		  };

		  if (int cmpCity = compare_val(City(), ref.City()); cmpCity == 0) {
			  if (int cmpUrban_Unit = compare_val(Urban_Unit(), ref.Urban_Unit()); cmpUrban_Unit == 0) {
				  if (int cmpDistrict = compare_val(District(), ref.District()); cmpDistrict == 0) {
						  if (int cmpStreet = compare_val(Street(), ref.Street()); cmpStreet == 0) {
							  return strStreetNumber < ref.StreetNumber() ? -1 : strStreetNumber > ref.StreetNumber() ? 1 : 0;
						  }
						  else return cmpStreet;
					  }
				  else return cmpDistrict;
			  }
			  else return cmpUrban_Unit;
		  }
		  else return cmpCity;
	  }
	  #endif

   private:
	  void _copy(TData const& ref) {
		 strCity         = ref.strCity;
		 strStreet       = ref.strStreet;
		 strStreetNumber = ref.strStreetNumber;
		 strZipCode      = ref.strZipCode;
		 strUrban_Unit   = ref.strUrban_Unit;
		 strOld_Unit     = ref.strOld_Unit;
		 strDistrict     = ref.strDistrict;
		 flLatitude      = ref.flLatitude;
		 flLongitude     = ref.flLongitude;
		 flDistance      = ref.flDistance;
		 flAngle         = ref.flAngle;
		 }

	  #if defined __BORLANDC__
	  int _compare(TData const& ref) const {
		  static auto compare_val = [](auto left, auto right) {
			  return left != right ? left < right ? -1 : 1 : 0;
		  };

		  if (int cmpCity = compare_val(City(), ref.City()); cmpCity == 0) {
			  if (int cmpUrban_Unit = compare_val(Urban_Unit(), ref.Urban_Unit()); cmpUrban_Unit == 0) {
				  if (int cmpDistrict = compare_val(District(), ref.District()); cmpDistrict == 0) {
						  if (int cmpStreet = compare_val(Street(), ref.Street()); cmpStreet == 0) {
							  return strStreetNumber < ref.StreetNumber() ? -1 : strStreetNumber > ref.StreetNumber() ? 1 : 0;
						  }
						  else return cmpStreet;
					  }
				  else return cmpDistrict;
			  }
			  else return cmpUrban_Unit;
		  }
		  else return cmpCity;
		  }
	  #endif
   };

using myData   = std::vector<TData>;
using itData   = myData::iterator;
using citData  = myData::const_iterator;


#endif

