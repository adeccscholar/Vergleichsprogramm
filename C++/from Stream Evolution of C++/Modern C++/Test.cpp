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

using namespace std::literals;
namespace fs = std::filesystem;

const func_vector funcs = {
   [](TData<double>& data, std::string const& val) { data.City(val); } ,
   //nullptr,
   [](TData<double>& data, std::string const& val) { data.Street(val); } ,
   [](TData<double>& data, std::string const& val) { data.StreetNumber(val); },
   [](TData<double>& data, std::string const& val) { data.ZipCode(val); },
   [](TData<double>& data, std::string const& val) { data.UrbanUnit(val); },
   [](TData<double>& data, std::string const& val) { data.UrbanUnit_Old(val); },
   [](TData<double>& data, std::string const& val) { data.District(val); },
   [](TData<double>& data, std::string const& val) { data.Latitude(val); },
   [](TData<double>& data, std::string const& val) { data.Longitude(val); },
   [](TData<double>&, std::string const&) { throw std::runtime_error("unexpected number of elements.");  } 
};

const func_vector_vw funcs_vw = {
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


template <typename ty>
inline size_t Read_0(data_vector<ty>& vData, func_vector const& funcs, std::istream& ifs) {
   size_t iLineCnt = 0u;
   std::string strRow;
   while(std::getline(ifs, strRow)) {
      if(strRow.length() > 2) {
         TData<double> data;
         auto input = tokenize(strRow, ";", 9);
         if(input.size() == 9) {
            #if defined __BORLANDC__
               size_t iCnt = 0u; 
               for (auto const& element : input) { funcs[iCnt](data, input[iCnt]); ++iCnt; }
            #else
               for (size_t iCnt = 0u; auto const& element : input) { funcs[iCnt](data, input[iCnt]); ++iCnt; }
            #endif
            vData.emplace_back(std::make_pair(std::forward<TData<double>>(data), Result<ty>()));
            ++iLineCnt;
            }
         }
      }
   return iLineCnt;
   }

template <typename ty>
inline size_t Read_1(data_vector<ty>& vData, func_vector const& funs, std::string const& buffer) {
   size_t pos = 0u;
   for(auto end = buffer.find('\n'); end != std::string::npos; pos = end + 1u, end = buffer.find('\n', pos )) {
      size_t iCnt = 0u;
      TData<ty> data;
      do {
         auto tmp = buffer.find(';', pos);
         if (tmp > end) tmp = end;
         funcs[iCnt++](data, buffer.substr(pos, tmp - pos));
         pos = tmp + 1;
         } 
      while (pos < end);
         vData.emplace_back(std::make_pair(std::forward<TData<double>>(data), Result<ty>()));
   }
   return vData.size();
}

template <typename ty>
inline size_t Read_2(data_vector<ty>& vData, func_vector_vw const& funcs, std::string const& buffer) {
   std::string_view view(buffer.c_str(), buffer.size());
   using my_size_t = typename data_vector<ty>::size_type;
   using my_pair = std::pair< my_size_t, my_size_t>;
   for (auto [pos, end] = my_pair { 0u, view.find('\n') }; end != std::string_view::npos; pos = end + 1u, end = view.find('\n', pos)) {
      size_t iCnt = 0u;
      TData<ty> data;
      do {
         auto tmp = view.find(';', pos);
         if (tmp > end) tmp = end;
         funcs[iCnt++](data, view.substr(pos, tmp - pos));
         pos = tmp + 1;
      } while (pos < end);
      vData.emplace_back(std::make_pair(std::forward<TData<double>>(data), Result<ty>()));
   }
   return vData.size();
}



auto OpenFile(std::string const& strFilename, bool boText = true) {
   std::ifstream ifs(strFilename, (boText ? std::ifstream::in : std::ifstream::in | std::ifstream::binary));
   if (!ifs.is_open()) {
      std::ostringstream os;
      os << "File \"" << strFilename << "\" can't open!\n";
      throw std::runtime_error(os.str());
      }
   return ifs;
   }

auto Test1(std::string const& strFilename) {
   auto ifs = OpenFile(strFilename);
   data_vector<double> vecData;
   Read_0<double>(vecData, funcs, ifs);
   return vecData;
   }



// 36.533.626 36.536.320   (8.920)
std::array<char, 4'096 * 8'920> buffer;

auto Test2(std::string const& strFilename) {
   auto ifs = OpenFile(strFilename);
   data_vector<double> vecData;
   ifs.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
   Read_0<double>(vecData, funcs, ifs);
   return vecData;
}

auto Test3(std::string const& strFilename) {
   auto ifs = OpenFile(strFilename);
   ifs.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
   data_vector<double> vecData;
   vecData.reserve(std::count(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), '\n'));
   ifs.seekg(0, std::ios::beg);

   Read_0<double>(vecData, funcs, ifs);
   return vecData;
   }



