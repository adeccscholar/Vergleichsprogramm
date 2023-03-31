#pragma once

#include <string>
#include <array>
#include <vector>
#include <charconv>
#include <cstdlib>


inline std::string my_Double_to_String(double const& value) {
   std::array<char, 16> target;
   auto [ptrValue, ec] = std::to_chars(target.data(), target.data() + target.size(), value);
   return std::string(target.data(), ptrValue);
   }

inline std::string my_Double_to_String(double const& value, int iPrecision) {
	std::array<char, 20> target;
	auto [ptrValue, ec] = std::to_chars(target.data(), target.data() + target.size(), value, std::chars_format::fixed, iPrecision);
	return std::string(target.data(), ptrValue);
}

inline std::string my_Double_to_String_short(double const& value, int iPrecision) {
	std::string target{ "000000000000000000" };;
	std::to_chars(target.data(), target.data() + target.size(), value, std::chars_format::fixed, iPrecision);
	target.resize(target.find_last_not_of("0") + 1);
	return target;
}


inline std::string my_Double_to_String_G(double const& value, int iPrecision) {
	std::array<char, 20> target;
	auto [ptrValue, ec] = std::to_chars(target.data(), target.data() + target.size(), value, std::chars_format::fixed, iPrecision);
	auto itPoint = std::find(target.begin(), target.end(), '.');
	*itPoint = ',';
	auto iLen = itPoint - target.begin();
	auto d = std::div((int)iLen, 3);
	if (d.quot > 0) {
		std::string strVal(target.data(), ptrValue);
		size_t start = d.rem == 0 ? 3 : d.rem;
		for (size_t a = start == 3 ? 1 : 0; a < d.quot; ++a) {
		   strVal.insert(strVal.begin() + start, '.');
		   start += 4;
		   }
		return strVal;
	   }
	return std::string ( target.data(), ptrValue );
   }