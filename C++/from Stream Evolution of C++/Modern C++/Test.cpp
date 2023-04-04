/**
 \file
 \brief Hauptdatei für das Leseprojekt
 \details Diese Datei enthält die Funktion "main" des Testprogramms. Hier beginnt und endet die Ausführung des Programms.
 \details Zusätzlich sind noch alle Routinen, Datentypen und Hilfskonstrukte hier
 \author  Volker Hillmann
 \date    09.12.2022
 \copyright Copyright &copy; 2022-2023. adecc Systemhaus GmbH
 \version 1.0
*/

#include "MyData.h"
#include "Tool_Helper.h"
#include "MyFileIterator.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <string>
#include <string_view> // !! C++17
#include <vector>
#include <map>
#include <array>
#include <fstream>
#include <functional>
#include <execution> // !! C++17
#include <iterator>
#include <optional> // !! C++17
#include <atomic>
#include <filesystem>
#include <thread>
#include <future>
#include <format>
#include <ranges>


using namespace std::literals;
namespace fs = std::filesystem;

inline void Reading(data_vector<double>& vData, std::string const& strFile) {
   static const func_vector_vw funcs_vw = {
   [](TData<double>& data, std::string_view const& val) { data.City(val); } ,
   [](TData<double>& data, std::string_view const& val) { data.Street(val); } ,
   [](TData<double>& data, std::string_view const& val) { data.StreetNumber(val); },
   [](TData<double>& data, std::string_view const& val) { data.ZipCode(val); },
   [](TData<double>& data, std::string_view const& val) { data.UrbanUnit(val); },
   [](TData<double>& data, std::string_view const& val) { data.UrbanUnit_Old(val); },
   [](TData<double>& data, std::string_view const& val) { data.District(val); },
   [](TData<double>& data, std::string_view const& val) { data.Latitude(val); },
   [](TData<double>& data, std::string_view const& val) { data.Longitude(val); },
   [](TData<double>&, std::string_view const&) { throw std::runtime_error("unexpected number of elements.");  }
   };

   const auto iSize = fs::file_size(strFile);
   std::string strBuffer(iSize, '\0');
   //std::string strBuffer;
   //strBuffer.resize(iSize);
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
inline void WritingPart(ty begin, ty end, std::string const& strFile) {
   std::string strBuffer;
   strBuffer.reserve(176 * std::distance(begin, end));
   std::for_each(begin, end, [&strBuffer](auto const& val) {
      std::string fill1(70 - val.first.City().length() - val.first.District().length(), ' ');
      std::string fill2(64 - val.first.Street().length() - val.first.StreetNumber().length(), ' ');
      std::format_to(std::back_inserter(strBuffer), "{} {} - {}, {}{} {}{}={}m / {}°\n",
         val.first.ZipCode(), val.first.City(), val.first.District(), fill1,
         val.first.Street(), val.first.StreetNumber(), fill2, my_Double_to_String_G(val.second.first, 3),
         my_Double_to_String_G(val.second.second, 3));
      });

   std::ofstream(strFile).write(strBuffer.data(), strBuffer.size());
}

inline void Writing(data_vector<double> const& vData, std::string const& strFile) {
   WritingPart(vData.cbegin(), vData.cend(), strFile);
}



inline void Sorting(data_vector<double>& vData) {

   static const std::string special_chars = "äöüßÄÖÜáèéçñ"s;
   static const std::map<unsigned char, std::string> special_values = { {'ä',"ae"s}, {'Ä',"Ae"s },{'ü',"ue"s},{'Ü',"Ue"s},{'ß',"ss"s},
                                                               {'ö',"oe"s}, {'Ö',"Oe"s },{'á',"a"s },{'è',"e"s },{'é',"e"s },{'ç',"c"s },{'ñ',"n"s } };

   static constexpr auto normalize = [](std::string const& param, size_t pos) noexcept {
      std::string text = param;
      do {
         text.replace(pos, 1, special_values.find(text[pos])->second);
         } 
      while ((pos = text.find_first_of(special_chars, pos)) != std::string::npos);
      return text;
   };

   static constexpr auto compare = [](std::string const& left, std::string const& right) noexcept {
      if(auto ret = left <=> right; ret == 0) return ret;
      else {
         auto cleft_pos = left.find_first_of(special_chars);
         auto cright_pos = right.find_first_of(special_chars);
         auto cleft = cleft_pos != std::string::npos;
         auto cright = cright_pos != std::string::npos;

         if (!cleft && cright)       return left <=> normalize(right, cright_pos);
         else if (cleft && !cright)  return normalize(left, cleft_pos) <=> right;
         else if (cleft && cright)   return normalize(left, cleft_pos) <=> normalize(right, cright_pos);
         else return ret;
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
      if (auto cmp = a_Nr <=> b_Nr; cmp != 0) return cmp < 0;
      else {
         return lower(a_ptr) < lower(b_ptr);
         }
      };

   std::sort(std::execution::par, vData.begin(), vData.end(), [](auto const& lhs, auto const& rhs) {
      if (auto cmp = compare(lhs.first.City(), rhs.first.City()); cmp != 0) return cmp < 0;
      if (auto cmp = compare(lhs.first.UrbanUnit(), rhs.first.UrbanUnit()); cmp != 0) return cmp < 0;
      if (auto cmp = compare(lhs.first.District(), rhs.first.District()); cmp != 0) return cmp < 0;
      if (auto cmp = lhs.first.ZipCode() <=> rhs.first.ZipCode(); cmp != 0) return cmp < 0;
      if (auto cmp = compare(lhs.first.Street(), rhs.first.Street()); cmp != 0) return cmp < 0;
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
      std::copy(fs::directory_iterator(p), fs::directory_iterator(), std::back_inserter(files));
      auto it_dir = std::partition(std::execution::par, files.begin(), files.end(), [](auto const& p) { return !fs::is_directory(p); });
      if (boDelete) std::for_each(std::execution::par, files.begin(), it_dir, [](auto const& p) { fs::remove(p); });
      std::for_each(std::execution::par, it_dir, files.end(), [](auto const& p2) { ClearDir(p2, true);  });
      };

   ClearDir(strPath, false);
   //std::for_each(fs::directory_iterator(strPath), fs::directory_iterator(), [](auto& d) { if (fs::is_directory(d)) fs::remove_all(d.path()); });
}

std::vector<std::function<void(std::pair<TData<double>, Result<double>>&, std::string_view const&)>> funcs_read_d = {
      [](std::pair<TData<double>, Result<double>>& data, std::string_view const& val) { data.first.StreetNumber(val); },
      [](std::pair<TData<double>, Result<double>>& data, std::string_view const& val) { data.first.ZipCode(val); },
      [](std::pair<TData<double>, Result<double>>& data, std::string_view const& val) { data.first.UrbanUnit_Old(val); },
      [](std::pair<TData<double>, Result<double>>& data, std::string_view const& val) { data.first.Latitude(val); },
      [](std::pair<TData<double>, Result<double>>& data, std::string_view const& val) { data.first.Longitude(val); },
      [](std::pair<TData<double>, Result<double>>& data, std::string_view const& val) { 
                                               std::from_chars(val.data(), val.data() + val.size(), data.second.first); },
      [](std::pair<TData<double>, Result<double>>& data, std::string_view const& val) { 
                                               std::from_chars(val.data(), val.data() + val.size(), data.second.second); },
      [](std::pair<TData<double>, Result<double>>& data, std::string_view const& val) { throw std::runtime_error("file corruped"); }
};

inline void ReadFromDirectory(std::string const& strPath, data_vector<double>& vData) {
   static auto constexpr is_dir    = [](auto const& p) { return fs::is_directory(p);  };
   static auto constexpr is_file   = [](auto const& p) { return fs::is_regular_file(p);  };
   static auto constexpr cnt_items = [](auto const& p) { return std::distance(fs::directory_iterator{ p }, { }); };

   using detail_data = std::tuple<std::string, std::string, std::string, std::string>;
   using data_to_read = std::tuple<fs::path, std::string, std::vector<my_line_count>, detail_data>;
   using to_read_vec = std::vector<data_to_read>;
   to_read_vec to_read;

   vData.clear();

   try {
      std::vector<fs::path> cities;
      cities.reserve(cnt_items(strPath));
      std::copy(fs::directory_iterator(strPath), fs::directory_iterator(), std::back_inserter(cities));
      auto it_cities = std::partition(std::execution::par, cities.begin(), cities.end(), is_dir); // [](auto const& p) { return fs::is_directory(p); });
      std::for_each(cities.begin(), it_cities, [&to_read](auto const& d) {
         std::string strCities = d.stem().string();
         std::vector<fs::path> units;
         units.reserve(cnt_items(d));
         std::copy(fs::directory_iterator(d), fs::directory_iterator(), std::back_inserter(units));
         auto it_units = std::partition(std::execution::par, units.begin(), units.end(), is_dir); // [](auto const& p) { return fs::is_directory(p); });
         std::for_each(units.begin(), it_units, [&to_read, &strCities](auto const& d) {
            std::string strUnit = d.stem().string();
            std::vector<fs::path> districts;
            districts.reserve(cnt_items(d));
            std::copy(fs::directory_iterator(d), fs::directory_iterator(), std::back_inserter(districts));
            auto it_districts = std::partition(std::execution::par, districts.begin(), districts.end(), is_dir); // [](auto const& p) { return fs::is_directory(p); });
            std::for_each(districts.begin(), it_districts, [&to_read, &strCities, &strUnit](auto const& d) {
               std::string strDistrict = d.stem().string();
               std::vector<fs::path> streets;
               streets.reserve(cnt_items(d));
               std::copy(fs::directory_iterator(d), fs::directory_iterator(), std::back_inserter(streets));
               auto it_streets = std::partition(std::execution::par, streets.begin(), streets.end(), is_file); // [](auto const& p) { return fs::is_regular_file(p); });
               std::for_each(streets.begin(), it_streets, [&to_read, &strCities, &strUnit, &strDistrict](auto const& d) {
                  std::string strStreet = d.stem().string();
                  detail_data data = std::make_tuple( strCities, strUnit, strDistrict, strStreet );
                  to_read.emplace_back(d, std::string(), std::vector<my_line_count>(), std::forward<detail_data>(data));
                  });
               });
            });

         });

   }
   catch(std::exception& ex) {
      std::cerr << "error while preparing reading: " << ex.what() << std::endl;
      }

   my_line_count::reset();

   std::for_each(std::execution::par, to_read.begin(), to_read.end(), [](auto& val) mutable {
      std::ifstream ifs(std::get<0>(val).string());
      if(ifs.is_open()) {
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
         std::for_each(std::execution::par, std::get<2>(val).begin(), std::get<2>(val).end(), [&vData, &val](auto& val_) mutable {
            vData[val_.index].first.City(std::get<0>(std::get<3>(val)));
            vData[val_.index].first.UrbanUnit(std::get<1>(std::get<3>(val)));
            vData[val_.index].first.District(std::get<2>(std::get<3>(val)));
            vData[val_.index].first.Street(std::get<3>(std::get<3>(val)));

            size_t iPos = 0, iEnd, iCount = 0;
            do {
               iEnd = val_.view.find(";", iPos);
               funcs_read_d[iCount++](vData[val_.index], val_.view.substr(iPos, iEnd - iPos));
               if (iEnd != std::string_view::npos) iPos = iEnd + 1;
               } 
            while (iEnd != std::string_view::npos);
            });
         });
      }
   catch(std::exception& ex) {
      std::cerr << "error while parsing data: " << ex.what() << std::endl;
      }

   }





inline void WriteToDirectory(std::string const& strPath, data_vector<double>& vData) {
//   auto constexpr WriteToDirectory = [](std::string const& strPath, data_vector<double>& vData) {
      /*
      static constexpr auto lower = [](std::string&& strVal) {
         std::transform(std::execution::par, strVal.begin(), strVal.end(), strVal.begin(), [](char val) { return std::tolower(val); });
         return strVal;
      };
      */
      //static constexpr auto to_number = [](auto const& val) { int ret;  std::from_chars(val.data(), val.data() + val.size(), ret); return ret; };
      static constexpr auto compare_streetnumber2 = [](std::string const& aSNr, std::string const& bSNr) noexcept {
         int iNbr1, iNbr2;
         auto [pt1, ec1] = std::from_chars(aSNr.data(), aSNr.data() + aSNr.size(), iNbr1);
         auto [pt2, ec2] = std::from_chars(bSNr.data(), bSNr.data() + bSNr.size(), iNbr2);
         if (auto cmp = iNbr1 <=> iNbr2;cmp != 0) return cmp < 0;
         else {
            return std::string(pt1) < std::string(pt2); //return lower(a_ptr) < lower(b_ptr);
         }
      };

      static auto constexpr build = [](auto const& ref) { 
         return std::tie(ref.first.City(), ref.first.UrbanUnit(), ref.first.District(), ref.first.Street()); 
         };

      static constexpr auto compare = [](auto const& a, auto const& b) noexcept {  
         //if (auto cmp = build(a) <=> build(b); cmp != 0) return cmp < 0;
         //if (auto cmp = std::tie(a.first.City(), a.first.UrbanUnit(), a.first.District(), a.first.Street()) <=>
         //               std::tie(b.first.City(), b.first.UrbanUnit(), b.first.District(), b.first.Street()); cmp != 0) return cmp < 0;
         //*
         if (auto cmp = a.first.City() <=> b.first.City(); cmp != 0) return cmp < 0;
         if (auto cmp = a.first.UrbanUnit() <=> b.first.UrbanUnit(); cmp != 0) return cmp < 0;
         if (auto cmp = a.first.District() <=> b.first.District(); cmp != 0) return cmp < 0;
         if (auto cmp = a.first.Street() <=> b.first.Street(); cmp != 0) return cmp < 0;
         //*/
         return compare_streetnumber2(a.first.StreetNumber(), b.first.StreetNumber());
      };

      /*
      static constexpr auto next = [](auto const& first, auto const& val) {
         return first->first.Street() != val.first.Street() ? true :
            (first->first.District() != val.first.District() ? true :
               (first->first.UrbanUnit() != val.first.UrbanUnit() ? true :
                  (first->first.City() != val.first.City() ? true : false)));
      };
      */

      std::sort(std::execution::par, vData.begin(), vData.end(), compare);

      using tplParts = std::tuple<data_vector<double>::const_iterator, data_vector<double>::const_iterator, std::ostringstream>;
      using myIterPair = std::tuple<data_vector<double>::const_iterator, data_vector<double>::const_iterator, fs::path>;
      std::vector<myIterPair> positions;
      fs::path myOldPath = ""s;
      for (auto first = vData.begin(); first != vData.end();) {
         auto const& [address, result] = *first;
         fs::path myPath = fs::path(strPath) / address.City() / address.UrbanUnit() / address.District();
         if (myPath != myOldPath) {
            fs::create_directories(myPath);
            myOldPath = myPath;
         }
         auto last = std::find_if(std::execution::par, first, vData.end(), [&first](auto const& val) {
            return first->first.Street() != val.first.Street() ? true :
               (first->first.District() != val.first.District() ? true :
                  (first->first.UrbanUnit() != val.first.UrbanUnit() ? true :
                     (first->first.City() != val.first.City() ? true : false)));
            });
         //positions.emplace_back( std::make_tuple(first, last, myPath / (first->first.Street() + ".csv"s)));
         positions.emplace_back(first, last, myPath / (first->first.Street() + ".csv"s));
         first = last;
      }

      //auto size = std::distance(positions.cbegin(), positions.cend()) * 70;
      std::for_each(std::execution::par, positions.cbegin(), positions.cend(), [](auto const& val) {
         std::string strBuffer;
         //strBuffer.reserve(std::distance(std::get<0>(val), std::get<1>(val)));
         std::for_each(std::get<0>(val), std::get<1>(val), [&strBuffer](auto const& value) {
            std::format_to(std::back_inserter(strBuffer), "{};{};{};{};{};{};{}\n",
            value.first.StreetNumber(), value.first.ZipCode(), value.first.UrbanUnit_Old(),
            my_Double_to_String_short(value.first.Latitude(), 9), my_Double_to_String_short(value.first.Longitude(), 9),
            my_Double_to_String_short(value.second.first, 3), my_Double_to_String_short(value.second.second, 3));
            });
         std::ofstream(std::get<2>(val)).write(strBuffer.data(), strBuffer.size());
         });

   }


std::string get_current_time_and_date() {
   auto const now = std::chrono::system_clock::now();
   auto const timew = std::chrono::current_zone()->to_local(now);
   const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
   return std::format("{:%d.%m.%Y %X},{:03d}", timew, millis.count());
  }

void Rechentest(std::string const& strDirectory) {
   using myTimeType = std::chrono::microseconds;  // std::chrono::milliseconds
   static const double time_factor = 1000000.;    // 1000.
   static const int    time_prec   = 6;           // 3

   data_vector<double> vData;
   data_vector<double>::iterator it;
   Location<double> point    = { 52.520803, 13.40945 };
   std::string strFilename   = strDirectory + "\\berlin_infos.dat"s;
   std::string strOutput_all = strDirectory + "\\Testausgabe_alle.txt"s;
   std::string strOutput     = strDirectory + "\\Testausgabe.txt"s;

   using call_func = std::function<void()>;
   using mySizeRet = std::optional<size_t>;
   using size_func = std::function<mySizeRet()>;

   static size_func addresses_size = [&vData]() -> mySizeRet {
      return std::make_optional<size_t>(vData.size());
      };

   static size_func addresses_capacity = [&vData]() -> mySizeRet {
      return std::make_optional<size_t>(vData.capacity());
      };

   static size_func value_size = [&it, &vData]() -> mySizeRet {
      return std::make_optional<size_t>(std::distance(vData.begin(), it));
      };

   std::vector<std::tuple<std::string, std::string, call_func, size_func>> test_funcs = {
      { "read file "s + strFilename, "datasets read from file"s,
            std::bind(Reading, std::ref(vData), std::cref(strFilename)),
            addresses_size },
      { "calculate data"s, "datasets calculated for point"s,
            [&vData, &point]() { Calculate(point, vData.begin(), vData.end()); },
            addresses_size },
      { "sort data"s, "datasets sorted in vector"s,
            std::bind(Sorting, std::ref(vData)),
            addresses_size },
      { "write data to "s + strOutput_all, "datasets wrote to file"s,
            std::bind(Writing, std::cref(vData), std::cref(strOutput_all)),
            addresses_size },
      { "delete directory "s + strDirectory, "directories deleted"s,
            std::bind(DeleteDirectories, std::cref(strDirectory)),
            []() { std::optional<size_t> retval = { };  return retval; } },
      { "write data to directory "s + strDirectory, "datasets wrote to directory"s,
            std::bind(WriteToDirectory, std::cref(strDirectory), std::ref(vData)),
            addresses_size },
      { "delete data "s, "datasets still in alive"s,
            [&vData]() { vData.clear(); vData.shrink_to_fit(); },
            addresses_capacity },
      { "read data from directory "s + strDirectory, "datasets read from file"s,
            std::bind(ReadFromDirectory, std::cref(strDirectory), std::ref(vData)),
            addresses_size },
      { "partitioning data to "s, "datasets partitioned in vector"s,
            [&vData, &it]() { it = std::partition(std::execution::par, vData.begin(), vData.end(), [](auto const& val) {
                              return val.second.first < 1000.0; });  },
            value_size },
      { "sort partitioned data "s, "partitioned datasets sorted"s,
            [&vData, &it]() { std::sort(std::execution::par, vData.begin(), it, [](auto const& lhs, auto const& rhs) {
                                    if (lhs.second.first != rhs.second.first) return lhs.second.first > rhs.second.first;
                                    else return lhs.second.second < rhs.second.second; }); },
            value_size },
      { "write this data to "s + strOutput, "datasets wrote to file"s,
            [&vData, &it, &strOutput]() { WritingPart(vData.begin(), it, strOutput); } ,
            value_size }
      };
     
   auto time_for_all = myTimeType::zero();
   for (auto const& test_func : test_funcs) {
      std::cout << get_current_time_and_date() << " " << std::left << std::setw(45) << std::get<0>(test_func) << "-";
      try {
         auto func_start = std::chrono::high_resolution_clock::now();
         std::get<2>(test_func)();
         auto func_ende = std::chrono::high_resolution_clock::now();
         auto runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
         time_for_all += runtime;
         if (std::get<3>(test_func)()) { std::cout << std::right << std::setw(9) << *std::get<3>(test_func)() << " "; }
         else { std::cout << "          "; }
         std::cout << std::left << std::setw(30) << std::get<1>(test_func) << " in " << std::right
            << std::setw(9) << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";
      }
      catch (std::exception& ex) {
         std::cout << "error: " << ex.what() << "\n";
      }
   }
   std::cout << std::endl << std::left << "time for all operations " << std::right
      << std::setw(9) << std::setprecision(time_prec) << time_for_all.count() / time_factor << " sec\n";

   std::cout << "\nFinished.\n";
}


int main(int argc, char* argv[]) {
   std::string strInput = "D:\\Test"s;
   if (argc > 1) strInput = argv[1];
   std::ios_base::sync_with_stdio(false);
   std::cout.setf(std::ios::showpoint);
   std::cout.setf(std::ios::fixed);

   Rechentest(strInput);

   
   //std::thread t(Rechentest, std::cref(strInput));
   //t.join();

   #if defined __BORLANDC__
   std::cout << "... press a key ...";
   getchar();
   #endif
   return 0;
}

