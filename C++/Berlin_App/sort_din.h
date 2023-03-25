#pragma once
#include <string>
#include <algorithm> 
#include <map>
#include <locale>

inline std::string string_normalize (std::string const& param) {
	static const std::map<std::string, std::string> special_values = { { "ä", "ae"}, { "Ä", "Ae" }, { "ü", "ue" }, { "Ü", "Ue" }, { "ß", "ss" },
																								{ "ö", "oe"}, { "Ö", "Oe" }, {"á", "a"},  {"è", "e" }, { "é", "e" }, {"ç", "c"}, {"ñ", "n"} };
	std::string text = param;
	std::for_each(special_values.begin(), special_values.end(), [&text](auto const& vals) {
		size_t start_pos = 0;
		while ((start_pos = text.find(vals.first, start_pos)) != std::string::npos) {
			text.replace(start_pos, vals.first.length(), vals.second);
			start_pos += vals.second.length();
		}
		});
	return text;
};

inline std::string string_normalize_lower(std::string const& param) {
	static std::locale loc("de_DE");
	static auto lower = [](std::string const& strVal) {
      std::string retval = strVal;  
		std::transform(retval.begin(), retval.end(), retval.begin(), [](std::string::value_type val) { return std::tolower(val, loc); });
		return retval;
	};

	static const std::map<std::string, std::string> special_values = { { "ä", "ae"}, { "ü", "ue" }, { "ß", "ss" }, { "ö", "oe"},
                                                                       {"á", "a"},  {"è", "e" }, { "é", "e" }, {"ç", "c"}, {"ñ", "n"} };
	std::string text = lower(param);
	std::for_each(special_values.begin(), special_values.end(), [&text](auto const& vals) {
		size_t start_pos = 0;
		while ((start_pos = text.find(vals.first, start_pos)) != std::string::npos) {
			text.replace(start_pos, vals.first.length(), vals.second);
			start_pos += vals.second.length();
		}
		});
	return text;
};


inline int compare_DIN(std::string const& left, std::string const& right) {
	static const std::string special_chars = "äÄöÖüÜßáèéçñ";
	if (left.find_first_of(special_chars) != std::string::npos || right.find_first_of(special_chars) != std::string::npos) {
		auto nleft  = string_normalize(left);
		auto nright = string_normalize(right);
		return nleft != nright ? nleft < nright ? -1 : 1 : 0;
	}
	return left != right ? left < right ? -1 : 1 : 0;
};


inline int compare_StreetNumber(std::string const& aSNr, std::string const& bSNr) {
	static std::locale loc("de_DE");
	static auto lower = [](std::string&& strVal) {
		std::transform(strVal.begin(), strVal.end(), strVal.begin(), [](std::string::value_type val) { return std::tolower(val, loc); });
		return strVal;
	   };

	std::string::size_type szA, szB;
	int iAN = stoi(aSNr, &szA);
	int iBN = stoi(bSNr, &szB);
	return iAN == iBN ? lower(aSNr.substr(szA)) < lower(bSNr.substr(szB)) : iAN < iBN;
};
