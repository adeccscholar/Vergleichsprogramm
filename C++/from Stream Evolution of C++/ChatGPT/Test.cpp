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



void Rechentest(std::string const& strFilename) {
   using myTimeType = std::chrono::microseconds; // std::chrono::milliseconds
   static const double time_factor = 1000000.;
   static const int    time_prec = 6;

   data_vector vData;
   int value;
   Location    point         = { 52.520803, 13.40945 };
   std::string strOutput_all = "D:\\Test\\Testausgabe_alle.txt";
   std::string strOutput     = "D:\\Test\\Testausgabe.txt";
   std::string strDirectory  = "D:\\Test\\ChatGPT";

   std::vector<std::tuple<std::string, std::string, std::function<void()>, std::function<std::optional<size_t>()>>> test_funcs = {
      { "read file " + strFilename, "datasets read from file",
            std::bind(TChatGPT::read_from_File, std::ref(vData), std::cref(strFilename)),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "sort data", "datasets sorted in vector",
            //std::bind(TChatGPT::sort_DIN5007, std::ref(vData)),
            std::bind(TChatGPT::sort_DIN5007_Var2, std::ref(vData)),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "calculate data", "datasets calculated for point",
            std::bind(TChatGPT::calculate_addresses, std::ref(vData), std::cref(point)),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "write data to " + strOutput_all, "datasets wrote to file",
            std::bind(TChatGPT::write_to_File, std::ref(vData), std::cref(strOutput_all), -1),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "delete directory " + strDirectory, "directories deleted",
            std::bind(TChatGPT::delete_directory, std::cref(strDirectory)),
            []() { std::optional<size_t> retval = { };  return retval; } },
      { "write data to directory " + strDirectory, "datasets wrote to directory",
            //std::bind(TChatGPT::write_addresses_to_directories, std::cref(strDirectory), std::cref(vData)),
            std::bind(TChatGPT::write_addresses_to_directory_sorted, std::cref(strDirectory), std::ref(vData)),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "delete data ", "datasets still in alive",
            [&vData]() { vData.clear(); vData.shrink_to_fit(); },
            [&vData]() { return std::optional<size_t>(vData.capacity()); } },
      { "read data from directory " + strDirectory, "datasets read from file",
            std::bind(TChatGPT::read_addresses_from_directory, std::cref(strDirectory), std::ref(vData)),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "partitioning data to ", "datasets partitioned in vector",
            [&vData, &value]() { value = TChatGPT::pushMatchingToFront(vData, 1000.0);  },
            [&value]() { return std::optional<size_t>(value); } },
      { "sort partitioned data ", "partitioned datasets sorted",
            std::bind(TChatGPT::sortAddressesInRange, std::ref(vData), value),
            [&value]() { return std::optional<size_t>(value); } },
      { "write this data to " + strOutput, "datasets wrote to file",
            std::bind(TChatGPT::write_to_File, std::cref(vData), std::cref(strOutput), value),
            [&value]() { return std::optional<size_t>(value); } }
      };
   ///*
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
   //*/
/*
   auto func_start = std::chrono::high_resolution_clock::now();
   ReadChatGPT(vData, strFilename);
   auto func_ende = std::chrono::high_resolution_clock::now();
   auto runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " datasets read in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";


   func_start = std::chrono::high_resolution_clock::now();
   SortChatGPT_DIN5007(vData);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " datasets sorted in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   BerechneChatGPT(vData, point);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " datasets calculated in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   WriteChatGPT(vData, strOutput_all);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " datasets wrote to \"" << strOutput_all << "\" in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   deleteDirectory(strDirectory);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " deleted Directories \"" << strDirectory << "\" in "
             << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   //writeAddressesToDirectories(strDirectory, vData);
   write_addresses_to_directory_sorted(strDirectory, vData);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " wrote to Directory \"" << strDirectory << "\" in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   readAddressesFromDirectory(strDirectory, vData);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " read from Directory \"" << strDirectory << "\" in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   auto val = pushMatchingToFront(vData, 1000.0);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << val << " datasets partitioned in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   sortAddressesInRange(vData, val);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << val << " datasets sorted in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   WriteChatGPT(vData, strOutput, val);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << val << " datasets wrote to \"" << strOutput << "\" in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

*/



   std::cout << "\nFinished.\n";
}

int main() {
   std::cout.setf(std::ios::showpoint);
   std::cout.setf(std::ios::fixed);
   std::cout.imbue(std::locale("de_DE"));

   std::string strInput = "D:\\Test\\berlin_infos.dat";
   Rechentest(strInput);
#if defined __BORLANDC__
   std::cout << "... press a key ...";
   getchar();
#endif
   return 0;
}

