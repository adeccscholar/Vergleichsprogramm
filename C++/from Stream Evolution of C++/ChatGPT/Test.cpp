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

#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <string>
#include <vector>
#include <fstream>
#include <optional>

#include <filesystem>
namespace fs = std::filesystem;



#define PI 3.14159265358979323846

// Funktion zur Konvertierung von Grad in Radian
double deg2rad(double deg) {
   return (deg * PI / 180);
}
double rad2deg(double rad) {
   return (rad * 180 / PI);
}

// Funktion zur Berechnung des Abstands zwischen zwei Positionen in Metern
double distance(Location pos1, Location pos2) {
   double r = 6371e3; // Erdradius in Metern
   double lat1 = deg2rad(pos1.Latitude()); // Umwandlung in Radiant
   double lat2 = deg2rad(pos2.Latitude());
   double dLat = deg2rad(pos2.Latitude() - pos1.Latitude());
   double dLon = deg2rad(pos2.Longitude() - pos1.Longitude());

   double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
   double c = 2 * atan2(sqrt(a), sqrt(1 - a));
   double d = r * c;
   return d;
   }

// Funktion zur Berechnung des Kurswinkels von Position 1 zu Position 2
double bearing(Location pos1, Location pos2) {
   double lat1 = deg2rad(pos1.Latitude()); // Umwandlung in Radiant
   double lat2 = deg2rad(pos2.Latitude());
   double dLon = deg2rad(pos2.Longitude() - pos1.Longitude());

   double y = sin(dLon) * cos(lat2);
   double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);

   double bearing = rad2deg(atan2(y, x));

   // Korrektur des Winkels auf den Bereich von 0 bis 360 Grad
   bearing = fmod(bearing + 360.0, 360.0);

   return bearing;
}


int pushMatchingToFront(data_vector& addresses, double matchValue) {
   int i = 0, j = static_cast<int>(addresses.size()) - 1;
   while (i < j) {
      if (addresses[i].second.first < matchValue) {
         i++;
      }
      else if (addresses[j].second.first >= matchValue) {
         j--;
      }
      else {
         swap(addresses[i], addresses[j]);
      }
   }
   return i; // (v[i] == matchValue) ? i + 1 : i;
}

void sortAddressesInRange(data_vector& addresses, int endIndex) {
   sort(addresses.begin(), addresses.begin() + endIndex, [](const auto& a1, const auto& a2) {
      if (a1.second.first != a2.second.first) {
         return a1.second.first > a2.second.first; // Sortiere absteigend nach Entfernung
      }
      else {
         return a1.second.second < a2.second.second; // Sortiere aufsteigend nach Kurswinkel
         }
      });
   }

auto BerechneChatGPT(data_vector& addresses, Location const& point) {
   for (auto& [address, result] : addresses) {
      double dist = distance( point, { address.Latitude(), address.Longitude() });
      result.first = dist;
      double angle = bearing( { address.Latitude(), address.Longitude() }, point);
      result.second = fmod(360.0 - angle, 360.0); // Kurswinkel als Komplement des Azimuts
   }
}

void deleteDirectory(const std::string& path) {
   if (std::filesystem::exists(path)) {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
         if (entry.is_directory()) {
            deleteDirectory(entry.path().string());
         }
         else {
            std::filesystem::remove(entry.path());
         }
      }
      std::filesystem::remove(path);
   }
}

void writeAddressesToDirectories(const std::string& root_dir, const data_vector& addresses) {
   // create a root directory for all cities
   std::filesystem::create_directory(root_dir);

   for (const auto& [address, result] : addresses) {
      std::string dir = root_dir + "\\" + address.City() + "\\" + address.UrbanUnit() + "\\" + address.District();
      std::filesystem::create_directories(dir);  // Koorigiert, da alle Verzeichnisse erstellt werden müssen

      // open file with name of street
      std::ofstream file(dir + "\\" + address.Street() + ".csv", std::ios::app);
      file.setf(std::ios::fixed);
      file.setf(std::ios::showpoint);

      // write address data to file
      file << address.StreetNumber() << ";" << address.ZipCode() << ";" << address.UrbanUnit_Old() << ";"
           << std::setprecision(9) << address.Longitude() << ";" << std::setprecision(9) << address.Latitude()
           << ";" << std::setprecision(3) << result.first << ";" << std::setprecision(3) << result.second
           << std::endl;
      file.close();
      }
   }


