#pragma once

#include "Tool_Helper.h"

#include "MyData.h"
#include "MyFileIterator.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <tuple>
#include <vector>
#include <map>
#include <algorithm>
#include <execution> 
#include <iterator>
#include <filesystem>
#include <future>
#include <ranges>
#include <concepts>

using namespace std::literals;
namespace fs = std::filesystem;

template <typename ty>
   requires std::floating_point<ty>
inline void Reading(data_vector<ty>& vData, std::string const& strFile) {
   static const func_vector_vw<ty> funcs_vw = {
      /*
      [](TData<ty>& data, std::string_view const& val) { data.City(val); } ,
      [](TData<ty>& data, std::string_view const& val) { data.Street(val); } ,
      [](TData<ty>& data, std::string_view const& val) { data.StreetNumber(val); },
      [](TData<ty>& data, std::string_view const& val) { data.ZipCode(val); },
      [](TData<ty>& data, std::string_view const& val) { data.UrbanUnit(val); },
      [](TData<ty>& data, std::string_view const& val) { data.UrbanUnit_Old(val); },
      [](TData<ty>& data, std::string_view const& val) { data.District(val); },
      [](TData<ty>& data, std::string_view const& val) { data.Latitude(val); },
      [](TData<ty>& data, std::string_view const& val) { data.Longitude(val); },
      [](TData<ty>&, std::string_view const&) { throw std::runtime_error("unexpected number of elements.");  }
      */
      [](TData<ty>& data, std::string_view val) { data.City(val); } ,
      [](TData<ty>& data, std::string_view val) { data.Street(val); } ,
      [](TData<ty>& data, std::string_view val) { data.StreetNumber(val); },
      [](TData<ty>& data, std::string_view val) { data.ZipCode(val); },
      [](TData<ty>& data, std::string_view val) { data.UrbanUnit(val); },
      [](TData<ty>& data, std::string_view val) { data.UrbanUnit_Old(val); },
      [](TData<ty>& data, std::string_view val) { data.District(val); },
      [](TData<ty>& data, std::string_view val) { data.Latitude(val); },
      [](TData<ty>& data, std::string_view val) { data.Longitude(val); },
      [](TData<ty>&, std::string_view) { throw std::runtime_error("unexpected number of elements.");  }
   };

   const auto iSize = fs::file_size(strFile);
   //std::string strBuffer(iSize, '\0');
   std::string strBuffer;
   strBuffer.resize(iSize);
   std::ifstream ifs(strFile, std::ifstream::in | std::ifstream::binary);
   if (!ifs.is_open()) return;
   ifs.rdbuf()->pubsetbuf(strBuffer.data(), strBuffer.size());
   ifs.read(strBuffer.data(), iSize);
   ifs.close();
   std::string_view test(strBuffer.data(), strBuffer.size());

   my_line_count::reset();
   std::vector<my_line_count> lines;
   size_t size = std::count(std::execution::par, test.begin(), test.end(), '\n');
   lines.reserve(size);
   my_lines file_data(test);
   std::copy(file_data.begin(), file_data.end(), std::back_inserter(lines));
   vData.resize(lines.size());
   std::for_each(std::execution::par,
      lines.begin(), lines.end(), [&vData](auto const& val) {
         size_t iPos = 0u, iEnd;
         int iCount = 0;  // funcs_vw
         do {
            iEnd = val.view.find(';', iPos);
            funcs_vw[iCount++](vData[val.index].first, val.view.substr(iPos, iEnd - iPos));
            if (iEnd != std::string_view::npos) iPos = iEnd + 1;
         } while (iEnd != std::string_view::npos);
      });
   return;
   }

template <typename ty>
   requires std::floating_point<ty>
