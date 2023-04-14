// Test_Tune.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

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
#include <functional>
#include <optional> // !! C++17
#include <thread>
#include <format>

#include <chrono>
#include <ranges>



int main(int argc, char* argv[]) {
   std::ios_base::sync_with_stdio(false);
   std::cout.setf(std::ios::showpoint);
   std::cout.setf(std::ios::fixed);

   std::string strDirectory = "D:\\Test"s;
   if (argc == 2) strDirectory = argv[1];

   data_vector<double> vData;
   std::string strFilename = strDirectory + "\\berlin_infos.dat"s;
   Reading<double>(vData, strFilename);
   Calculate(Location<double>{ 52.520803, 13.40945 }, vData.begin(), vData.end());
   Sorting<double>(vData);
   std::string strOutput_all = strDirectory + "\\Testausgabe_alle.txt"s;
   Writing<double>(vData, strOutput_all);
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
   std::string strOutput = strDirectory + "\\Testausgabe.txt"s;
   WritingPart<double>(vData.begin(), it, strOutput);

   return 0;
}