auto Test4(std::string const& strFilename) {
   auto ifs = OpenFile(strFilename);
   ifs.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
   ifs.seekg(0, std::ios::end);
   
   auto size = ifs.tellg();
   ifs.seekg(0, std::ios::beg);
   std::string strBuffer(size, '\0');
   ifs.read(strBuffer.data(), size);
   ifs.close();
   
   data_vector<double> vData;
   #if defined __BORLANDC__
      vData.reserve(std::count(strBuffer.begin(), strBuffer.end(), '\n'));
   #else
      vData.reserve(std::count(std::execution::par, strBuffer.begin(), strBuffer.end(), '\n'));
   #endif
   std::istringstream iss(std::forward<std::string>(strBuffer));

   Read_0<double>(vData, funcs, iss);
   return vData;
   }

auto Test5(std::string const& strFilename) {
   auto ins = OpenFile(strFilename, false);
   ins.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
   ins.seekg(0, std::ios::end);
   auto size = ins.tellg();
   ins.seekg(0, std::ios::beg);
   std::string strBuffer(size, '\0');
   ins.read(strBuffer.data(), size);
   ins.close();
   data_vector<double> vData;

   #if defined __BORLANDC__
      vData.reserve(std::count(strBuffer.begin(), strBuffer.end(), '\n'));
   #else
      vData.reserve(std::count(std::execution::par, strBuffer.begin(), strBuffer.end(), '\n'));
   #endif
   Read_1<double>(vData, funcs, strBuffer);
   return vData;
   }