inline void WritingPart(auto begin, auto end, std::string const& strFile) {

   using itDataType = typename data_vector<ty>::const_iterator;
   using tplData = std::tuple<itDataType, itDataType, std::string>;

   static auto constexpr WritePart = [](itDataType begin, itDataType end, std::string& strBuffer) {
      std::for_each(std::execution::seq, begin, end, [&strBuffer](auto const& val) {
         std::string fill1(70 - val.first.City().length() - val.first.District().length(), ' ');
         std::string fill2(64 - val.first.Street().length() - val.first.StreetNumber().length(), ' ');
         std::format_to(std::back_inserter(strBuffer), "{} {} - {}, {}{} {}{}={}m / {}°\n",
            val.first.ZipCode(), val.first.City(), val.first.District(), fill1,
            val.first.Street(), val.first.StreetNumber(), fill2, my_Double_to_String_G(val.second.first, 3),
            my_Double_to_String_G(val.second.second, 3));
         });
      return true;
   };

   const size_t maxP = 8u;
   size_t pos = 0, count = std::distance(begin, end) / maxP;
   std::vector<std::pair<std::future<bool>, tplData>> procs(maxP);
   for (int i : std::ranges::iota_view{ 0u, maxP }) {
      std::get<0>(procs[i].second) = begin + i * count;
      std::get<1>(procs[i].second) = i < maxP - 1 ? begin + (i + 1) * count : end;
      if (i < maxP - 1) std::get<2>(procs[i].second).reserve(171 * count);
      else {
         size_t c = std::distance(std::get<0>(procs[i].second), std::get<1>(procs[i].second));
         std::get<2>(procs[i].second).reserve(171 * c);
      }
      procs[i].first = std::async(WritePart, std::get<0>(procs[i].second), std::get<1>(procs[i].second),
         std::ref(std::get<2>(procs[i].second)));
   }

   std::ofstream of(strFile);
   for (auto& part : procs) {
      bool test = part.first.get();
      of << std::get<2>(part.second);
   }

   /*
   std::string strBuffer;
   strBuffer.reserve(171 * std::distance(begin, end));
   std::for_each(std::execution::seq, begin, end, [&strBuffer](auto const& val) {
      std::string fill1(70 - val.first.City().length() - val.first.District().length(), ' ');
      std::string fill2(64 - val.first.Street().length() - val.first.StreetNumber().length(), ' ');
      std::format_to(std::back_inserter(strBuffer), "{} {} - {}, {}{} {}{}={}m / {}°\n",
         val.first.ZipCode(), val.first.City(), val.first.District(), fill1,
         val.first.Street(), val.first.StreetNumber(), fill2, my_Double_to_String_G(val.second.first, 3),
         my_Double_to_String_G(val.second.second, 3));
      });

   std::ofstream(strFile).write(strBuffer.data(), strBuffer.size());
   */
   }


template <typename ty>
requires std::floating_point<ty>
inline void Writing(data_vector<ty> const& vData, std::string const& strFile) {
   //WritingPart(vData.cbegin(), vData.cend(), strFile);

   using itDataType = data_vector<ty>::const_iterator;
   using tplData = std::tuple<itDataType, itDataType, std::string>;

   static auto constexpr WritePart = [](itDataType begin, itDataType end, std::string& strBuffer) {
      std::for_each(std::execution::seq, begin, end, [&strBuffer](auto const& val) {
         std::string fill1(70 - val.first.City().length() - val.first.District().length(), ' ');
         std::string fill2(64 - val.first.Street().length() - val.first.StreetNumber().length(), ' ');
         std::format_to(std::back_inserter(strBuffer), "{} {} - {}, {}{} {}{}={}m / {}°\n",
            val.first.ZipCode(), val.first.City(), val.first.District(), fill1,
            val.first.Street(), val.first.StreetNumber(), fill2, my_Double_to_String_G(val.second.first, 3),
            my_Double_to_String_G(val.second.second, 3));
         });
      return true;
   };

   const size_t maxP = 16u;
   size_t pos = 0, count = vData.size() / maxP;
   std::vector<std::pair<std::future<bool>, tplData>> procs(maxP);
   for (int i : std::ranges::iota_view{ 0u, maxP }) {
      std::get<0>(procs[i].second) = vData.cbegin() + i * count;
      std::get<1>(procs[i].second) = i < maxP - 1 ? vData.cbegin() + (i + 1) * count : vData.cend();
      if (i < maxP - 1) std::get<2>(procs[i].second).reserve(171 * count);
      else {
         size_t c = std::distance(std::get<0>(procs[i].second), std::get<1>(procs[i].second));
         std::get<2>(procs[i].second).reserve(171 * c);
      }
      procs[i].first = std::async(WritePart, std::get<0>(procs[i].second), std::get<1>(procs[i].second),
         std::ref(std::get<2>(procs[i].second)));
   }

   std::ofstream of(strFile);
   for (auto& part : procs) {
      bool test = part.first.get();
      of << std::get<2>(part.second);
   }
}

