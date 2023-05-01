#pragma once

#include "MyType_Traits.h"

#include <charconv>
#include <string>
#include <string_view>
#include <cstdlib>


template <std::integral ty>
   // requires std::integral<ty>
inline ty constexpr [[nodiscard]] string_to_int(std::string const& strValue) {
   ty value = 0;
   /*auto [ptr, ec] =*/ std::from_chars(strValue.data(), strValue.data() + strValue.size(), value);
   // Fehlerbehandlung
   return value;
}

//template <typename ty>
//   requires std::floating_point<ty>
//inline constexpr std::string my_Double_to_String(ty const& value) {
inline constexpr std::string [[nodiscard]] my_Double_to_String(std::floating_point auto const& value) {
   std::array<char, 20> target;
   auto [ptrValue, ec] = std::to_chars(target.data(), target.data() + target.size(), value);
   return std::string(target.data(), ptrValue);
}

template <typename ty>
   requires std::floating_point<ty>
inline constexpr std::string [[nodiscard]] my_Double_to_String_short(ty const& value, int iPrecision) {
   std::string target{ "0000000000000000" };
   std::to_chars(target.data(), target.data() + target.size(), value, std::chars_format::fixed, iPrecision);
   //target.resize(target.find_last_not_of("0") + 1);
   return target.erase(target.find_last_not_of("0") + 1);
}

//template <typename ty>
//requires std::floating_point<ty>
//inline std::string my_Double_to_String(ty const& value, int iPrecision) {
inline constexpr std::string [[nodiscard]] my_Double_to_String(std::floating_point auto const& value, int iPrecision) {
   std::array<char, 18> target;
   auto [ptrValue, ec] = std::to_chars(target.data(), target.data() + target.size(), value, std::chars_format::fixed, iPrecision);
   return std::string(target.data(), ptrValue);
}

inline constexpr std::string [[nodiscard]] my_Double_to_String_G(std::floating_point auto const& value, int iPrecision) {
   std::array<char, 18> target;
   auto [ptrValue, ec] = std::to_chars(target.data(), target.data() + target.size(), value, std::chars_format::fixed, iPrecision);
   auto itPoint = std::find(target.begin(), target.end(), '.');
   *itPoint = ',';
   auto iLen = itPoint - target.begin();
   auto d = std::div((int)iLen, 3);
   if (std::string strVal(target.data(), ptrValue); d.quot > 0) {
      size_t start = d.rem == 0 ? 3 : d.rem;
      for (size_t a = start == 3 ? 1 : 0; a < d.quot; ++a) {
         strVal.insert(strVal.begin() + start, '.');
         start += 4;
      }
      return strVal;
   }
   else return strVal;
}
