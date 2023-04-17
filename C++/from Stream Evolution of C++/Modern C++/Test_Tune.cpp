// Test_Tune.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include "MyProcess.h"

#include <string>
#include <algorithm>

int main(int argc, char* argv[]) {

   std::string strDirectory = "D:\\Test"s;
   if (argc == 2) strDirectory = argv[1];

   data_vector<double> vData;
   Reading<double>(vData, strDirectory + "\\berlin_infos.dat"s);
   Calculate(Location<double>{ 52.520803, 13.40945 }, vData.begin(), vData.end());
   Sorting<double>(vData);
   Writing<double>(vData, strDirectory + "\\Testausgabe_alle.txt"s);
   DeleteDirectories(strDirectory);
   WriteToDirectory<double>(strDirectory, vData);
   vData.clear(); 
   vData.shrink_to_fit();
   ReadFromDirectory<double>(strDirectory, vData);
   auto it = std::partition(std::execution::par, vData.begin(), vData.end(), [](auto const& val) {
                                 return val.second.first < 1000.0; 
                                 });
   std::sort(std::execution::par, vData.begin(), it, [](auto const& lhs, auto const& rhs) {
                                 if (lhs.second.first != rhs.second.first) return lhs.second.first > rhs.second.first;
                                 else return lhs.second.second < rhs.second.second; 
                                 });
   WritingPart<double>(vData.begin(), it, strDirectory + "\\Testausgabe.txt"s);

   return 0;
}