inline auto Test6(std::string const& strFilename) {
   const auto iSize = fs::file_size(strFilename);
   auto ins = OpenFile(strFilename, false);
   ins.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
   
   std::string strBuffer;
   strBuffer.resize(iSize);
   ins.read(strBuffer.data(), iSize);
   ins.close();
   std::string_view test(strBuffer.data(), strBuffer.size());
   
   my_line_count::reset();
   std::vector<my_line_count> lines;
   #if defined __BORLANDC__
   size_t size = std::count(test.begin(), test.end(), '\n');
   #else
   size_t size = std::count(std::execution::par, test.begin(), test.end(), '\n');
   #endif
   lines.reserve(size);
   my_lines file_data(test);
   std::copy(file_data.begin(), file_data.end(), std::back_inserter(lines));
   data_vector<double> vData;
   vData.resize(lines.size());
   #if defined __BORLANDC__
   std::for_each( 
   #else
   std::for_each(std::execution::par, 
   #endif
                              lines.begin(), lines.end(), [&vData](auto const& val) {
                                                  size_t iPos = 0u, iEnd;
                                                  int iCount = 0;  // funcs_vw
                                                  do {
                                                     iEnd = val.view.find(';', iPos);
                                                     funcs_vw[iCount++](vData[val.index].first, val.view.substr(iPos, iEnd - iPos));
                                                     if (iEnd != std::string_view::npos) iPos = iEnd + 1;
                                                     } 
                                                  while (iEnd != std::string_view::npos);
                                                  } );
     return vData;
}


bool tfunc(std::vector<my_line_count>& lines, data_vector<double>& vData, size_t start, size_t end) {
   for (; start < end; ++start) {
      int iCount = 0;
      size_t iPos = 0u, iEnd;
      do {
         iEnd = lines[start].view.find(';', iPos);
         funcs_vw[iCount++](vData[lines[start].index].first, lines[start].view.substr(iPos, iEnd - iPos));
         if (iEnd != std::string_view::npos) iPos = iEnd + 1;
         } 
      while (iEnd != std::string_view::npos);

      }
   return true;
   }


auto Test7(std::string const& strFilename, size_t iTasks = 8 ) {
   using namespace std::placeholders;
   const auto iSize = fs::file_size(strFilename);
   auto ins = OpenFile(strFilename, false);
   ins.rdbuf()->pubsetbuf(buffer.data(), buffer.size());

   std::string strBuffer(iSize, '\0');
   ins.read(strBuffer.data(), iSize);
   ins.close();
   std::string_view test(strBuffer.data(), strBuffer.size());

   my_line_count::reset();
   std::vector<my_line_count> lines;
   lines.reserve(std::count(test.begin(), test.end(), '\n'));
   my_lines file_data(test);
   std::copy(file_data.begin(), file_data.end(), std::back_inserter(lines));
   data_vector<double> vData;
   vData.resize(lines.size());

   std::vector<std::future<bool>> tasks(iTasks);

   auto r_func = [&lines, &vData](auto start, auto end) {
         for(;start < end; ++start) {
            int iCount = 0;
            size_t iPos = 0u, iEnd;
            do {
               iEnd = lines[start].view.find(';', iPos);
               funcs_vw[iCount++](vData[lines[start].index], lines[start].view.substr(iPos, iEnd - iPos));
               if (iEnd != std::string_view::npos) iPos = iEnd + 1;
               } 
            while (iEnd != std::string_view::npos);

            }
         return true;
         };

   auto iCnt = vData.size();
   auto iElem = iCnt / iTasks;
   for (int i = 0; i < iTasks; ++i) {   
      //tasks[i] = std::async(r_func, i * iElem, (i + 1) == iTasks ? iCnt : (i + 1) * iElem);
      //std::launch::deferred
      tasks[i] = std::async(std::launch::async, std::bind(tfunc, std::ref(lines), std::ref(vData), _1, _2), 
                            i * iElem, (i + 1) == iTasks ? iCnt : (i + 1) * iElem);
      }

   bool boRetVal;
   for (auto& task : tasks) if (!task.get()) boRetVal = false;

   return vData;
}


void Test(std::string const& strFilename) {
   using namespace std::placeholders;
   static std::vector<std::tuple<std::string, std::string, std::string, std::function< data_vector<double>(std::string const&)>>> tests = {
      { "Test1"s, "1st Test, Reading sequential with getline, vector dynamic increased."s,    "D:\\Test\\Testausgabe1.txt"s, Test1 },
      { "Test2"s, "2nd Test, Reading with getline, with buffer, vector dynamic increased."s,  "D:\\Test\\Testausgabe2.txt"s, Test2 },
      { "Test3"s, "3rd Test, Reading twice with getline, read buffer, vector prereserved."s,  "D:\\Test\\Testausgabe3.txt"s, Test3 },
      { "Test4"s, "4th Test, Reading twice over stringstream with getline, vector reserved"s, "D:\\Test\\Testausgabe4.txt"s, Test4 },
      { "Test5"s, "5th Test, string operations, vector reserved."s,                           "D:\\Test\\Testausgabe5.txt"s, Test5 },
      { "Test6"s, "6th Test, our iterator, vector reserved."s,                                "D:\\Test\\Testausgabe6.txt"s, Test6 },
      { "Test7"s, "7th Test, our iterator, vector reserved, async."s,        "D:\\Test\\Testausgabe7.txt"s, std::bind(Test7, _1, 16) }
      };
   try {

      for(auto& [test, description, output, func] : tests) {
         std::cout << description << "\n";
         auto func_start = std::chrono::high_resolution_clock::now();

         auto vData = func(strFilename);

         auto func_ende = std::chrono::high_resolution_clock::now();
         auto runtime = std::chrono::duration_cast<std::chrono::milliseconds>(func_ende - func_start);
      
         std::cout << vData.size() << " datasets read in "
                   << std::setw(12) << std::setprecision(3) << runtime.count() / 1000. << " sec\n";

         std::ofstream ofs(output);
         Write<double>(vData, ofs);
         std::cout << "...\n\n";
         }
      }
   catch(std::exception& ex) {
      std::cerr << ex.what() << std::endl;
      }
   }


inline void Sorting(data_vector<double>& vData) {

   static const std::string special_chars = "äöüßÄÖÜáèéçñ"s;
   static const std::map<char, std::string> special_values = { {'ä',"ae"s}, {'Ä',"Ae"s },{'ü',"ue"s},{'Ü',"Ue"s},{'ß',"ss"s},
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


void Rechentest(std::string const& strFilename) {
   using myTime_Duration = std::chrono::microseconds;
   static const double time_factor = 1000000.;
   static const int    time_precision = 6;
   //using myTime_Duration = std::chrono::milliseconds;
   //static const double time_factor    = 1000.;
   //static const int    time_precision = 3;

   auto func_start = std::chrono::high_resolution_clock::now();

   auto vData = Test6(strFilename);

   auto func_ende = std::chrono::high_resolution_clock::now();
   auto runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets read in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   Sorting(vData);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets sorted in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   Location<double> point = { 52.520803, 13.40945 };
   Calculate(point, vData.begin(), vData.end());
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets calculated in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";

   std::string strOutput = "D:\\Test\\Testausgabe_alle.txt"s;
   std::ostringstream os;
   func_start = std::chrono::high_resolution_clock::now();
   std::ofstream ofs(strOutput);
   Write<double>(vData, os);
   ofs.write(os.str().data(), os.str().size());
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets wrote to \"" << strOutput << "\" in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";


   strOutput = "D:\\Test\\Testausgabe.txt"s;
   std::cout << "\n Write file \"" << strOutput << "\"...\n";

   func_start = std::chrono::high_resolution_clock::now();
   auto it = std::partition(std::execution::par, vData.begin(), vData.end(), [](auto const& val) {
      return val.second.first < 1000.0;
      });
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << vData.size() << " datasets partition in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   std::sort(std::execution::par, vData.begin(), it, [](auto const& lhs, auto const& rhs) {
      if (auto cmp = lhs.second.first <=> rhs.second.first; cmp != 0) return cmp > 0;
      else return lhs.second.second < rhs.second.second;
      });
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << std::distance(vData.begin(), it) << " datasets sorted in "
      << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";




   ofs.close();
   std::ostringstream os2;
   func_start = std::chrono::high_resolution_clock::now();
   ofs.open(strOutput);
   Write<double>(vData.begin(), it, os2);
   ofs.write(os2.str().data(), os2.str().size());
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTime_Duration>(func_ende - func_start);
   std::cout << std::distance(vData.begin(), it) << " datasets wrote to \"" << strOutput << "\" in "
             << std::setprecision(time_precision) << runtime.count() / time_factor << " sec\n";
   std::cout << "\nFinished.\n";




}

int main() {
   std::ios_base::sync_with_stdio(false);
   std::cout.setf(std::ios::showpoint);
   std::cout.setf(std::ios::fixed);

   //TData<double> data;
   //Location<double> point  = { 2.0, 1.0 };
   //Calculate<double>(data, point);
 
   //Location<double> point = { 2.0, 1.0 };
   //Location<int> point1 = { 2, 1 };

   
   std::string strInput = "D:\\Test\\berlin_infos.dat"s;
   //std::thread t(Test, std::cref(strInput));
   //t.join();
   Rechentest(strInput);
   #if defined __BORLANDC__
   std::cout << "... press a key ...";
   getchar();
   #endif
   return 0;
}