void write_addresses_to_directory_sorted(const std::string& root_dir, data_vector& addresses)
{
   // Sort addresses by city, district, district part and street
   std::sort(addresses.begin(), addresses.end(), [](const auto& a, const auto& b) {
         if (a.first.City() != b.first.City()) {
            return a.first.City() < b.first.City();
         }
         if (a.first.UrbanUnit() != b.first.UrbanUnit()) {
            return a.first.UrbanUnit() < b.first.UrbanUnit();
         }
         if (a.first.District() != b.first.District()) {
            return a.first.District() < b.first.District();
         }
         if (a.first.Street() != b.first.Street()) {
            return a.first.Street() < b.first.Street();
         }
         std::string::size_type szA, szB;
         int iAN = std::stoi(a.first.StreetNumber(), &szA), iBN = std::stoi(b.first.StreetNumber(), &szB);
         if(iAN != iBN) {
            return iAN < iBN;
            }
         if(a.first.StreetNumber() != b.first.StreetNumber()) {
            return a.first.StreetNumber() < b.first.StreetNumber();
            }
         return a.first.ZipCode() < b.first.ZipCode();
      });

   // Iterate over sorted addresses and write to files
   std::ofstream file;
   std::string prev_city = "", prev_unit = "", prev_district = "", prev_street = "";
   for (const auto& [address, result] : addresses) {
      // Check if we need to open a new file
      if (address.City() != prev_city || address.UrbanUnit() != prev_unit || 
         address.District() != prev_district || address.Street() != prev_street) {
         // Close previous file
         if (file.is_open()) {
            file.close();
         }

         // Create directories if they don't exist
         std::string dir = root_dir + "\\" + address.City() + "\\" + address.UrbanUnit() + "\\" + address.District();
         std::filesystem::create_directories(dir);

         // Open new file
         std::string filename = dir + "\\" + address.Street() + ".csv";
         file.open(filename);
         if (!file.is_open()) {
            std::cerr << "Error: Failed to open file " << filename << std::endl;
            return;
         }
         file.setf(std::ios::fixed);
         file.setf(std::ios::showpoint);
      }

      // Write address data to file
      file << address.StreetNumber() << ";" << address.ZipCode() << ";" << address.UrbanUnit_Old() << ";"
         << std::setprecision(9) << address.Longitude() << ";" << std::setprecision(9) << address.Latitude()
         << ";" << std::setprecision(3) << result.first << ";" << std::setprecision(3) << result.second
         << std::endl;

      // Update previous values
      prev_city     = address.City();
      prev_unit     = address.UrbanUnit();
      prev_district = address.District();
      prev_street   = address.Street();
   }

   // Close last file
   if (file.is_open()) {
      file.close();
   }
}


void readAddressesFromDirectory(std::string directoryPath, data_vector& addresses) {
  // addresses.clear();
  // addresses.shrink_to_fit();

   //data_vector addresses;

   for (const auto& cityDir : std::filesystem::directory_iterator(directoryPath)) {
      if (!std::filesystem::is_directory(cityDir)) {
         continue;
      }
      std::string cityName = cityDir.path().filename().string();

      for (const auto& unitDir : std::filesystem::directory_iterator(cityDir.path())) {
         if (!std::filesystem::is_directory(unitDir)) {
            continue;
         }
         std::string unitName = unitDir.path().filename().string();

         for (const auto& districtDir : std::filesystem::directory_iterator(unitDir.path())) {
            if (!std::filesystem::is_directory(districtDir)) {
               continue;
            }
            std::string districtName = districtDir.path().filename().string();

            for (const auto& file : std::filesystem::directory_iterator(districtDir.path())) {
               if (!std::filesystem::is_regular_file(file)) {
                  continue;
               }
               std::string filename = file.path().filename().string();
               std::string streetName = filename.substr(0, filename.find(".csv"));

               std::ifstream infile(file.path());
               std::string line;
               while (std::getline(infile, line)) {
                  std::stringstream ss(line);
                  std::string data;
                  TData address;
                  Result result;
                  address.City(cityName);
                  address.UrbanUnit(unitName);
                  address.District(districtName);
                  address.Street(streetName);
                  int count = 0;
                  while (std::getline(ss, data, ';')) {
                     count++;
                     if (count == 1) {
                        address.StreetNumber(data);
                     }
                     else if(count == 2) {
                        address.ZipCode(data);
                     }
                     else if(count == 3) {
                        address.UrbanUnit_Old(data);
                     }
                     else if (count == 4) {
                        double flTmp;
                        std::istringstream(data) >> flTmp;
                        address.Latitude(flTmp);
                     }
                     else if (count == 5) {
                        double flTmp;
                        std::istringstream(data) >> flTmp;
                        address.Longitude(flTmp);
                     }
                     else if (count == 6) {
                        std::istringstream(data) >> result.first;
                     }
                     else if (count == 7) {
                        std::istringstream(data) >> result.second;
                     }
                  }
                  addresses.push_back( { address, result } );
               }
            }
         }
      }
   }
  // return addresses;
}

