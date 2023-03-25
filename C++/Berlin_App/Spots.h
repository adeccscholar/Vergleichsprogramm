#pragma once

#include "GeoCalc.h"
#include <string>
#include <locale>
#include <vector>
#include <algorithm>

class TSpots {
private:
	std::string strName     = "";       ///< name of the location / spot 
	std::string strCategory = "";       ///< category / kind of the location
	double      flLatitude  = 0.0;      ///< latitude
	double      flLongitude = 0.0;      ///< longitude

	std::string strCity   = "";         ///< city
	std::string strStreet = "";         ///< street
	std::string strStreetNumber = "";   ///< street number
	std::string strZipCode = "";        ///< zip code
	std::string strUrban_Unit = "";     ///< urban Unit
	std::string strDistrict = "";       ///< district 
	double      flDistance = 0.0;       ///< distance to the spot
	double      flAngle = 0.0;          ///< angle to the spot
public:
	TSpots(void) = default;

	TSpots(TSpots const& ref) {
		_copy(ref);
	}

	TSpots(TSpots&& ref) noexcept {
		std::swap(strName, ref.strName);
		std::swap(strCategory, ref.strCategory);
		std::swap(flLatitude, ref.flLatitude);
		std::swap(flLongitude, ref.flLongitude);
		std::swap(strCity, ref.strCity);
		std::swap(strStreet, ref.strStreet);
		std::swap(strStreetNumber, ref.strStreetNumber);
		std::swap(strZipCode, ref.strZipCode);
		std::swap(strUrban_Unit, ref.strUrban_Unit);
		std::swap(strDistrict, ref.strDistrict);
		std::swap(flDistance, ref.flDistance);
		std::swap(flAngle, ref.flAngle);
	}

	virtual ~TSpots(void) { }

	TSpots& operator = (TSpots const& ref) {
		_copy(ref);
		return *this;
	}

	TSpots& operator = (TSpots&& ref) noexcept {
		std::swap(strName, ref.strName);
		std::swap(strCategory, ref.strCategory);
		std::swap(flLatitude, ref.flLatitude);
		std::swap(flLongitude, ref.flLongitude);
		std::swap(strCity, ref.strCity);
		std::swap(strStreet, ref.strStreet);
		std::swap(strStreetNumber, ref.strStreetNumber);
		std::swap(strZipCode, ref.strZipCode);
		std::swap(strUrban_Unit, ref.strUrban_Unit);
		std::swap(strDistrict, ref.strDistrict);
		std::swap(flDistance, ref.flDistance);
		std::swap(flAngle, ref.flAngle);
		return *this;
	}

	TSpots& operator = (pairLocation val) {
		flDistance = val.first;
		flAngle = val.second;
		return *this;
	}

	operator pairLocation () { return std::make_pair(flLatitude, flLongitude); }

	bool operator < (TSpots const& ref) const { return _compare(ref) < 0; }
	bool operator <= (TSpots const& ref) const { return _compare(ref) <= 0; }
	bool operator > (TSpots const& ref) const { return _compare(ref) > 0; }
	bool operator >= (TSpots const& ref) const { return _compare(ref) >= 0; }
	bool operator == (TSpots const& ref) const { return _compare(ref) == 0; }
	bool operator != (TSpots const& ref) const { return _compare(ref) != 0; }

	TSpots& SetDistance(pairDistance val) {
		flLatitude = val.first;
		flLongitude = val.second;
		return *this;
	}


	// Selectors
	std::string const& Name(void) const { return strName; }
	std::string const& Category(void) const { return strCategory; }
	double const& Latitude(void) const { return flLatitude; }
	double const& Longitude(void) const { return flLongitude; }
	std::string const& City(void) const { return strCity; }
	std::string const& Street(void) const { return strStreet; }
	std::string const& StreetNumber(void) const { return strStreetNumber; }
	std::string const& ZipCode(void) const { return strZipCode; }
	std::string const& Urban_Unit(void) const { return strUrban_Unit; }
	std::string const& District(void) const { return strDistrict; }
	double const& Distance(void) const { return flDistance; }
	double const& Angle(void) const { return flAngle; }

	// Manipulators
	std::string const& Name(std::string const& newVal) { return strName = newVal; }
	std::string const& Category(std::string const& newVal) { return strCategory = newVal; }
	double const& Latitude(double const& newVal) { return flLatitude = newVal; }
	double const& Longitude(double const& newVal) { return flLongitude = newVal; }
	std::string const& City(std::string const& newVal) { return strCity = newVal; }
	std::string const& Street(std::string const& newVal) { return strStreet = newVal; }
	std::string const& StreetNumber(std::string const& newVal) { return strStreetNumber = newVal; }
	std::string const& ZipCode(std::string const& newVal) { return strZipCode = newVal; }
	std::string const& Urban_Unit(std::string const& newVal) { return strUrban_Unit = newVal; }
	std::string const& District(std::string const& newVal) { return strDistrict = newVal; }
	double const& Distance(double const& newVal) { return flDistance = newVal; }
	double const& Angle(double const& newVal) { return flAngle = newVal; }

	std::string const& Name(std::string&& newVal) { swap(strName, newVal); return strName; }
	std::string const& Category(std::string&& newVal) { swap(strCategory, newVal); return strCategory; }

	double      const& Latitude(std::string&& newVal) {
		return flLatitude = std::stod(newVal);
	}

	double      const& Longitude(std::string&& newVal) {
		return flLongitude = std::stod(newVal);
	}

	std::string const& City(std::string&& newVal) { return strCity = newVal; }
	std::string const& Street(std::string&& newVal) { return strStreet = newVal; }
	std::string const& StreetNumber(std::string&& newVal) { return strStreetNumber = newVal; }
	std::string const& ZipCode(std::string&& newVal) { return strZipCode = newVal; }
	std::string const& Urban_Unit(std::string&& newVal) { return strUrban_Unit = newVal; }
	std::string const& District(std::string&& newVal) { return strDistrict = newVal; }

	double      const& Distance(std::string&& newVal) {
		return flDistance = std::stod(newVal);
	}

	double      const& Angle(std::string&& newVal) {
		return flAngle = std::stod(newVal);
	}

private:
	void _copy(TSpots const& ref) {
		strName = ref.strName;
		strCategory = ref.strCategory;
		flLatitude = ref.flLatitude;
		flLongitude = ref.flLongitude;
		strCity = ref.strCity;
		strStreet = ref.strStreet;
		strStreetNumber = ref.strStreetNumber;
		strZipCode = ref.strZipCode;
		strUrban_Unit = ref.strUrban_Unit;
		strDistrict = ref.strDistrict;
		flDistance = ref.flDistance;
		flAngle = ref.flAngle;
	}

	int _compare(TSpots const& ref) const {
		static std::locale loc("de_DE");
		static auto lower = [](std::string const& strInput) {
			std::string strSrc(strInput.length(), ' ');
			std::transform(strInput.begin(), strInput.end(), strSrc.begin(), [](std::string::value_type val) { return std::tolower(val, loc); });
			return strSrc;
		};
		std::string a = lower(Name());
		std::string b = lower(ref.Name());
		return (a == b ? 0 : (a < b ? -1 : 1));
	}
};

using mySpots = std::vector<TSpots>;
