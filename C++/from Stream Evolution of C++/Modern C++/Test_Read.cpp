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

const func_vector<double> funcs = {
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
/*
const func_vector_vw<double> funcs_vw = {
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
*/
const func_vector_vw<double> funcs_vw = {
   [](TData<double>& data, std::string_view val) { data.City(val); } ,
   [](TData<double>& data, std::string_view val) { data.Street(val); } ,
   [](TData<double>& data, std::string_view val) { data.StreetNumber(val); },
   [](TData<double>& data, std::string_view val) { data.ZipCode(val); },
   [](TData<double>& data, std::string_view val) { data.UrbanUnit(val); },
   [](TData<double>& data, std::string_view val) { data.UrbanUnit_Old(val); },
   [](TData<double>& data, std::string_view val) { data.District(val); },
   [](TData<double>& data, std::string_view val) { data.Latitude(val); },
   [](TData<double>& data, std::string_view val) { data.Longitude(val); },
   [](TData<double>&, std::string_view const&) { throw std::runtime_error("unexpected number of elements.");  }
};

template <typename ty>
requires std::floating_point<ty>
inline size_t Read_0(data_vector<ty>& vData, func_vector<ty> const& funcs, std::istream& ifs) {
   size_t iLineCnt = 0u;
   std::string strRow;
   while (std::getline(ifs, strRow)) {
      if (strRow.length() > 2) {
         TData<double> data;
         auto input = tokenize(strRow, ";", 9);
         if (input.size() == 9) {
            for (size_t iCnt = 0u; auto const& element : input) { funcs[iCnt](data, input[iCnt]); ++iCnt; }
            vData.emplace_back(std::make_pair(std::forward<TData<double>>(data), Result<ty>()));
            ++iLineCnt;
         }
      }
   }
   return iLineCnt;
}

template <typename ty>
requires std::floating_point<ty>
inline size_t Read_1(data_vector<ty>& vData, func_vector<ty> const& funs, std::string const& buffer) {
   size_t pos = 0u;
   for (auto end = buffer.find('\n'); end != std::string::npos; pos = end + 1u, end = buffer.find('\n', pos)) {
      size_t iCnt = 0u;
      TData<ty> data;
      do {
         auto tmp = buffer.find(';', pos);
         if (tmp > end) tmp = end;
         funcs[iCnt++](data, buffer.substr(pos, tmp - pos));
         pos = tmp + 1;
      } while (pos < end);
      vData.emplace_back(std::make_pair(std::forward<TData<ty>>(data), Result<ty>()));
   }
   return vData.size();
}

template <typename ty>
requires std::floating_point<ty>
inline size_t Read_2(data_vector<ty>& vData, func_vector_vw<ty> const& funcs, std::string const& buffer) {
   std::string_view view(buffer.c_str(), buffer.size());
   using my_size_t = typename data_vector<ty>::size_type;
   using my_pair = std::pair< my_size_t, my_size_t>;
   for (auto [pos, end] = my_pair{ 0u, view.find('\n') }; end != std::string_view::npos; pos = end + 1u, end = view.find('\n', pos)) {
      size_t iCnt = 0u;
      TData<ty> data;
      do {
         auto tmp = view.find(';', pos);
         if (tmp > end) tmp = end;
         funcs[iCnt++](data, view.substr(pos, tmp - pos));
         pos = tmp + 1;
      } while (pos < end);
      vData.emplace_back(std::make_pair(std::forward<TData<ty>>(data), Result<ty>()));
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

template <typename ty>
requires std::floating_point<ty>
inline void Write(typename data_vector<ty>::const_iterator begin, typename data_vector<ty>::const_iterator end, std::ostream& os) {
   //os.setf(std::ios::showpoint);
   //os.setf(std::ios::fixed);
   //os.precision(6);
   /*
   std::for_each(vData.cbegin(), vData.cend(), [&os](auto const& val) {
      os << val.first.ZipCode() << " " << val.first.City() << " / " << val.first.UrbanUnit() << ", "
         << val.first.Street() << " " << val.first.StreetNumber()
         << " -> (" << std::setprecision(6) << val.first.Latitude() << ", "
         << std::setprecision(6) << val.first.Longitude() << ") -> "
         << std::setprecision(3) << val.second.first << "m in "
         << std::setprecision(1) << val.second.second << "°\n";
   */

   /*
   std::for_each(begin, end, [&os](auto const& val) {
      os << val.first.ZipCode() << " " << val.first.City() << " / " << val.first.UrbanUnit() << ", "
         << val.first.Street() << " " << val.first.StreetNumber()
         << " -> (" << my_Double_to_String_G(val.first.Latitude(), 6) << ", "
         << my_Double_to_String_G(val.first.Longitude(), 6) << ") -> "
         << my_Double_to_String_G(val.second.first, 3) << "m in "
         << my_Double_to_String_G(val.second.second, 1) << "°\n";
      });
   */
   ///*
   std::string strBuffer;
   std::for_each(begin, end, [&strBuffer](auto const& val) {
      auto const& [address, result] = val;
      std::format_to(std::back_inserter(strBuffer), "{};{};{};{};{};{:.9f};{:.9f};{:.3f};{:.1f}\n",
         address.ZipCode(), address.City(), address.UrbanUnit(), address.Street(), address.StreetNumber(),
         address.Latitude(), address.Longitude(), result.first, result.second);
      });
   os.write(strBuffer.data(), strBuffer.size());
   //*/
}


template <typename ty>
inline void Write(data_vector<ty> const& vData, std::ostream& os) {
   Write<ty>(vData.cbegin(), vData.cend(), os);
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
         } while (iEnd != std::string_view::npos);
      });
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
      } while (iEnd != std::string_view::npos);

   }
   return true;
}


auto Test7(std::string const& strFilename, size_t iTasks = 8) {
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
      for (; start < end; ++start) {
         int iCount = 0;
         size_t iPos = 0u, iEnd;
         do {
            iEnd = lines[start].view.find(';', iPos);
            funcs_vw[iCount++](vData[lines[start].index], lines[start].view.substr(iPos, iEnd - iPos));
            if (iEnd != std::string_view::npos) iPos = iEnd + 1;
         } while (iEnd != std::string_view::npos);

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

      for (auto& [test, description, output, func] : tests) {
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
   catch (std::exception& ex) {
      std::cerr << ex.what() << std::endl;
   }
}