void SortChatGPT_DIN5007(data_vector& vData) {
   std::sort(vData.begin(), vData.end(), [](auto const& lhs, auto const& rhs) { return lhs.first.CompareDIN5007(rhs.first);  });
   }

void ReadChatGPT(data_vector& addresses, std::string const& strFilename) {
   addresses.clear();
   std::ifstream infile(strFilename);  // Datei öffnen
   if (!infile) {
      // geändert in exception anstatt veraltetem Weg über ganzzahlige Rückgabewerte
      throw std::runtime_error("Die Datei konnte nicht geöffnet werden.");
      }
   std::string line;
   while (getline(infile, line)) {  // Zeilenweise Daten einlesen
      TData address;

      // Daten aus der Zeile auslesen
      size_t pos = 0;
      std::string token;
      int field_count = 0;
      while ((pos = line.find(';')) != std::string::npos) {
         token = line.substr(0, pos);
         line.erase(0, pos + 1);

         switch (field_count) {  // Daten in die Datenstruktur schreiben
         case 0:
            address.City(token);
            break;
         case 1:
            address.Street(token);
            break;
         case 2:
            address.StreetNumber(token);
            break;
         case 3:
            address.ZipCode(token);
            break;
         case 4:
            address.UrbanUnit(token);
            break;
         case 5:
            address.UrbanUnit_Old(token);
            break;
         case 6:
            address.District(token);
            break;
         case 7:
            address.Latitude(stod(token));
            break;
         case 8:
            address.Longitude(stod(token));
            break;
         default:
            break;
         }
         field_count++;
      }

      // Letztes Feld auslesen und in die Datenstruktur schreiben
      if (!line.empty()) {
         switch (field_count) {
         case 0:
            address.City(line);
            break;
         case 1:
            address.Street(line);
            break;
         case 2:
            address.StreetNumber(line);
            break;
         case 3:
            address.ZipCode(line);
            break;
         case 4:
            address.UrbanUnit(line);
            break;
         case 5:
            address.UrbanUnit_Old(line);
            break;
         case 6:
            address.District(line);
            break;
         case 7:
            address.Latitude(stod(line));
            break;
         case 8:
            address.Longitude(stod(line));
            break;
         default:
            break;
         }
      }

      addresses.push_back({ address, { 0., 0.} });  // Datenstruktur dem Vektor hinzufügen
   }

   infile.close();  // Datei schließen
}


