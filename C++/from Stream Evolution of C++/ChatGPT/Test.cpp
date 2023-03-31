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
#include "ChatGPT.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <optional>

using namespace std::string_literals;

void Rechentest(std::string const& strFilename) {
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
            std::bind(TChatGPT::read_addresses_from_file, std::ref(vData), std::cref(strFilename)),
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
}

int main() {
   std::cout.setf(std::ios::showpoint);
   std::cout.setf(std::ios::fixed);
#if !defined __GNUC__
   std::cout.imbue(std::locale("de_DE"));
#endif
   std::string strInput = "D:\\Test\\berlin_infos.dat";
   Rechentest(strInput);
#if defined __BORLANDC__ || defined __GNUC__
   std::cout << "... press a key ...";
   getchar();
#endif
   return 0;
}

