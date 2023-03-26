
#include "ChatGPT.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

int TChatGPT::pushMatchingToFront(data_vector& addresses, double matchValue) {
   int i = 0, j = static_cast<int>(addresses.size()) - 1;
   while (i < j) {
      if (addresses[i].second.Distance() < matchValue) {
         i++;
      }
      else if (addresses[j].second.Distance() >= matchValue) {
         j--;
      }
      else {
         swap(addresses[i], addresses[j]);
      }
   }
   return i; // (v[i] == matchValue) ? i + 1 : i;
}

void TChatGPT::sortAddressesInRange(data_vector& addresses, int endIndex) {
   sort(addresses.begin(), addresses.begin() + endIndex, [](const auto& a1, const auto& a2) {
      if (a1.second.Distance() != a2.second.Distance()) {
         return a1.second.Distance() > a2.second.Distance(); // Sortiere absteigend nach Entfernung
      }
      else {
         return a1.second.Angle() < a2.second.Angle(); // Sortiere aufsteigend nach Kurswinkel
      }
      });
}

void TChatGPT::calculate_addresses(data_vector& addresses, Location const& point) {
   for (auto& [address, result] : addresses) {
      double dist = point.distance({ address.Latitude(), address.Longitude() });
      result.Distance(dist);
      double angle = point.bearing({ address.Latitude(), address.Longitude() });
      //result.Angle(fmod(360.0 - angle, 360.0)); // Kurswinkel als Komplement des Azimuts
      result.Angle(angle);
   }
}

void TChatGPT::delete_directory(const std::string& path) {
   if (std::filesystem::exists(path)) {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
         if (entry.is_directory()) {
            delete_directory(entry.path().string());
         }
         else {
            std::filesystem::remove(entry.path());
         }
      }
      std::filesystem::remove(path);
   }
}

void TChatGPT::delete_addresses(data_vector& addresses) {
   addresses.clear(); 
   addresses.shrink_to_fit();
   }

void TChatGPT::write_addresses_to_directories(const std::string& root_dir, const data_vector& addresses) {
   // create a root directory for all cities
   fs::create_directory(root_dir);

   for (const auto& [address, result] : addresses) {
      std::string dir = root_dir + "\\" + address.City() + "\\" + address.UrbanUnit() + "\\" + address.District();
      fs::create_directories(dir);  // Koorigiert, da alle Verzeichnisse erstellt werden müssen

      // open file with name of street
      std::ofstream file(dir + "\\" + address.Street() + ".csv", std::ios::app);
      file.setf(std::ios::fixed);
      file.setf(std::ios::showpoint);

      // write address data to file
      file << address.StreetNumber() << ";" << address.ZipCode() << ";" << address.UrbanUnit_Old() << ";"
           << std::setprecision(9) << address.Longitude() << ";" << std::setprecision(9) << address.Latitude()
           << ";" << std::setprecision(3) << result.Distance() << ";" << std::setprecision(3) << result.Angle()
           << std::endl;
      file.close();
   }
}


void TChatGPT::write_addresses_to_directory_sorted(const std::string& root_dir, data_vector& addresses) {
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
      if (iAN != iBN) {
         return iAN < iBN;
      }
      if (a.first.StreetNumber() != b.first.StreetNumber()) {
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
         << ";" << std::setprecision(3) << result.Distance() << ";" << std::setprecision(3) << result.Angle()
         << std::endl;

      // Update previous values
      prev_city = address.City();
      prev_unit = address.UrbanUnit();
      prev_district = address.District();
      prev_street = address.Street();
   }

   // Close last file
   if (file.is_open()) {
      file.close();
   }
}


void TChatGPT::read_addresses_from_directory(std::string directoryPath, data_vector& addresses) {
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
                     else if (count == 2) {
                        address.ZipCode(data);
                     }
                     else if (count == 3) {
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
                        double flTmp;
                        std::istringstream(data) >> flTmp;
                        result.Distance(flTmp);
                     }
                     else if (count == 7) {
                        double flTmp;
                        std::istringstream(data) >> flTmp;
                        result.Angle(flTmp);
                     }
                  }
                  addresses.push_back({ address, result });
               }
            }
         }
      }
   }
}

void TChatGPT::sort_DIN5007(data_vector& addresses) {
   std::sort(addresses.begin(), addresses.end(), [](auto const& lhs, auto const& rhs) { return lhs.first.CompareDIN5007(rhs.first);  });
}

void TChatGPT::sort_DIN5007_Var2(data_vector& addresses) {
   std::sort(addresses.begin(), addresses.end(), [](auto const& lhs, auto const& rhs) { return lhs.first.CompareDIN5007_Var2(rhs.first);  });
}

void TChatGPT::read_from_File(data_vector& addresses, std::string const& strFilename) {
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


void TChatGPT::write_to_File(data_vector const& addresses, std::string const& strFilename, int count) {
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
         << " -> " << std::fixed << std::setprecision(3) << distance.Distance() << "m"
         << " in " << std::fixed << std::setprecision(1) << distance.Angle() << "°\n";
      std::string line = line_stream.str();
      // Zeile in die Datei schreiben
      outfile << line;
      i++;
   }
   outfile.close();  // Datei schließen
}