void WriteChatGPT(data_vector const& addresses, std::string const& strFilename, int count = -1) {
   // Datei für die Ausgabe öffnen
   std::ofstream outfile(strFilename);
   if (!outfile) {
      // geändert in exception anstatt veraltetem Weg über ganzzahlige Rückgabewerte
      throw std::runtime_error("Die Datei konnte nicht geöffnet werden.");
      }

   // Adressen in die Ausgabedatei schreiben
   std::locale german(std::locale("de_DE"));
   int i = 0;
   for (const auto& [address, distance] : addresses) {
      // Zeile im gewünschten Format zusammenstellen
      if (count > 0 && !(i < count)) break;
      std::ostringstream line_stream;
      line_stream.imbue(german);
      line_stream << address.ZipCode() << " " << address.City() << " / " << address.UrbanUnit() << ", " 
                  << address.Street() << " " << address.StreetNumber() 
                  << " -> (" << std::fixed << std::setprecision(9) << address.Latitude()
                  << ", " << std::fixed << std::setprecision(9) << address.Longitude() << ")"
                  << " -> " << std::fixed << std::setprecision(3) << distance.first << "m"
                  << " in " << std::fixed << std::setprecision(1) << distance.second << "°\n";
      std::string line = line_stream.str();
      // Zeile in die Datei schreiben
      outfile << line;
      i++;
      }
   outfile.close();  // Datei schließen
   }



void Rechentest(std::string const& strFilename) {
   using myTimeType = std::chrono::microseconds; // std::chrono::milliseconds
   static const double time_factor = 1000000.;
   static const int    time_prec = 6;
 
   data_vector vData;
   int value;
   Location point = { 52.520803, 13.40945 };
   std::string strOutput_all = "D:\\Test\\Testausgabe_alle.txt";
   std::string strOutput     = "D:\\Test\\Testausgabe.txt";
   std::string strDirectory  = "D:\\Test\\ChatGPT";

   std::vector<std::tuple<std::string, std::string, std::function<void()>, std::function<std::optional<size_t> ()>>> test_funcs = {
      { "read file " + strFilename, "datasets readed", 
            std::bind(ReadChatGPT, std::ref(vData), std::cref(strFilename)), 
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "sort data", "datasets sorted",
            std::bind(SortChatGPT_DIN5007, std::ref(vData)),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "calculate data", "datasets calculated",
            std::bind(BerechneChatGPT, std::ref(vData), std::cref(point)),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "write data to " + strOutput_all, "datasets wrote",
            std::bind(WriteChatGPT, std::ref(vData), std::cref(strOutput_all), -1),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "delete directory " + strDirectory, "directories deleted",
            std::bind(deleteDirectory, std::cref(strDirectory)),
            []() { std::optional<size_t> retval = { };  return retval; } },
      { "write data to directory " + strDirectory, "datasets wrote",
            //std::bind(write_addresses_to_directory_sorted, std::cref(strDirectory), std::ref(vData)),
            std::bind(writeAddressesToDirectories, std::cref(strDirectory), std::cref(vData)),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "delete data ", "datasets still in alive",
            [&vData]() { vData.clear(); vData.shrink_to_fit(); },
            [&vData]() { return std::optional<size_t>(vData.capacity()); } },
      { "read data from directory " + strDirectory, "datasets read",
            std::bind(readAddressesFromDirectory, std::cref(strDirectory), std::ref(vData)),
            [&vData]() { return std::optional<size_t>(vData.size()); } },
      { "partitioning data to ", "datasets partitioned",
            [&vData, &value]() { value = pushMatchingToFront(vData, 1000.0);  },
            [&value]() { return std::optional<size_t>(value); } },
      { "read data to " + strDirectory, "datasets sorted",
            std::bind(sortAddressesInRange, std::ref(vData), value),
            [&value]() { return std::optional<size_t>(value); } },
      { "write data to " + strOutput, "datasets sorted",
            std::bind(WriteChatGPT, std::cref(vData), std::cref(strOutput), value),
            [&value]() { return std::optional<size_t>(value); } }
   };
   ///*
   myTimeType time_for_all = myTimeType::zero();
   for(auto const& test_func : test_funcs) {
      std::cout << std::left << std::setw(50) << std::get<0>(test_func) << " ... ";
      try {
         auto func_start = std::chrono::high_resolution_clock::now();
         std::get<2>(test_func)();
         auto func_ende = std::chrono::high_resolution_clock::now();
         auto runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
         time_for_all += runtime;
         if(std::get<3>(test_func)()) { std::cout << std::right << std::setw(10) << *std::get<3>(test_func)() << " "; }
         else { std::cout << "           "; }
         std::cout << std::left << std::setw(25) << std::get<1>(test_func) << " in " << std::right
                   << std::setw(10) << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";
         }
      catch(std::exception& ex) {
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

