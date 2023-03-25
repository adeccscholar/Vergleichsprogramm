#include "Processes_Data.h"
#include "My_Lines_Iterator.h"
#include "Data.h"

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <execution>
#include <vector>
#include <stdexcept>

void Read3(std::string const& strFile, myData& vec_data) {

   std::ifstream ifs(strFile);
   if (!ifs.is_open()) {
      std::ostringstream os;
      os << "error while opeining file \"" << strFile << "\"" << std::endl;
      throw std::runtime_error(os.str().c_str());
   }
   std::stringstream strStream;
   strStream << ifs.rdbuf();
   ifs.close();
   std::string_view test(strStream.view());

   std::vector<my_line<TData>> lines;
   lines.reserve(count(test.begin(), test.end(), '\n'));
   my_lines file_data(test);
   copy(file_data.begin(), file_data.end(), back_inserter(lines));
   vec_data.reserve(lines.size());

   std::for_each(std::execution::par, lines.begin(), lines.end(), [](auto& val) mutable {
      size_t iPos = 0, iEnd;
      int iCount = 0;
      do {
         iEnd = val.view.find(";", iPos);
         funcs_read_f[iCount++](val.data, val.view.substr(iPos, iEnd - iPos));
         if (iEnd != std::string_view::npos) iPos = iEnd + 1;
      } while (iEnd != std::string_view::npos);
      });

   std::for_each(lines.begin(), lines.end(), [&vec_data](auto&& val) {
      vec_data.emplace_back(std::forward<TData>(val));
      });
}

void Read2(std::string const& strFile, myData& vec_data) {
   std::ifstream ifs(strFile);
   if (!ifs.is_open()) {
      std::ostringstream os;
      os << "error while opeining file \"" << strFile << "\"" << std::endl;
      throw std::runtime_error(os.str().c_str());
   }
   std::stringstream strStream;
   strStream << ifs.rdbuf();
   ifs.close();
   std::string_view test(strStream.view());
   vec_data.reserve(std::count(test.begin(), test.end(), '\n'));
   size_t start_pos = 0, end_pos = 0;
   size_t iPos = 0, iEnd;
   int iCount;
   TData data;
   while ((end_pos = test.find('\n', start_pos)) != std::string_view::npos) {
      iPos = start_pos;
      iCount = 0;
      do {
         iEnd = test.find(";", iPos);
         if (iEnd > end_pos) iEnd = end_pos;
         funcs_read_f[iCount++](data, test.substr(iPos, iEnd - iPos));
         if (iEnd != end_pos) iPos = iEnd + 1;
      } while (iEnd != end_pos);
      vec_data.emplace_back(std::move(data));
      start_pos = end_pos + 1;
   }
   return;
}

template <typename container, typename functions>
void Read_all(std::string const& strFile, container& vec_data, functions const& funcs) {
   std::ifstream ifs(strFile);
   if (ifs.is_open()) {
      typename container::value_type data;
      std::string strRow;
      while (std::getline(ifs, strRow)) {
         int iCount = 0;
         size_t iPos = 0, iEnd;
         do {
            iEnd = strRow.find(";", iPos);
            funcs[iCount++](data, strRow.substr(iPos, iEnd - iPos));
            iPos = iEnd + 1;
            }
         while (iEnd != std::string::npos);
         vec_data.emplace_back(std::move(data));
         }
      }
   else {
      std::ostringstream os;
      os << "error while opeining file \"" << strFile << "\"" << std::endl;
      throw std::runtime_error(os.str().c_str());
      }
   }

