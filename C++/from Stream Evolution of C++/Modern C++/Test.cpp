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
inline void WritingPart(ty begin, ty end, std::string const& strFile) {
   /*
   std::ofstream of(strFile);
   std::string dummy;
   dummy.reserve(176 * vData.size());
   std::ostringstream os(forward<std::string>(dummy));
   //std::for_each(begin, end, [&os](auto& val) {
      auto const& [address, result]= val;
      std::string strCity = address.City() + " - "s + address.District() + ", "s;
      std::string strStreet = address.Street() + " "s + address.StreetNumber();
      os << std::left << std::setw(5) << address.ZipCode() << " "
         << std::setw(75) << strCity
         << std::setw(65) << strStreet << "="s
         << my_Double_to_String_G(result.first, 3) << "m / "s
         << my_Double_to_String_G(result.second, 3) << "°\n"s;
      }
   of << os.str();
   //*/


   //*
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
   //*/
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
   //std::sort(vData.begin(), vData.end(), [](auto const& lhs, auto const& rhs) {
      auto const& a = lhs.first;
      auto const& b = rhs.first;
      if (auto cmp = compare(a.City(), b.City()); cmp != 0) return cmp < 0;
      if (auto cmp = compare(a.UrbanUnit(), b.UrbanUnit()); cmp != 0) return cmp < 0;
      if (auto cmp = compare(a.District(), b.District()); cmp != 0) return cmp < 0;
      if (auto cmp = a.ZipCode() <=> b.ZipCode(); cmp != 0) return cmp < 0;
      if (auto cmp = compare(a.Street(), b.Street()); cmp != 0) return cmp < 0;
      return compare_streetnumber2(a.StreetNumber(), b.StreetNumber());
      });


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

inline void ReadFromDirectory(std::string const& strPath, data_vector<double>& vData) {
   using detail_data = std::tuple<std::string, std::string, std::string, std::string>;
   using data_to_read = std::tuple<fs::path, std::string, std::vector<my_line_count>, detail_data>;
   using to_read_vec = std::vector<data_to_read>;
   to_read_vec to_read;

   try {
      std::vector<fs::path> cities;
      std::copy(fs::directory_iterator(strPath), fs::directory_iterator(), std::back_inserter(cities));
      auto it_cities = std::partition(std::execution::par, cities.begin(), cities.end(), [](auto const& p) { return fs::is_directory(p); });
      std::for_each(cities.begin(), it_cities, [&to_read](auto& d) {
         std::string strCities = d.stem().string();
         std::vector<fs::path> units;
         std::copy(fs::directory_iterator(d), fs::directory_iterator(), std::back_inserter(units));
         auto it_units = std::partition(std::execution::par, units.begin(), units.end(), [](auto const& p) { return fs::is_directory(p); });
         std::for_each(units.begin(), it_units, [&to_read, &strCities](auto& d) {
            std::string strUnit = d.stem().string();
            std::vector<fs::path> districts;
            std::copy(fs::directory_iterator(d), fs::directory_iterator(), std::back_inserter(districts));
            auto it_districts = std::partition(std::execution::par, districts.begin(), districts.end(), [](auto const& p) { return fs::is_directory(p); });
            std::for_each(districts.begin(), it_districts, [&to_read, &strCities, &strUnit](auto& d) {
               std::string strDistrict = d.stem().string();
               std::vector<fs::path> streets;
               std::copy(fs::directory_iterator(d), fs::directory_iterator(), std::back_inserter(streets));
               auto it_streets = std::partition(std::execution::par, streets.begin(), streets.end(), [](auto const& p) { return fs::is_is_regular_file(p); });
               std::for_each(streets.begin(), it_streets, [&to_read, &strCities, &strUnit, &strDistrict](auto& d) {
                  std::string strStreet = d.stem().string();
                  detail_data data = { strCities, strUnit, strDistrict, strStreet };
                  to_read.emplace_back(d, std::string(), std::vector<my_line_count>(), std::forward<detail_data>(data));
                  });
               });
            });

         });

   }
   catch(std::exception& ex) {
      std::cerr << "error while preparing reading: " << ex.what() << std::endl;
      }
   }

