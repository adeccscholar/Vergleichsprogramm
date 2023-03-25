// Berlin_App.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "Processes.h"
#include "Test.h"
#include "sort_din.h"
#include "My_Double.h"
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <functional>
#include <locale>
//#include <format>
#include <codecvt>

//#include <CoreWindow.h>

using namespace std;

bool   boStop = true;
size_t iPara1, iPara2, iPara3;
size_t iLimit;
size_t iLoops;

extern size_t iBufferPages1, iBufferPages2, iBufferPages3;

extern std::string strPath;
std::string strOutput;
extern std::string strFile_data;
extern std::string strFile_data_out;
extern std::string strFile_dist_out;
extern std::string strFile_spots;
extern std::string strFile_spots_out;
extern std::string strFile_points_out;
extern std::string strFile_control;

template <typename func1, typename func2>
void Call(std::string const& text, func1 f1, func2 f2) {
    auto func_start = chrono::high_resolution_clock::now();
   f1();
    auto func_ende = chrono::high_resolution_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(func_ende - func_start).count();
   std::cout << left << setw(55) << text << " Size = " << right << setw(10) << f2() << "    in " << setw(12) << setprecision(3) << time/1000.0 << " ms" << endl;
   /*
   static auto loc = locale(locale("de_DE"), new TMyNum());
   std::cout.imbue(loc);
    format_to(ostream_iterator<char>(std::cout),loc, "{:55s} Size = {:10d}    in {:12.3f} ms\n", text, f2(), time/1000.0);
   */
   // noch Probleme mit thousand separator + length
    //static auto loc = locale(locale("de_DE"), new TMyNum());
    //format_to(ostream_iterator<char>(std::cout), loc, "{:55s} Size = {:10Ld}    in {:12.3Lf} ms\n", text, f2(), time / 1000.0);
}


template <typename func1>
void Call(std::string const& text, func1 f1) {
    auto func_start = chrono::high_resolution_clock::now();
    f1();
    auto func_ende = chrono::high_resolution_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(func_ende - func_start).count();
    std::cout << left << setw(73) << text << "    in " << right << setw(12) << setprecision(3) << time / 1000.0 << " ms" << endl;
   }