void Read1_templ(std::string const& strFile, myData& vec_data) {
   static std::vector<std::function<void(TData&, std::string&&)>> funcs = {
         [](TData& data, std::string&& val) { data.City(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Street(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.StreetNumber(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.ZipCode(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Urban_Unit(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Old_Unit(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.District(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Latitude(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Longitude(std::forward<std::string>(val)); }
   };
   Read_all(strFile, vec_data, funcs);
   }

void Read1(std::string const& strFile, myData& vec_data) {
   static std::vector<std::function<void(TData&, std::string&&)>> funcs = {
         [](TData& data, std::string&& val) { data.City(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Street(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.StreetNumber(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.ZipCode(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Urban_Unit(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Old_Unit(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.District(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Latitude(std::forward<std::string>(val)); },
         [](TData& data, std::string&& val) { data.Longitude(std::forward<std::string>(val)); }
   };

   std::ifstream ifs(strFile);
   if (ifs.is_open()) {
      //vec_data.reserve(std::count(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), '\n'));
      //ifs.seekg(0, ios::beg);
      TData data;
      std::string strRow;
      while (std::getline(ifs, strRow)) {
         int iCount = 0;
         size_t iPos = 0, iEnd;
         do {
            iEnd = strRow.find(";", iPos);
            funcs[iCount++](data, strRow.substr(iPos, iEnd - iPos));
            iPos = iEnd + 1;
         } while (iEnd != std::string::npos);
         vec_data.emplace_back(std::move(data));
      }
   }
   else {
      std::ostringstream os;
      os << "error while opeining file \"" << strFile << "\"" << std::endl;
      throw std::runtime_error(os.str().c_str());
   }
   return;
}

void Read(std::string const& strFile, myData& vec_data) {
   static const std::vector<std::function<void(TData&, std::string const&)>> funcs_rrr = {
         [](TData& data, std::string const& val) { data.City(val); },
         [](TData& data, std::string const& val) { data.Street(val); },
         [](TData& data, std::string const& val) { data.StreetNumber(val); },
         [](TData& data, std::string const& val) { data.ZipCode(val); },
         [](TData& data, std::string const& val) { data.Urban_Unit(val); },
         [](TData& data, std::string const& val) { data.Old_Unit(val); },
         [](TData& data, std::string const& val) { data.District(val); },
         [](TData& data, std::string const& val) { data.Latitude(val); },
         [](TData& data, std::string const& val) { data.Longitude(val); }
   };

   std::ifstream ifs(strFile);
   if (ifs.is_open()) {
      //vec_data.reserve(std::count(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), '\n'));
      //ifs.seekg(0, std::ios::beg);
      TData data;
      std::string strRow;
      size_t iPos, iEnd, iCount;
      while (std::getline(ifs, strRow)) {
         iCount = 0; 
         iPos = 0;
         do {
            iEnd = strRow.find(";", iPos);
            funcs_rrr[iCount++](data, strRow.substr(iPos, iEnd - iPos));
            iPos = iEnd + 1;
         } while (iEnd != std::string::npos);
         vec_data.push_back(data);
      }
   }
   else {
      std::ostringstream os;
      os << "error while opeining file \"" << strFile << "\"" << std::endl;
      throw std::runtime_error(os.str().c_str());
   }
   return;
}


void Read0(std::string const& strFile, myData& vec_data) {
   std::ifstream ifs(strFile);

   if (ifs.is_open()) {
      //vec_data.reserve(count(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>(), '\n'));
      //ifs.seekg(0, ios::beg);
      TData data;
      std::string strRow;
      while (std::getline(ifs, strRow)) {
         int iCount = 0;
         size_t iPos = 0, iEnd;

         iEnd = strRow.find(";", iPos);
         data.City(strRow.substr(iPos, iEnd - iPos));
         iPos = iEnd + 1;
         iEnd = strRow.find(";", iPos);
         data.Street(strRow.substr(iPos, iEnd - iPos));
         iPos = iEnd + 1;
         iEnd = strRow.find(";", iPos);
         data.StreetNumber(strRow.substr(iPos, iEnd - iPos));
         iPos = iEnd + 1;
         iEnd = strRow.find(";", iPos);
         data.ZipCode(strRow.substr(iPos, iEnd - iPos));
         iPos = iEnd + 1;
         iEnd = strRow.find(";", iPos);
         data.Urban_Unit(strRow.substr(iPos, iEnd - iPos));
         iPos = iEnd + 1;
         iEnd = strRow.find(";", iPos);
         data.Old_Unit(strRow.substr(iPos, iEnd - iPos));
         iPos = iEnd + 1;
         iEnd = strRow.find(";", iPos);
         data.District(strRow.substr(iPos, iEnd - iPos));
         iPos = iEnd + 1;
         iEnd = strRow.find(";", iPos);
         double flValue = atof(strRow.substr(iPos, iEnd - iPos).c_str());
         data.Latitude(flValue);
         iPos = iEnd + 1;
         iEnd = strRow.find(";", iPos);
         flValue = atof(strRow.substr(iPos, iEnd - iPos).c_str());
         data.Longitude(flValue);
         vec_data.push_back(data);
      }
   }
   else {
      std::ostringstream os;
      os << "error while opeining file \"" << strFile << "\"" << std::endl;
      throw std::runtime_error(os.str().c_str());
   }
   return;
}