template <typename ty>
   requires std::floating_point<ty>
inline void Sorting(data_vector<ty>& vData) {

   static const std::string special_chars = "ßüöäÄÖÜáèéçñ"s;
   static const std::map<char, std::string> special_values = { {'Ö',"Oe"s },{'ä',"ae"s}, {'Ä',"Ae"s },{'ñ',"n"s },{'ü',"ue"s},
                                                               {'ç',"c"s },{'Ü',"Ue"s},{'è',"e"s },{'ß',"ss"s},
                                                               {'ö',"oe"s}, {'á',"a"s },{'é',"e"s } };

   static constexpr auto normalize = [](std::string const& param, size_t pos) noexcept {
      std::string text = param;
      do {
         text.replace(pos, 1, special_values.find(text[pos])->second);
      } while ((pos = text.find_first_of(special_chars, pos)) != std::string::npos);
      return text;
   };

   static constexpr auto compare = [](std::string const& left, std::string const& right) noexcept {
      if (auto ret = left <=> right; ret == 0) [[likely]] return ret;
      else {
         auto cleft_pos = left.find_first_of(special_chars);
         auto cright_pos = right.find_first_of(special_chars);
         auto cleft = cleft_pos != std::string::npos;
         auto cright = cright_pos != std::string::npos;

         if (!cleft && !cright) [[likely]] return ret;
         else if (!cleft && cright)  return left <=> normalize(right, cright_pos);
         else if (cleft && !cright)  return normalize(left, cleft_pos) <=> right;
         else return normalize(left, cleft_pos) <=> normalize(right, cright_pos);
      }
   };

   static constexpr auto lower = [](std::string&& strVal) {
      std::transform(std::execution::par, strVal.begin(), strVal.end(), strVal.begin(), [](char val) { return std::tolower(val); });
      return strVal;
   };


   static constexpr auto compare_streetnumber2 = [](std::string const& aSNr, std::string const& bSNr) noexcept {
      int a_Nr, b_Nr;
      auto [a_ptr, a_ec] { std::from_chars(aSNr.data(), aSNr.data() + aSNr.size(), a_Nr) };
      auto [b_ptr, b_ec] { std::from_chars(bSNr.data(), bSNr.data() + bSNr.size(), b_Nr) };
      if (auto cmp = a_Nr <=> b_Nr; cmp != 0) [[likely]] return cmp < 0;
      else {
         return std::tolower(*a_ptr) < std::tolower(*b_ptr); // only one char as extension
      }
   };

   std::sort(std::execution::par, vData.begin(), vData.end(), [](auto const& lhs, auto const& rhs) {
      if (auto cmp = compare(lhs.first.City(), rhs.first.City()); cmp != 0) return cmp < 0;
      else if (auto cmp = compare(lhs.first.UrbanUnit(), rhs.first.UrbanUnit()); cmp != 0) return cmp < 0;
      else if (auto cmp = compare(lhs.first.District(), rhs.first.District()); cmp != 0) return cmp < 0;
      else if (auto cmp = lhs.first.ZipCode() <=> rhs.first.ZipCode(); cmp != 0) return cmp < 0;
      else if (auto cmp = compare(lhs.first.Street(), rhs.first.Street()); cmp != 0) [[likely]] return cmp < 0;
      return compare_streetnumber2(lhs.first.StreetNumber(), rhs.first.StreetNumber());
      });
}