void Standard_Test(void) {
    myData vec_data;
   mySpots vec_spots;
   
    //SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
   try {
        // read from file
        auto prog_start = chrono::high_resolution_clock::now();
    Call("sequential file with addresses read ", bind(ReadFile, cref(strFile_data), ref(vec_data)), [&vec_data]() { return vec_data.size(); });

        // calculating
        pair<double, double> point = make_pair(52.520803, 13.40945); // Berlin, television tower
        Call("distance/angle with geodetic coordinates calculated", bind(CalculatingDistance, cref(point), ref(vec_data)));

        // sorting
        Call("address data sorted for criterias", bind(Sorting4Districts, ref(vec_data)));
       Call("address data wrote to sequential file", bind(Write_Streets_parallel, cref(strFile_data_out), iPara1, ref(vec_data)));
       Call("old directories and files deleted from drive", bind(Clean_Directory, cref(strPath)));
       Call("address data wrote to directory structure", bind(WriteTo_Directory, cref(strPath), iPara3, iLimit, ref(vec_data)));
        Call("all dynamic allocated data cleaned", bind(CleanData, ref(vec_data)), [&vec_data]() { return vec_data.capacity(); });
        Call("files read from directories", bind(Read_Directory, cref(strPath), ref(vec_data)), [&vec_data]() { return vec_data.size(); });

      /*
        myData check_Data, diff_Data;
      Read_Directory(strPath, check_Data);
      sort(check_Data.begin(), check_Data.end());
        sort(vec_data.begin(), vec_data.end());
      set_difference(vec_data.begin(), vec_data.end(), check_Data.begin(), check_Data.end(), back_inserter(diff_Data));
        Write_Part_of_Streets(std::cout, diff_Data.begin(), diff_Data.end());
      */

        vector<TData>::iterator it;
        Call("data partitioned", bind(Partion_Distance, ref(vec_data), 1000, ref(it)), [&it, &vec_data]() { return it - vec_data.begin(); });
      Call("part of data sorted", bind(Sorting4Distance, vec_data.begin(), it), [&it, &vec_data]() { return it - vec_data.begin(); });
        Call("part of data wrote in file", bind(Write_Part_of_Streets, cref(strFile_dist_out), iPara2, vec_data.begin(), it));
      
      auto prog_ende = chrono::high_resolution_clock::now();
        auto time = chrono::duration_cast<chrono::microseconds>(prog_ende - prog_start).count();
      std::cout << endl << left << setw(80) << "time over all " << right << setw(12) << setprecision(3) << time / 1000.0 << " ms" << endl;



      std::cout << endl << endl << endl;
     /*
      Call("spots read", bind(ReadSpots, cref(strFile_spots), ref(vec_spots)), [&vec_spots]() { return vec_spots.size(); });
      Call("spots sorted", bind(SortSpots, ref(vec_spots)));
      of.open(strFile_spots_out);
      Call("spots wrote to file", bind(WriteSpots, ref(of), cref(vec_spots)));
      of.close();
      
      std::cout << endl << endl << endl;
     */
     /*
        of.open(strFile_control);
        Call("wrote all data to control file", bind(Write_Part_of_Addresses, ref(of), vec_data.begin(), vec_data.end()));
        of.close();
     */ 
        vecGeoPoints points;
      points.reserve(55);
      ofstream of(strFile_points_out);
        of.imbue(locale(locale("de_DE"), new TMyNum()));
        of.setf(ios::showpoint);
        of.setf(ios::fixed);
        Call("Statistic - global",       bind(BuildStatisticTotal, ref(of), ref(points), ref(vec_data)));
        Call("Statistic - districts",    bind(BuildStatisticUrban_Unit, ref(of), ref(points), ref(vec_data)));
        of.close();

        Call("Add Spots to Statistic",   bind(ReadSpotsAddPoints, cref(strFile_spots), ref(points)), [&points]() { return points.size(); });
        Call("Calculating Spots",        bind(CalculatingSpotsPoints, ref(points), ref(vec_data)));
      Call("Write Spots of Statistic", bind(WriteSpotsFile, cref(strFile_spots_out), cref(points)), [&points]() { return points.size(); });
      
    }
    catch (exception& ex) {
        std::cout << ex.what() << endl;
      }
}

void Read_Test(void) {
    myData vec_data;

    try {
        // read from file
        Call("sequential file linear ",                bind(Read0, cref(strFile_data), ref(vec_data)), [&vec_data]() { return vec_data.size(); });
        Call("all dynamic allocated data cleaned",     bind(CleanData, ref(vec_data)), [&vec_data]() { return vec_data.capacity(); });
        Call("sequential file with function vector ",  bind(Read, cref(strFile_data), ref(vec_data)), [&vec_data]() { return vec_data.size(); });
        Call("all dynamic allocated data cleaned",     bind(CleanData, ref(vec_data)), [&vec_data]() { return vec_data.capacity(); });
        Call("sequential file with RValue + emplace ", bind(Read1_templ, cref(strFile_data), ref(vec_data)), [&vec_data]() { return vec_data.size(); });
        Call("all dynamic allocated data cleaned",     bind(CleanData, ref(vec_data)), [&vec_data]() { return vec_data.capacity(); });
        Call("sequential file with string_view ",      bind(Read2, cref(strFile_data), ref(vec_data)), [&vec_data]() { return vec_data.size(); });
        Call("all dynamic allocated data cleaned",     bind(CleanData, ref(vec_data)), [&vec_data]() { return vec_data.capacity(); });
        Call("sequential file parallel iterator ",     bind(Read3, cref(strFile_data), ref(vec_data)), [&vec_data]() { return vec_data.size(); });
        Call("all dynamic allocated data cleaned",     bind(CleanData, ref(vec_data)), [&vec_data]() { return vec_data.capacity(); });
        Call("sequential file parallel optimized",     bind(ReadFile, cref(strFile_data), ref(vec_data)), [&vec_data]() { return vec_data.size(); });
        Call("all dynamic allocated data cleaned",     bind(CleanData, ref(vec_data)), [&vec_data]() { return vec_data.capacity(); });
    }
    catch (exception& ex) {
        std::cout << ex.what() << endl;
    }
}

