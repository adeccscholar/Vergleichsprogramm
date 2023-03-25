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

#define PI 3.14159265358979323846
#define R 6371000 // Erdradius in Metern


// Funktion zur Konvertierung von Grad in Radian
double deg2rad(double deg) {
   return (deg * PI / 180);
}
double rad2deg(double rad) {
   return (rad * 180 / PI);
}

// Funktion zur Berechnung des Abstands zwischen zwei Positionen in Metern
double distance_1(Location<double> pos1, Location<double> pos2) {
   double dlon = deg2rad(pos2.Longitude() - pos1.Longitude());
   double dlat = deg2rad(pos2.Latitude() - pos1.Latitude());

   double a = pow(sin(dlat / 2), 2) + cos(deg2rad(pos1.Latitude())) * cos(deg2rad(pos2.Latitude())) * pow(sin(dlon / 2), 2);
   double c = 2 * atan2(sqrt(a), sqrt(1 - a));
   double distance = R * c;
   return distance;
   }

double distance(Location<double> pos1, Location<double> pos2) {
   double r = 6371e3; // Erdradius in Metern
   double lat1 = pos1.Latitude() * PI / 180.0; // Umwandlung in Radiant
   double lat2 = pos2.Latitude() * PI / 180.0;
   double dLat = (pos2.Latitude() - pos1.Latitude()) * PI / 180.0;
   double dLon = (pos2.Longitude() - pos1.Longitude()) * PI / 180.0;

   double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
   double c = 2 * atan2(sqrt(a), sqrt(1 - a));
   double d = r * c;
   return d;
   }

// Funktion zur Berechnung des Kurswinkels von Position 1 zu Position 2
double bearing_1(Location<double> pos1, Location<double> pos2) {
   double dlon = deg2rad(pos2.Longitude() - pos1.Longitude());
   double lat1 = deg2rad(pos1.Latitude());
   double lat2 = deg2rad(pos2.Latitude());

   double y = sin(dlon) * cos(lat2);
   double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dlon);
   double bearing = atan2(y, x);
   return rad2deg(bearing);
   }

double bearing(Location<double> pos1, Location<double> pos2) {
   double lat1 = pos1.Latitude() * PI / 180.0; // Umwandlung in Radiant
   double lat2 = pos2.Latitude() * PI / 180.0;
   double dLon = (pos2.Longitude() - pos1.Longitude()) * PI / 180.0;

   double y = sin(dLon) * cos(lat2);
   double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);

   double bearing = atan2(y, x) * 180.0 / PI;

   // Korrektur des Winkels auf den Bereich von 0 bis 360 Grad
   bearing = fmod(bearing + 360.0, 360.0);

   return bearing;
}


int pushMatchingToFront(data_vector<double>& addresses, double matchValue) {
   int i = 0, j = addresses.size() - 1;
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

void sortAddressesInRange(data_vector<double>& addresses, int endIndex) {
   sort(addresses.begin(), addresses.begin() + endIndex, [](const auto& a1, const auto& a2) {
      if (a1.second.first != a2.second.first) {
         return a1.second.first > a2.second.first; // Sortiere absteigend nach Entfernung
      }
      else {
         return a1.second.second < a2.second.second; // Sortiere aufsteigend nach Kurswinkel
         }
      });
   }

auto BerechneChatGPT(data_vector<double>& addresses, Location<double> point) {
   for (auto& [address, result] : addresses) {
      double dist = distance( point, { address.Latitude(), address.Longitude() });
      result.first = dist;
      double angle = bearing( { address.Latitude(), address.Longitude() }, point);
      result.second = fmod(360.0 - angle, 360.0); // Kurswinkel als Komplement des Azimuts
   }
}

auto TestchatGPT(data_vector<double>& addresses, std::string const& strFilename) {
   addresses.clear();
   std::ifstream infile(strFilename);  // Datei öffnen
   if (!infile) {
      std::cerr << "Die Datei konnte nicht geöffnet werden." << std::endl;
      return 1;
   }
   std::string line;
   while (getline(infile, line)) {  // Zeilenweise Daten einlesen
      TData<double> address;

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
   return 0;
}


auto WriteChatGPT(data_vector<double> const& addresses, std::string const& strFilename, int count = -1) {
   // Datei für die Ausgabe öffnen
   std::ofstream outfile(strFilename);
   if (!outfile) {
      std::cerr << "Die Datei konnte nicht geöffnet werden." << std::endl;
      return 1;
      }
   //outfile.imbue(std::locale("de_DE"));

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
   return 0;
   }



void Rechentest(std::string const& strFilename) {
   using myTimeType = std::chrono::microseconds; // std::chrono::milliseconds
   static const double time_factor = 1000000.;
   static const int    time_prec = 6;
   auto func_start = std::chrono::high_resolution_clock::now();
   data_vector<double> vData;
   TestchatGPT(vData, strFilename);
   
   auto func_ende = std::chrono::high_resolution_clock::now();
   auto runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " datasets read in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";


   func_start = std::chrono::high_resolution_clock::now();
   std::sort(vData.begin(), vData.end(), [](auto const& lhs, auto const& rhs) { return lhs.first.CompareDIN5007(rhs.first);  });
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " datasets sorted in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   func_start = std::chrono::high_resolution_clock::now();
   Location<double> point = { 52.520803, 13.40945 };
   BerechneChatGPT(vData, point);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " datasets calculated in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   std::string strOutput = "D:\\Test\\Testausgabe_alle.txt"s;
   func_start = std::chrono::high_resolution_clock::now();
   WriteChatGPT(vData, strOutput);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << vData.size() << " datasets wrote to \"" << strOutput << "\" in "
      << std::setprecision(time_prec) << runtime.count() / time_factor << " sec\n";

   strOutput = "D:\\Test\\Testausgabe.txt"s;

   func_start = std::chrono::high_resolution_clock::now();
   auto val = pushMatchingToFront(vData, 1000.0);
   func_ende = std::chrono::high_resolution_clock::now();
   runtime = std::chrono::duration_cast<myTimeType>(func_ende - func_start);
   std::cout << val << " datasets partition in "
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

   std::cout << "\nFinished.\n";
}

int main() {
   std::cout.setf(std::ios::showpoint);
   std::cout.setf(std::ios::fixed);

   std::string strInput = "D:\\Test\\berlin_infos.dat"s;
   Rechentest(strInput);
   #if defined __BORLANDC__
   std::cout << "... press a key ...";
   getchar();
   #endif
   return 0;
}

