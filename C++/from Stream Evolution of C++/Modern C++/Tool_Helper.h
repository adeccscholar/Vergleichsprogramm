#pragma once

#include "MyConvert.h"

#include <functional>
#include <string>
#include <chrono>
#include <vector>
#include <locale>
#include <array>
#include <cstdlib>
#include <concepts>
#include <format>

struct TMyNum : public std::numpunct<char> {
   char_type   do_decimal_point() const { return ','; }
   char_type   do_thousands_sep() const { return '.'; }
   std::string do_grouping()      const { return "\3"; }
};

/**
 * @brief method to call a function and measure the used time
 * @tparam tty type for the time value (from std::chrono) for the measure
 * @tparam fty type for function which is called inside this template
 * @tparam ...Args Pack with arguments for the function call
 * @param time Reference Parameter from the type tty for the used time
 * @param function Parameter with the function which called
 * @param ...args Arguments Pack for the function call
 * @return original return value of the function which called
*/
template <typename tty, typename fty, typename... Args>
inline auto Call(tty& time, fty function, Args... args) {
   auto func_start = std::chrono::high_resolution_clock::now();
   auto ret = function(std::forward<Args>(args)...);
   auto func_ende = std::chrono::high_resolution_clock::now();
   time = std::chrono::duration_cast<tty>(func_ende - func_start);
   return ret;
}

inline constexpr std::string ReadHlp(std::string const& strRow, size_t& iPos) {
   size_t iEnd = strRow.find(";", iPos), iOldPos(iPos);
   iPos = iEnd + 1;
   return strRow.substr(iPos, iEnd - iOldPos);
}

inline constexpr std::vector<std::string> tokenize(std::string const& strInput, std::string const& strDelimiter, int iSize = -1) {
   std::vector<std::string> vecData;
   if (iSize >= 0) vecData.reserve(iSize);
   size_t iPos = 0, iEnd;
   do {
      iEnd = strInput.find(strDelimiter, iPos);
      std::string strPart = strInput.substr(iPos, iEnd - iPos);
      vecData.emplace_back(std::forward<std::string>(strPart));
      iPos = iEnd + strDelimiter.length();
   } while (iEnd != std::string::npos);
   return vecData;
}