void Quick_Test(void) {
    myData vec_data;
    try {
        auto prog_start = chrono::high_resolution_clock::now();
        Call("sequential file with addresses read ", bind(ReadFile, cref(strFile_data), ref(vec_data)), [&vec_data]() { return vec_data.size(); });
        pair<double, double> point = make_pair(52.520803, 13.40945); // Berlin, television tower
        Call("distance/angle with geodetic coordinates calculated", bind(CalculatingDistance, cref(point), ref(vec_data)));
        Call("address data sorted for criterias", bind(Sorting4Districts, ref(vec_data)));
        Call("address data wrote to sequential file", bind(Write_Streets_parallel, cref(strFile_data_out), iPara1, ref(vec_data)));
        Call("old directories and files deleted from drive", bind(Clean_Directory, cref(strPath)));
        Call("address data wrote to directory structure", bind(WriteTo_Directory, cref(strPath), iPara3, iLimit, ref(vec_data)));
        Call("all dynamic allocated data cleaned", bind(CleanData, ref(vec_data)), [&vec_data]() { return vec_data.capacity(); });
        Call("files read from directories", bind(Read_Directory, cref(strPath), ref(vec_data)), [&vec_data]() { return vec_data.size(); });
        vector<TData>::iterator it;
        Call("data partitioned", bind(Partion_Distance, ref(vec_data), 1000, ref(it)), [&it, &vec_data]() { return it - vec_data.begin(); });
        Call("part of data sorted", bind(Sorting4Distance, vec_data.begin(), it), [&it, &vec_data]() { return it - vec_data.begin(); });
        Call("part of data wrote in file", bind(Write_Part_of_Streets, cref(strFile_dist_out), iPara2, vec_data.begin(), it));

        auto prog_ende = chrono::high_resolution_clock::now();
        auto time = chrono::duration_cast<chrono::microseconds>(prog_ende - prog_start).count();
        std::cout << endl << left << setw(80) << "time over all " << right << setw(12) << setprecision(3) << time / 1000.0 << " ms" << endl;
    }
    catch (exception& ex) {
        std::cout << ex.what() << endl;
    }
}

function<void (void)> test_function;
map<string, pair<function<void(void)>, std::string>> tests = { {"quick"s,    { Quick_Test,      "quick performance test, only 1st stage, processing address data"s } },
                                                               {"read"s,     { Read_Test ,    "read test, compare different variants for read the sequential file"s } },
                                                               {"standard"s, { Standard_Test, "performance test, perform all tests to processing address data"s } } };

void SetTest(std::string const& strTest) {
   if(auto t = tests.find(strTest); t != tests.end()) {
      test_function = t->second.first;
      std::cout << t->second.second << endl;
      }
    else {
        std::cout << "Fehler im Programmaufruf: unexpected test " << strTest << endl
                << "  possible tests are" << endl;
      for(auto [mod, val] : tests) std::cout << "   - " << mod << endl;
      exit(4);
       }
   }

// iBufferPages1, iBufferPages2;
using pairParam = pair <function<void(string const&)>, string>;
map<string, pairParam> funcs_parameter = {
       { "-f"s,  { [](string const& val) { strPath = val; },             "data_path"s } },
       { "-o"s,  { [](string const& val) { strOutput = val; },           "output file (default is stdout)"s } },
       { "-l"s,  { [](string const& val) { iLoops = stoi(val); },        "loops (default = 1)"s } },
       { "-p1"s, { [](string const& val) { iPara1 = stoi(val); },        "para1 (positive number with parallel for seq file)s" } },
       { "-b1"s, { [](string const& val) { iBufferPages1 = stoi(val); }, "BufferPages1 (positive number with pages of 4'096 for seq file)"s  }},
       { "-p2"s, { [](string const& val) { iPara2 = stoi(val); },        "para2 (positive number with parallel for parts)"s } },
       { "-b2"s, { [](string const& val) { iBufferPages2 = stoi(val); }, "BufferPages2 (positive number with pages of 4'096 for parts)"s  }},
       { "-p3"s, { [](string const& val) { iPara3 = stoi(val); },        "para2 (positive number with parallel for parts)"s } },
       { "-b3"s, { [](string const& val) { iBufferPages3 = stoi(val); }, "BufferPages3 (positive number with pages of 4'096 for parts)"s  }},
       { "-l3"s, { [](string const& val) { iLimit = stoi(val); },        "Limit before Dirs wite parallel (positive number with streets before parallel)"s  }},
       { "-t"s,  { SetTest,                                              "test"s } },
       { "-m"s,  { [](string const& val) { 
                       if(val == "stop") boStop = true; 
                       else boStop = false; 
                       },                                                "text stop, wait at the end"s } }
   };