inline void DeleteDirectories_with_ranges(std::string const& strPath) {
   static constexpr auto is_directory = [](auto const& d) { return fs::is_directory(d); };
   for (auto const& dir : fs::directory_iterator(strPath) | std::views::filter(is_directory)) fs::remove_all(dir.path());
   }

inline void DeleteDirectories(std::string const& strPath) {
   static std::function<void(fs::path const&, bool)> ClearDir = [](fs::path const& p, bool boDelete) {
      std::vector<fs::path> files;
      files.reserve(std::distance(fs::directory_iterator(p), fs::directory_iterator()));
      std::copy(fs::directory_iterator(p), fs::directory_iterator(), std::back_inserter(files));
      auto it_dir = std::partition(std::execution::par, files.begin(), files.end(), [](fs::path const& p) { return !fs::is_directory(p); });
      if (boDelete) std::for_each(std::execution::par, files.begin(), it_dir, [](fs::path const& p) { fs::remove(p); });
      std::for_each(std::execution::par, it_dir, files.end(), [](fs::path const& p2) { ClearDir(p2, true); } );
      if (boDelete) fs::remove(p);
      };
   ClearDir(strPath, false);
   }

//std::for_each(fs::directory_iterator(strPath), fs::directory_iterator(), [](auto& d) { if (fs::is_directory(d)) fs::remove_all(d.path()); });

template <typename ty>
   requires std::floating_point<ty>
