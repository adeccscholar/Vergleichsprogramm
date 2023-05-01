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


#include "Tool_Helper.h"
#include "MyCheck.h"
#include "MyProcess.h"
#include "MyTestStatistcs.h"

#include <iostream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <string_view> // !! C++17
#include <vector>
#include <map>
#include <fstream>
#include <format>

#include <chrono>



void Rechentest(fs::path const& directory, int iCount = 1) {
   using myTimeType = std::chrono::microseconds;  // std::chrono::milliseconds
   static const double time_factor = 1000000.;    // 1000.
   static const int    time_prec   = 6;           // 3

   data_vector<double> vData;
   data_vector<double>::iterator it;
   Location<double> point    = { 52.520803, 13.40945 };
   static const std::string strFilename   = "berlin_infos.dat";
   static const std::string strOutput_all = "Testausgabe_alle.txt";
   static const std::string strOutput     = "Testausgabe.txt";

   fs::path    filename                   = directory / strFilename;
   fs::path    output_all                 = directory / strOutput_all;
   fs::path    output                     = directory / strOutput;

   std::vector<std::string> captions;

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

   static size_func empty_size = []() -> mySizeRet { std::optional<size_t> ret{ }; return ret; };

   static std::vector<std::tuple<std::string, std::string, std::string, call_func, size_func>> test_funcs = {
      { "delete file"s, "delete all output file "s, "output files deleted"s,
            [&directory]() {
               for (fs::path dir{ directory }; auto const& file : { "testausgabe.txt", "testausgabe_alle.txt" }) {
                  fs::remove(dir / file);
                  }
               },
         empty_size },
      { "read file"s, "read file \""s + strFilename + "\""s, "datasets read from file"s,
            std::bind(Reading<double>, std::ref(vData), std::cref(filename)),
            addresses_size 
      },
      // -----------------------------------------------------------------------------
      { "calc point"s, "calculate data"s, "datasets calculated for point"s,
            [&vData, &point]() { CalculateRange(point, vData.begin(), vData.end()); },
            //std::bind(CalculateRange<double>, std::cref(point), vData.begin(), vData.end()),
            addresses_size 
      },
      // -----------------------------------------------------------------------------
      { "sort din"s, "sort data"s, "datasets sorted in vector"s,
            std::bind(Sorting<double>, std::ref(vData)),
            addresses_size 
      },
      // -----------------------------------------------------------------------------
      { "write all"s, "write data to \""s + strOutput_all + "\""s, "datasets wrote to file"s,
            std::bind(Writing<double>, std::cref(vData), std::cref(output_all)),
            addresses_size 
      // -----------------------------------------------------------------------------
      },
      { "delete dir"s, "delete directory "s, "directories deleted"s,
            std::bind(DeleteDirectories, std::cref(directory)),
            empty_size 
      },
      // -----------------------------------------------------------------------------
      { "write dir"s, "write data to directory "s, "datasets wrote to directories"s,
            std::bind(WriteToDirectory<double>, std::cref(directory), std::ref(vData)),
            addresses_size 
      },
      // -----------------------------------------------------------------------------
      { "del data"s, "delete data "s, "datasets still in alive"s,
            [&vData]() { 
                vData.clear(); 
                vData.shrink_to_fit(); 
                },
            addresses_capacity 
      },
      // -----------------------------------------------------------------------------
      { "read dir"s, "read data from directory "s, "datasets read from directories"s,
            std::bind(ReadFromDirectory<double>, std::cref(directory), std::ref(vData)),
            addresses_size 
      },
      // -----------------------------------------------------------------------------
      { "split data"s, "partitioning data to "s, "datasets partitioned in vector"s,
            [&vData, &it]() {
               it = std::partition(std::execution::par, vData.begin(), vData.end(), 
                         [](auto const& val) { return val.second.first < 1000.0; });
               },
            value_size
      },
      // -----------------------------------------------------------------------------
      { "sort part"s, "sort partitioned data "s, "partitioned datasets sorted"s,
            [&vData, &it]() {
                std::sort(std::execution::par, vData.begin(), it, [](auto const& lhs, auto const& rhs) {
                   if (auto cmp = lhs.second.first <=> rhs.second.first; cmp != 0) [[likely]] return cmp > 0;
                   else if (auto cmp = lhs.second.second <=> rhs.second.second; cmp != 0) return cmp < 0;
                   else if (auto cmp = lhs.first.Street() <=> rhs.first.Street(); cmp != 0) return cmp < 0;
                   else return lhs.first.StreetNumber() < rhs.first.StreetNumber();
                   });
                },
            value_size 
      },
      // -----------------------------------------------------------------------------
      { "write part"s, "write this data to \""s + strOutput + "\""s, "datasets wrote to file"s,
            [&vData, &it, &output]() { 
                WritingPart<double>(vData.begin(), it, output); 
                },
            value_size 
            }
      };
     
   TTestData<double> test;
   test.strTest        = "comparisons test"s;
   test.strDescription = "read and write cadastre data with geographic coordinates and geodetic calculations"s;
   for (int i = 0; auto const& test_func : test_funcs) {
      test.captions_short.emplace_back(std::get<0>(test_func));
      test.captions.emplace_back(std::get<1>(test_func));
      }
   test.captions_short.emplace_back("total"s);

   test.loc          = std::locale("de_DE");
   test.strDirectory = directory;
   test.iTestCases   = iCount;
   test.prec         = time_prec;
   test.start_time   = std::chrono::system_clock::now();
   std::string strTimeStamp = get_current_timestamp(test.start_time);
   test.strProtocol  = directory / ("protokoll"s + strTimeStamp + ".csv"s);
   test.strOverview  = directory / ("overview"s + strTimeStamp + ".txt"s);


   WriteStart<double>(std::cout, test);
   std::cout << std::endl;

   
   try {
      std::cout << std::format("Check Input with {} ... ", "D:\\Test_Reference");
      auto func_start = myClock::now();
      Compare_Input(directory, "D:\\Test_Reference");
      auto func_ende = myClock::now();
      auto runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
      std::cout << std::format(" Done in {:.{}f} sec\n\n", runtime.count() / time_factor, time_prec);
      }
   catch(std::exception& ex) {
      std::cout << std::format("Error while checking input files: {}\n", ex.what());
      return;
      }
   

   for(int step : std::ranges::iota_view{ 0, test.iTestCases }) {
      std::cout << std::format("{2:} start run {0:} of {1:}\n", step + 1, iCount, get_current_time_and_date());
      //std::cout << get_current_time_and_date() << " start run " << step + 1 << " of " << iCount << std::endl;
      auto time_for_all = myTimeType::zero();
      std::vector<double> measurement;
      for (auto const& test_func : test_funcs) {
         std::cout << std::format("{} {:<45s}-", get_current_time_and_date(), std::get<1>(test_func));
         try {
            auto func_start = myClock::now();
            std::get<3>(test_func)();
            auto func_ende = myClock::now();
            auto runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
            measurement.emplace_back(runtime.count() / time_factor);
            time_for_all += runtime;
 
            if (auto val = std::get<4>(test_func)(); val) std::cout << std::format("{:>9} ", *val); 
            else  std::cout << "          "; 
     
            std::cout << std::format("{:<30} in {:9.{}f} sec\n", std::get<2>(test_func), runtime.count() / time_factor, time_prec);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
         catch (std::exception& ex) {
            std::cout << "error: " << ex.what() << "\n";
            }
         }
      measurement.emplace_back(time_for_all.count() / time_factor);
      test.measurements.emplace_back(measurement);
      std::cout << std::endl << get_current_time_and_date() << " " << std::left << "time for all operations " 
                << std::right << std::setw(9) << std::setprecision(time_prec) << time_for_all.count() / time_factor << " sec\n";
      std::cout << "\n"s;
      std::cout << std::format("Check with directory {} ...", "D:\\Test_Reference");
      
      try {
         auto func_start = myClock::now();
         Compare_Output(directory, "D:\\Test_Reference");
         auto func_ende = myClock::now();
         auto runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
         std::cout << std::format(" Done in {:.{}f} sec\n\n", runtime.count() / time_factor, time_prec);
         }
      catch(std::exception& ex) {
         std::cout << std::format("Error while checking input files: {}\n", ex.what());
         return;
         }
      
      if(step + 1 < test.iTestCases) {
         static const std::string strWait = "clear all data and wait ... | "s;
         auto Clear = []() {
            for (int s = 0; s < strWait.size(); ++s) {
               std::cout << "\b \b";
               std::this_thread::sleep_for(std::chrono::milliseconds(5));
               }
            };
         std::cout << strWait;
         vData.clear();
         vData.shrink_to_fit();

         for(auto i : std::views::iota(0, 20)) {
            static char wheel[] = { '/',  '-', '\\',  '|' };
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::cout << "\b\b" << wheel[i % 4] << ' ';
            }
         Clear();
         }
      }
   test.end_time = std::chrono::system_clock::now();

   std::ofstream p(test.strProtocol);
   p.imbue(test.loc);
   WriteProtocol<double>(p, test);

   // Auswertung
   p.close();
   p.open(test.strOverview);
   p.imbue(test.loc);
   p.setf(std::ios::fixed);
   p.setf(std::ios::showpoint);
   WriteStatistic<double>(std::cout, test);
   WriteStatistic<double>(p, test);
   std::cout << "\n\nFinished. Good Bye!\n";
   }


int main(int argc, char* argv[]) {
   std::ios_base::sync_with_stdio(false);
   std::cout.setf(std::ios::showpoint);
   std::cout.setf(std::ios::fixed);

   std::string strInput = "D:\\Test"s;
   int iCount = 1;
   if (argc == 2) strInput = argv[1];
   else if(argc == 3) {
      iCount = string_to_int<int>(std::string(argv[1]));
      strInput = argv[2];
      }

   Rechentest(strInput, iCount);

   //Compare_Output("e:\\Test", "D:\\Test_Reference");
   
   //std::thread t(Rechentest, std::cref(strInput));
   //t.join();

   #if defined __BORLANDC__
   std::cout << "... press a key ...";
   getchar();
   #endif
   return 0;
}