std::string get_current_time_and_date() {
   auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
   return std::format("{:%Y-%m-%d %X}", time);
   }

int main(int argc, char* argv[]) {

   //auto test1 = my_Double_to_String(1234567.34, 3);     // 2 - 1   1.234.567,340
   //auto test2 = my_Double_to_String(123456789.34, 3);   // 3 - 0   123.456.789,340
   //cout << test1 << endl;
   //return 0;
   std::ios_base::sync_with_stdio(false);
   std::cout.imbue(locale(locale("de_DE"), new TMyNum()));
   std::cout.setf(ios::showpoint);
   std::cout.setf(ios::fixed);

   test_function = nullptr;
   iLoops = 1;
   iPara1 = 5;
   iBufferPages1 = 10;
   iPara2 = 3;
   iBufferPages2 = 10;
   iPara3 = 3;
   iBufferPages3 = 10;
   iLimit = 40;
   strPath = "d:\\Test"s;
   strOutput = ""s;
  
   if(argc > 1) {
      if (argc == 2) {
         std::cout << "Programmaufruf: " << argv[0] << endl;
         for (auto val : funcs_parameter) { std::cout << "  " << val.first << " " << val.second.second << endl; }
         std::cout << "  possible tests" << endl;
         for(auto val : tests) std::cout << "    " << setw(10) << val.first << " " << val.second.second << endl;
         return 1;
         }

      if (argc > 2 && argc % 2 != 0) {
         for (int i = 1; i < argc; i += 2) {
            if (auto it = funcs_parameter.find(argv[i]); it != funcs_parameter.end()) {
               it->second.first(argv[i+1]);
               }
            else {
               std::cout << "Fehler im Programmaufruf: " << argv[0] << endl
                         << "Parameter " << argv[i] << "is unexpected." << endl;
               for(auto val : funcs_parameter) { std::cout << "  " << val.first << " " << val.second.second << endl; }
               std::cout << "  possible tests" << endl;
               for (auto val : tests) std::cout << "    " << setw(10) << val.first << " " << val.second.second << endl;
               return 2;
               }
            }
         }
      else {
         std::cout << "Fehler im Programmaufruf: " << argv[0] << endl;
         for (auto val : funcs_parameter) { std::cout << "  " << val.first << " " << val.second.second << endl; }
         std::cout << "  possible tests" << endl;
         for (auto val : tests) std::cout << "    " << setw(10) << val.first << " " << val.second.second << endl;
         return 3;
         }
      }

   if(!test_function) SetTest("quick");
   std::cout << "Parameter:" << endl
             << "L  = " << iLoops << " (Loops for the test)" << endl
             << "P1 = " << iPara1 << " (Count of parallel parts to write serial file)" << endl
             << "B1 = " << iBufferPages1 << " (Pages are 4.096 Byte for serial write)" << endl
             << "P2 = " << iPara2 << " (Count of parallel parts to write directory files)" << endl
             << "B2 = " << iBufferPages2 << " (Pages are 4.096 Byte for directory write)" << endl
             << "Path = " << strPath << endl
             << "Modus Stop = " << boolalpha << boStop << endl << endl;

   Prepare(strPath);

   //Helper(strPath);
   //TestCalculate(strPath);

   //strOutput cout verbiegen
   std::streambuf* old_cout = nullptr;
   std::ofstream of;
   if (strOutput.length() > 1) {
      of.open(strOutput);
      if (!of) {
         std::cout << "Output file " << strOutput << " can't open" << std::endl;
         }
      else {
         old_cout = std::cout.rdbuf(of.rdbuf());
         }
      }

   std::cout << "Started: " << get_current_time_and_date() << std::endl;
   for (size_t iCntTest = 0; iCntTest < iLoops; ++iCntTest) {
      test_function();
      std::cout << std::endl;
      }
   std::cout << "Finished: " << get_current_time_and_date() << std::endl;

   if (old_cout) std::cout.rdbuf(old_cout);

   if(boStop) {
      std::cout << endl << "program finished successful, please press a key ... " << endl;
      getchar();
      }
   return 0;
   }