inline void ReadFromDirectory(std::string const& strPath, data_vector<ty>& vData) {
   /*
      static const std::vector<std::function<void(std::pair<TData<ty>, Result<ty>>&, std::string_view const&)>> funcs_read_d = {
         [](std::pair<TData<ty>, Result<ty>>& data, std::string_view const& val) { data.first.StreetNumber(val); },
         [](std::pair<TData<ty>, Result<ty>>& data, std::string_view const& val) { data.first.ZipCode(val); },
         [](std::pair<TData<ty>, Result<ty>>& data, std::string_view const& val) { data.first.UrbanUnit_Old(val); },
         [](std::pair<TData<ty>, Result<ty>>& data, std::string_view const& val) { data.first.Latitude(val); },
         [](std::pair<TData<ty>, Result<ty>>& data, std::string_view const& val) { data.first.Longitude(val); },
         [](std::pair<TData<ty>, Result<ty>>& data, std::string_view const& val) {
                                                  std::from_chars(val.data(), val.data() + val.size(), data.second.first); },
         [](std::pair<TData<ty>, Result<ty>>& data, std::string_view const& val) {
                                                  std::from_chars(val.data(), val.data() + val.size(), data.second.second); },
         [](std::pair<TData<ty>, Result<ty>>& data, std::string_view const& val) { throw std::runtime_error("file corruped"); }
      };
   */

   static const std::vector<std::function<void(std::pair<TData<ty>, Result<ty>>&, std::string_view const&)>> funcs_read_d = {
      [](std::pair<TData<ty>, Result<ty>>& data, std::string_view val) { data.first.StreetNumber(val); },
      [](std::pair<TData<ty>, Result<ty>>& data, std::string_view val) { data.first.ZipCode(val); },
      [](std::pair<TData<ty>, Result<ty>>& data, std::string_view val) { data.first.UrbanUnit_Old(val); },
      [](std::pair<TData<ty>, Result<ty>>& data, std::string_view val) { data.first.Latitude(val); },
      [](std::pair<TData<ty>, Result<ty>>& data, std::string_view val) { data.first.Longitude(val); },
      [](std::pair<TData<ty>, Result<ty>>& data, std::string_view val) {
                                               std::from_chars(val.data(), val.data() + val.size(), data.second.first); },
      [](std::pair<TData<ty>, Result<ty>>& data, std::string_view val) {
                                               std::from_chars(val.data(), val.data() + val.size(), data.second.second); },
      [](std::pair<TData<ty>, Result<ty>>& data, std::string_view val) { throw std::runtime_error("file corruped"); }
   };

   using detail_data = std::tuple<std::string, std::string, std::string, std::string>;
   using data_to_read = std::tuple<fs::path, std::string, std::vector<my_line_count>, detail_data>;
   using to_read_vec = std::vector<data_to_read>;
   to_read_vec to_read;

   vData.clear();

   try {
      std::vector<fs::path> cities;
      //cities.reserve(std::distance(fs::directory_iterator(strPath), fs::directory_iterator { }));
      std::copy(fs::directory_iterator(strPath), fs::directory_iterator{ }, std::back_inserter(cities));
      auto it_cities = std::partition(std::execution::par, cities.begin(), cities.end(), [](auto& p) { return fs::is_directory(p); });
      std::for_each(cities.begin(), it_cities, [&to_read](auto& d) {
         std::string&& strCities = d.stem().string();
         std::vector<fs::path> units;
         //units.reserve(std::distance(fs::directory_iterator(d), fs::directory_iterator{ }));
         std::copy(fs::directory_iterator(d), fs::directory_iterator(), std::back_inserter(units));
         auto it_units = std::partition(std::execution::par, units.begin(), units.end(), [](auto& p) { return fs::is_directory(p); });
         std::for_each(units.begin(), it_units, [&to_read, &strCities](auto& d) {
            std::string&& strUnit = d.stem().string();
            std::vector<fs::path> districts;
            //districts.reserve(std::distance(fs::directory_iterator(d), fs::directory_iterator{ }));
            std::copy(fs::directory_iterator(d), fs::directory_iterator(), std::back_inserter(districts));
            auto it_districts = std::partition(std::execution::par, districts.begin(), districts.end(), [](auto& p) { return fs::is_directory(p); });
            std::for_each(districts.begin(), it_districts, [&to_read, &strCities, &strUnit](auto& d) {
               std::string&& strDistrict = d.stem().string();
               std::vector<fs::path> streets;
               streets.reserve(std::distance(fs::directory_iterator(d), fs::directory_iterator{ }));
               std::copy(fs::directory_iterator(d), fs::directory_iterator(), std::back_inserter(streets));
               auto it_streets = std::partition(std::execution::par, streets.begin(), streets.end(), [](auto& p) { return fs::is_regular_file(p); });
               std::for_each(streets.begin(), it_streets, [&to_read, &strCities, &strUnit, &strDistrict](auto& d) {
                  std::string&& strStreet = d.stem().string();
                  detail_data data = { strCities, strUnit, strDistrict, strStreet };
                  to_read.emplace_back(d, std::string(), std::vector<my_line_count>(), std::forward<detail_data>(data));
                  });
               });
            });

         });

   }
   catch (std::exception& ex) {
      std::cerr << "error while preparing reading: " << ex.what() << std::endl;
   }

   my_line_count::reset();

   std::for_each(std::execution::par, to_read.begin(), to_read.end(), [](auto& val) mutable {
      std::ifstream ifs(std::get<0>(val).string());
      if (ifs.is_open()) {
         auto iSize = fs::file_size(std::get<0>(val));
         std::get<1>(val).resize(iSize);
         ifs.read(std::get<1>(val).data(), iSize);
         ifs.close();

         std::string_view test(std::get<1>(val).data(), std::get<1>(val).size());
         std::get<2>(val).reserve(std::count(test.begin(), test.end(), '\n'));
         my_lines file_data(test);
         std::copy(file_data.begin(), file_data.end(), std::back_inserter(std::get<2>(val)));
      }
      });

   vData.resize(my_line_count::GetCounter());

   try {
      std::for_each(std::execution::par, to_read.begin(), to_read.end(), [&vData](auto& val) {
         auto& [strCity, strUnit, strDistrict, strStreet] = std::get<3>(val);
         std::for_each(std::execution::par, std::get<2>(val).begin(), std::get<2>(val).end(), [&vData, &strCity, &strUnit, &strDistrict, &strStreet](auto& val_) mutable {
            auto& data = vData[val_.index];
            data.first.City(strCity);
            data.first.UrbanUnit(strUnit);
            data.first.District(strDistrict);
            data.first.Street(strStreet);

            size_t iPos = 0, iEnd, iCount = 0;
            do {
               iEnd = val_.view.find(";", iPos);
               funcs_read_d[iCount++](data, val_.view.substr(iPos, iEnd - iPos));
               if (iEnd != std::string_view::npos) iPos = iEnd + 1;
            } while (iEnd != std::string_view::npos);
            });
         });
   }
   catch (std::exception& ex) {
      std::cerr << "error while parsing data: " << ex.what() << std::endl;
   }
}