inline void WriteToDirectory(std::string const& strPath, data_vector<double>& vData) {
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

   static constexpr auto compare = [](auto const& a, auto const& b) noexcept {
     // if (auto cmp = std::tie(a.first.City(), a.first.UrbanUnit(), a.first.District(), a.first.Street()) <=>
     //                std::tie(b.first.City(), b.first.UrbanUnit(), b.first.District(), b.first.Street()); cmp != 0) return cmp < 0;
      if (auto cmp = a.first.City() <=> b.first.City(); cmp != 0) return cmp < 0;
      if (auto cmp = a.first.UrbanUnit() <=> b.first.UrbanUnit(); cmp != 0) return cmp < 0;
      if (auto cmp = a.first.District() <=> b.first.District(); cmp != 0) return cmp < 0;
      if (auto cmp = a.first.Street() <=> b.first.Street(); cmp != 0) return cmp < 0;
      return compare_streetnumber2(a.first.StreetNumber(), b.first.StreetNumber());
      };

   std::sort(std::execution::par, vData.begin(), vData.end(), compare);

   using tplParts = std::tuple<data_vector<double>::const_iterator, data_vector<double>::const_iterator, std::ostringstream>;
   using myIterPair = std::tuple<data_vector<double>::const_iterator, data_vector<double>::const_iterator, fs::path>;
   std::vector<myIterPair> positions;
   fs::path myOldPath = ""s;
   for(auto first = vData.begin(); first != vData.end();) {
      auto const& [address, result] = *first;
      fs::path myPath = fs::path(strPath) / address.City() / address.UrbanUnit() / address.District();
      if(myPath != myOldPath) {
         fs::create_directories(myPath);
         myOldPath = myPath;
         }
      auto last = std::find_if(std::execution::par, first, vData.end(), [&first](auto const& val) {
         return first->first.Street() != val.first.Street() ? true : 
            (first->first.District() != val.first.District() ? true : 
               (first->first.UrbanUnit() != val.first.UrbanUnit() ? true : 
                  (first->first.City() != val.first.City() ? true : false)));
         });
      positions.emplace_back(first, last, myPath / (first->first.Street() + ".csv"s));
      first = last;
      }

   auto size = std::distance(positions.cbegin(), positions.cend()) * 70;
   std::for_each(std::execution::par, positions.cbegin(), positions.cend(), [size](auto const& val) {
      std::string strBuffer;
      strBuffer.reserve(size);
      std::for_each(std::get<0>(val), std::get<1>(val), [&strBuffer](auto const& value) {
         //auto const& [address, result] = value;
         /*
         ofs << address.StreetNumber() << ";" << address.ZipCode() << ";" << address.UrbanUnit_Old() << ";"
            << my_Double_to_String(address.Latitude(), 9) << ";" << my_Double_to_String(address.Longitude(), 9) << ";"
            << my_Double_to_String(result.first, 3) << ";" << my_Double_to_String(result.second, 1) << std::endl;
         */
         //*
         std::format_to(std::back_inserter(strBuffer), "{};{};{};{};{};{};{}\n",
                            value.first.StreetNumber(), value.first.ZipCode(), value.first.UrbanUnit_Old(),
                            my_Double_to_String_short(value.first.Latitude(), 9), my_Double_to_String_short(value.first.Longitude(), 9),
                            my_Double_to_String_short(value.second.first, 3), my_Double_to_String_short(value.second.second, 3));
         //*/
         /*
         std::format_to(std::back_inserter(strBuffer), "{};{};{};{:.9f};{:.9f};{:.3f};{:.3f}\n",
                        value.first.StreetNumber(), value.first.ZipCode(), value.first.UrbanUnit_Old(), 
                        value.first.Latitude(), value.first.Longitude(),
                        value.second.first, value.second.second);
         */
         });
      std::ofstream(std::get<2>(val)).write(strBuffer.data(), strBuffer.size());
      });

}





void Rechentest(std::string const& strFilename) {
   using myTime_Duration = std::chrono::microseconds;
   static const double time_factor = 1000000.;
   static const int    time_precision = 6;
   //using myTime_Duration = std::chrono::milliseconds;
   //static const double time_factor    = 1000.;
   //static const int    time_precision = 3;

   data_vector<double> vData;
   data_vector<double>::iterator it;
   Location<double> point    = { 52.520803, 13.40945 };
   std::string strOutput_all = "D:\\Test\\Testausgabe_alle.txt"s;
   std::string strOutput     = "D:\\Test\\Testausgabe.txt"s;

   auto func_start = std::chrono::high_resolution_clock::now();
   Reading(vData, strFilename);

   auto func_ende = std::chrono::high_resolution_clock::now();
   auto runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets read in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   Calculate(point, vData.begin(), vData.end());
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets calculated in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   Sorting(vData);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets sorted in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";


   func_start = std::chrono::high_resolution_clock::now();
   Writing(vData, strOutput_all);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets wrote to \"" << strOutput << "\" in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";


   func_start = std::chrono::high_resolution_clock::now();
   DeleteDirectories("d:\\Test");
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << " directies deleted"
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   WriteToDirectory("D:\\Test", vData);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets wrote to directies "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";




  func_start = std::chrono::high_resolution_clock::now();
  it = std::partition(std::execution::par, vData.begin(), vData.end(), [](auto const& val) {
      return val.second.first < 1000.0;
      });
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets partition in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   std::sort(std::execution::par, vData.begin(), it, [](auto const& lhs, auto const& rhs) {
      if (lhs.second.first != rhs.second.first) return lhs.second.first > rhs.second.first;
      else return lhs.second.second < rhs.second.second;
      });
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << std::distance(vData.begin(), it) << " datasets sorted in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";



   func_start = std::chrono::high_resolution_clock::now();
   WritingPart(vData.begin(), it, strOutput);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << std::distance(vData.begin(), it) << " datasets wrote to \"" << strOutput << "\" in "
             << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";
   std::cout << "\nFinished.\n";



                                                                                                                                                                                                         
}