/*

sequential file with addresses read                     Size =    384.860    in      506,915 ms
distance/angle with geodetic coordinates calculated                          in       15,007 ms
address data sorted for criterias                                            in      411,379 ms
address data wrote to sequential file                                        in      927,569 ms
old directories and files deleted from drive                                 in      248,133 ms
address data wrote to directory structure                                    in    1.799,844 ms
all dynamic allocated data cleaned                      Size =          0    in       29,749 ms
files read from directories                             Size =    384.860    in    1.091,884 ms
data partitioned                                        Size =      1.653    in        1,302 ms
part of data sorted                                     Size =      1.653    in        0,159 ms
part of data wrote in file                                                   in        4,187 ms

time over all                                                                      5.048,417 ms

sequential file with addresses read                     Size =    384.860    in      431,986 ms
distance/angle with geodetic coordinates calculated                          in       12,548 ms
address data sorted for criterias                                            in      412,816 ms
address data wrote to sequential file                                        in      914,755 ms
old directories and files deleted from drive                                 in      273,464 ms
address data wrote to directory structure                                    in    1.711,379 ms
all dynamic allocated data cleaned                      Size =          0    in       28,393 ms
files read from directories                             Size =    384.860    in      950,046 ms
data partitioned                                        Size =      1.653    in        0,924 ms
part of data sorted                                     Size =      1.653    in        0,191 ms
part of data wrote in file                                                   in        3,920 ms

time over all                                                                      4.757,168 ms

sequential file with addresses read                     Size =    384.860    in      432,893 ms
distance/angle with geodetic coordinates calculated                          in       11,242 ms
address data sorted for criterias                                            in      404,963 ms
address data wrote to sequential file                                        in      999,299 ms
old directories and files deleted from drive                                 in      243,034 ms
address data wrote to directory structure                                    in    1.668,764 ms
all dynamic allocated data cleaned                      Size =          0    in       27,287 ms
files read from directories                             Size =    384.860    in      926,521 ms
data partitioned                                        Size =      1.653    in        1,036 ms
part of data sorted                                     Size =      1.653    in        0,143 ms
part of data wrote in file                                                   in        3,764 ms

time over all                                                                      4.736,565 ms

sequential file with addresses read                     Size =    384.860    in      414,350 ms
distance/angle with geodetic coordinates calculated                          in       10,023 ms
address data sorted for criterias                                            in      375,915 ms
address data wrote to sequential file                                        in      982,624 ms
old directories and files deleted from drive                                 in      237,815 ms
address data wrote to directory structure                                    in    1.620,420 ms
all dynamic allocated data cleaned                      Size =          0    in       26,485 ms
files read from directories                             Size =    384.860    in      914,951 ms
data partitioned                                        Size =      1.653    in        0,958 ms
part of data sorted                                     Size =      1.653    in        0,161 ms
part of data wrote in file                                                   in        3,975 ms

time over all                                                                      4.604,485 ms

sequential file with addresses read                     Size =    384.860    in      280,879 ms
distance/angle with geodetic coordinates calculated                          in        4,005 ms
address data sorted for criterias                                            in      291,072 ms
address data wrote to sequential file                                        in      810,554 ms
old directories and files deleted from drive                                 in      189,185 ms
address data wrote to directory structure                                    in    1.310,429 ms
all dynamic allocated data cleaned                      Size =          0    in       23,040 ms
files read from directories                             Size =    384.860    in      273,862 ms
data partitioned                                        Size =      1.653    in        1,027 ms
part of data sorted                                     Size =      1.653    in        0,136 ms
part of data wrote in file                                                   in        4,035 ms

time over all                                                                      3.196,129 ms

sequential file with addresses read                     Size =    384.860    in      267,743 ms
distance/angle with geodetic coordinates calculated                          in        3,964 ms
address data sorted for criterias                                            in      297,608 ms
address data wrote to sequential file                                        in      823,761 ms
old directories and files deleted from drive                                 in      191,271 ms
address data wrote to directory structure                                    in    1.242,995 ms
all dynamic allocated data cleaned                      Size =          0    in       23,928 ms
files read from directories                             Size =    384.860    in      223,934 ms
data partitioned                                        Size =      1.653    in        0,977 ms
part of data sorted                                     Size =      1.653    in        0,150 ms
part of data wrote in file                                                   in        4,319 ms

time over all                                                                      3.088,880 ms

sequential file with addresses read                     Size =    384.860    in      268,527 ms
distance/angle with geodetic coordinates calculated                          in        3,388 ms
address data sorted for criterias                                            in      291,068 ms
address data wrote to sequential file                                        in      797,275 ms
old directories and files deleted from drive                                 in      186,688 ms
address data wrote to directory structure                                    in    1.267,163 ms
all dynamic allocated data cleaned                      Size =          0    in       23,102 ms
files read from directories                             Size =    384.860    in      207,267 ms
data partitioned                                        Size =      1.653    in        0,958 ms
part of data sorted                                     Size =      1.653    in        0,109 ms
part of data wrote in file                                                   in        4,098 ms

time over all                                                                      3.058,351 ms

sequential file with addresses read                     Size =    384.860    in      282,988 ms
distance/angle with geodetic coordinates calculated                          in        3,700 ms
address data sorted for criterias                                            in      294,225 ms
address data wrote to sequential file                                        in      590,693 ms
old directories and files deleted from drive                                 in      193,549 ms
address data wrote to directory structure                                    in    1.238,569 ms
all dynamic allocated data cleaned                      Size =          0    in       23,141 ms
files read from directories                             Size =    384.860    in      211,427 ms
data partitioned                                        Size =      1.653    in        0,949 ms
part of data sorted                                     Size =      1.653    in        0,116 ms
part of data wrote in file                                                   in        4,096 ms

time over all                                                                      2.851,283 ms

sequential file with addresses read                     Size =    384.860    in      264,774 ms
distance/angle with geodetic coordinates calculated                          in        4,489 ms
address data sorted for criterias                                            in      289,979 ms
address data wrote to sequential file                                        in      497,464 ms
old directories and files deleted from drive                                 in      183,375 ms
address data wrote to directory structure                                    in    1.148,060 ms
all dynamic allocated data cleaned                      Size =          0    in       23,358 ms
files read from directories                             Size =    384.860    in      208,167 ms
data partitioned                                        Size =      1.653    in        0,978 ms
part of data sorted                                     Size =      1.653    in        0,110 ms
part of data wrote in file                                                   in        2,815 ms

time over all                                                                      2.631,223 ms

sequential file with addresses read                     Size =    384.860    in      264,137 ms
distance/angle with geodetic coordinates calculated                          in        4,545 ms
address data sorted for criterias                                            in      284,405 ms
address data wrote to sequential file                                        in      516,647 ms
old directories and files deleted from drive                                 in      184,867 ms
address data wrote to directory structure                                    in    1.104,918 ms
all dynamic allocated data cleaned                      Size =          0    in       23,227 ms
files read from directories                             Size =    384.860    in      168,297 ms
data partitioned                                        Size =      1.653    in        0,961 ms
part of data sorted                                     Size =      1.653    in        0,111 ms
part of data wrote in file                                                   in        2,538 ms

time over all                                                                      2.562,818 ms

sequential file with addresses read                     Size =    384.860    in      275,121 ms
distance/angle with geodetic coordinates calculated                          in        3,987 ms
address data sorted for criterias                                            in      292,633 ms
address data wrote to sequential file                                        in      192,551 ms
old directories and files deleted from drive                                 in      194,312 ms
address data wrote to directory structure                                    in    1.105,292 ms
all dynamic allocated data cleaned                      Size =          0    in       23,818 ms
files read from directories                             Size =    384.860    in      168,609 ms
data partitioned                                        Size =      1.653    in        1,058 ms
part of data sorted                                     Size =      1.653    in        0,124 ms
part of data wrote in file                                                   in        1,296 ms

time over all                                                                      2.268,099 ms
*/