template <typename ty>
   requires std::floating_point<ty>
inline void WriteToDirectory(std::string const& strPath, data_vector<ty>& vData) {
   static constexpr auto compare_streetnumber2 = [](std::string const& aSNr, std::string const& bSNr) noexcept {
      int iNbr1, iNbr2;
      auto [pt1, ec1] = std::from_chars(aSNr.data(), aSNr.data() + aSNr.size(), iNbr1);
      auto [pt2, ec2] = std::from_chars(bSNr.data(), bSNr.data() + bSNr.size(), iNbr2);
      if (auto cmp = iNbr1 <=> iNbr2; cmp != 0) return cmp < 0;
      else {
         return std::string(pt1) < std::string(pt2); //return lower(a_ptr) < lower(b_ptr);
      }
   };

   static auto constexpr build = [](auto const& ref) {
      return std::tie(ref.first.City(), ref.first.UrbanUnit(), ref.first.District(), ref.first.Street());
   };

   static constexpr auto compare = [](auto const& a, auto const& b) noexcept {
      if (auto cmp = std::tie(a.first.City(), a.first.UrbanUnit(), a.first.District(), a.first.Street()) <=>
         std::tie(b.first.City(), b.first.UrbanUnit(), b.first.District(), b.first.Street()); cmp != 0) return cmp < 0;
      return compare_streetnumber2(a.first.StreetNumber(), b.first.StreetNumber());
   };

   std::sort(std::execution::par, vData.begin(), vData.end(), compare);

   using tplParts = std::tuple<typename data_vector<ty>::const_iterator, typename data_vector<ty>::const_iterator, std::ostringstream>;
   using myIterPair = std::tuple<typename data_vector<ty>::const_iterator, typename data_vector<ty>::const_iterator, fs::path>;
   std::vector<myIterPair> positions;
   fs::path myOldPath = ""s;
   for (auto first = vData.begin(); first != vData.end();) {
      auto const& [address, result] = *first;
      fs::path myPath = fs::path(strPath) / address.City() / address.UrbanUnit() / address.District();
      if (myPath != myOldPath) {
         fs::create_directories(myPath);
         myOldPath = myPath;
      }
      auto last = std::find_if(std::execution::par, first, vData.end(), [&address](auto const& val) {
         return address.Street() != val.first.Street() ? true :
            (address.District() != val.first.District() ? true :
               (address.UrbanUnit() != val.first.UrbanUnit() ? true :
                  (address.City() != val.first.City() ? true : false)));
         });
      positions.emplace_back(first, last, myPath / (address.Street() + ".csv"s));
      first = last;
   }

   std::for_each(std::execution::par, positions.cbegin(), positions.cend(), [](auto const& val) {
      std::string strBuffer;
      strBuffer.reserve(std::distance(std::get<0>(val), std::get<1>(val)) * 68);
      std::for_each(std::get<0>(val), std::get<1>(val), [&strBuffer](auto const& value) {
         auto const& [address, result] = value;
         std::format_to(std::back_inserter(strBuffer), "{};{};{};{};{};{};{}\n",
            address.StreetNumber(), address.ZipCode(), address.UrbanUnit_Old(),
            my_Double_to_String_short(address.Latitude(), 9), my_Double_to_String_short(address.Longitude(), 9),
            my_Double_to_String_short(result.first, 3), my_Double_to_String_short(result.second, 3));
         });
      std::ofstream(std::get<2>(val)).write(strBuffer.data(), strBuffer.size());
      });
}