/*
   using myTimeType = std::chrono::microseconds; // std::chrono::milliseconds
   static const double time_factor = 1000000.;
   static const int    time_prec = 6;

   data_vector vData;
   int value = 0;
   Location    point         = { 52.520803, 13.40945 };
   std::string strOutput_all = "D:\\Test\\Testausgabe_alle.txt"s;
   std::string strOutput     = "D:\\Test\\Testausgabe.txt"s;
   std::string strDirectory  = "D:\\Test\\ChatGPT"s;

   using call_func = std::function<void()>;
   using mySizeRet = std::optional<size_t>;
   using size_func = std::function<mySizeRet()>;
   static size_func addresses_size = [&vData]() -> mySizeRet {
      return std::make_optional<size_t>( vData.size() );
      };

   static size_func addresses_capacity = [&vData]() -> mySizeRet {
      return std::make_optional<size_t>(vData.capacity());
      };

   static size_func value_size = [&value]() -> mySizeRet {
      return std::make_optional<size_t>(value);
      };

   std::vector<std::tuple<std::string, std::string, call_func, size_func>> test_funcs = {
      { "read file "s + strFilename, "datasets read from file"s,
            std::bind(Reading, std::ref(vData), std::cref(strFilename)),
            addresses_size },
      { "calculate data"s, "datasets calculated for point"s,
            std::bind(TChatGPT::calculate_addresses, std::ref(vData), std::cref(point)),
            addresses_size },
      { "sort data"s, "datasets sorted in vector"s,
            //std::bind(TChatGPT::sort_DIN5007, std::ref(vData)),
            std::bind(TChatGPT::sort_DIN5007_Var2, std::ref(vData)),
            addresses_size },
      { "write data to "s + strOutput_all, "datasets wrote to file"s,
            std::bind(TChatGPT::write_addresses_to_file, std::ref(vData), std::cref(strOutput_all)),
            addresses_size },
      { "delete directory "s + strDirectory, "directories deleted"s,
            std::bind(TChatGPT::delete_directory, std::cref(strDirectory)),
            []() { std::optional<size_t> retval = { };  return retval; } },
      { "write data to directory "s + strDirectory, "datasets wrote to directory"s,
            //std::bind(TChatGPT::write_addresses_to_directories, std::cref(strDirectory), std::cref(vData)),
            std::bind(TChatGPT::write_addresses_to_directory_sorted, std::cref(strDirectory), std::ref(vData)),
            addresses_size },
      { "delete data "s, "datasets still in alive"s,
            [&vData]() { vData.clear(); vData.shrink_to_fit(); },
            addresses_capacity },
      { "read data from directory "s + strDirectory, "datasets read from file"s,
            std::bind(TChatGPT::read_addresses_from_directory, std::cref(strDirectory), std::ref(vData)),
            addresses_size },
      { "partitioning data to "s, "datasets partitioned in vector"s,
            [&vData, &value]() { value = TChatGPT::push_matching_to_front(vData, 1000.0);  },
            value_size },
      { "sort partitioned data "s, "partitioned datasets sorted"s,
            std::bind(TChatGPT::sort_addresses_in_range, std::ref(vData), std::cref(value)),
            value_size },
      { "write this data to "s + strOutput, "datasets wrote to file"s,
            std::bind(TChatGPT::write_part_addresses_to_file, std::cref(vData), std::cref(strOutput), std::cref(value)),
            value_size }

      };

   myTimeType time_for_all = myTimeType::zero();
   for (auto const& test_func : test_funcs) {
      std::cout << std::left << std::setw(48) << std::get<0>(test_func) << " ... ";
      try {
         auto func_start = std::chrono::high_resolution_clock::now();
         std::get<2>(test_func)();
         auto func_ende = std::chrono::high_resolution_clock::now();
         auto runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
         time_for_all += runtime;
         if (std::get<3>(test_func)()) { std::cout << std::right << std::setw(10) << *std::get<3>(test_func)() << " "; }
         else { std::cout << "           "; }
         std::cout << std::left << std::setw(30) << std::get<1>(test_func) << " in " << std::right
            << std::setw(10) << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";
      }
      catch (std::exception& ex) {
         std::cout << "error: " << ex.what() << "\n";
      }
   }
   std::cout << std::endl << std::left << "time for all operations " << std::right
      << std::setw(10) << std::setprecision(time_prec) << time_for_all.count() / time_factor << " sec\n";

   std::cout << "\nFinished.\n";

*/


int main() {
   std::ios_base::sync_with_stdio(false);
   std::cout.setf(std::ios::showpoint);
   std::cout.setf(std::ios::fixed);

   std::string strInput = "D:\\Test\\berlin_infos.dat"s;

   //std::thread t(Rechentest, std::cref(strInput));
   //t.join();

   Rechentest(strInput);
   #if defined __BORLANDC__
   std::cout << "... press a key ...";
   getchar();
   #endif
   return 0;
}

