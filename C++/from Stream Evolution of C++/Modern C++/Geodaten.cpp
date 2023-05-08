#include "MyConvert.h"
#include "MyType_Traits.h"

#include "Geodaten.h"

#include "MyFileIterator.h"


#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <ranges>

namespace Geodaten {

template <typename string_type, myContainer container>
size_t parse(string_type const& source, std::string const& del, container& list) {
   size_t pos = 0u, end;
   do {
      end = source.find(del, pos);
      string_type part = source.substr(pos, end - pos);
      if(part.length() > 0) {
         string_type trimmed_part = part.substr(part.find_first_not_of(" "), part.find_last_not_of(" ") - part.find_first_not_of(" ") + 1);
         if(trimmed_part.length() > 0) [[likely]] {
            if constexpr (is_vector<container>::value) list.emplace_back(trimmed_part);
            else list.insert(trimmed_part);
            }
         }
      pos = end + del.length();
      } 
   while (end != string_type::npos);
   return list.size();
   }


template<typename... Args>
std::ostream& operator << (std::ostream& os, std::tuple<Args...> const& theTpl) {
   std::apply( [&os](Args const&... args) {
                      os << '[';
                      std::size_t n { 0 };
                      ((os << args << (++n != sizeof...(Args) ? ", " : "")), ...);
                      os << ']';
                      }, theTpl);
   return os;
   }
   
// wir fangen mit Thüringen an
data_type read_keys(std::string const& strFile) {
   /*
   static std::map<std::string, int> key_bridge = { { "L", 1}, { "R", 2}, { "K", 3}, { "G", 4 } };
   */
   static std::map<std::string, std::function<std::string (key_type&, std::vector<std::string> const&)>> key_bridge_th = {
      { "L", [](key_type& key_row, std::vector<std::string> const& parts) -> std::string { 
          std::get<0>(key_row) = string_to_int<int>(parts[1]);
          return parts[2];
          } },
      { "R", [](key_type& key_row, std::vector<std::string> const& parts) -> std::string {
          std::get<0>(key_row) = string_to_int<int>(parts[1]);
          std::get<1>(key_row) = string_to_int<int>(parts[2]);
          return parts[3];
          }  },
      { "K", [](key_type& key_row, std::vector<std::string> const& parts) -> std::string {
          std::get<0>(key_row) = string_to_int<int>(parts[1]);
          std::get<1>(key_row) = string_to_int<int>(parts[2]);
          std::get<2>(key_row) = string_to_int<int>(parts[3]);
          return parts[4];
          }  },
      { "G", [](key_type& key_row, std::vector<std::string> const& parts) -> std::string {
          std::get<0>(key_row) = string_to_int<int>(parts[1]);
          std::get<1>(key_row) = string_to_int<int>(parts[2]);
          std::get<2>(key_row) = string_to_int<int>(parts[3]);
          std::get<3>(key_row) = string_to_int<int>(parts[4]);
          return parts[5];
          }  }
   };


   data_type rows;

   std::ifstream ifs(strFile);
   if(!ifs.is_open()) [[unlikely]] {
      throw std::runtime_error("file not open");
      }
   else {
      std::string strRow;
      while(std::getline(ifs, strRow)) {
         key_type key_row { 0, 0, 0, 0, 0 };
         value_type value_row;
         std::vector<std::string> parts;
         parse(strRow, ";", parts);
         if (auto it = key_bridge_th.find(parts[0]); it != key_bridge_th.end()) value_row = it->second(key_row, parts);
         else [[unlikely]] {
            throw std::runtime_error("unexpected type in file");
            }
         /*
         switch(key_bridge[parts[0]]) {
            case 1:
               std::get<0>(key_row) = string_to_int<int>(parts[1]);
               value_row = parts[2];
               break;
            case 2:
               std::get<0>(key_row) = string_to_int<int>(parts[1]);
               std::get<1>(key_row) = string_to_int<int>(parts[2]);
               value_row = parts[3];
               break;
            case 3:
               std::get<0>(key_row) = string_to_int<int>(parts[1]);
               std::get<1>(key_row) = string_to_int<int>(parts[2]);
               std::get<2>(key_row) = string_to_int<int>(parts[3]);
               value_row = parts[4];
               break;
            case 4:
               std::get<0>(key_row) = string_to_int<int>(parts[1]);
               std::get<1>(key_row) = string_to_int<int>(parts[2]);
               std::get<2>(key_row) = string_to_int<int>(parts[3]);
               std::get<3>(key_row) = string_to_int<int>(parts[4]);
               value_row = parts[5];
               break;
            default:
               throw std::runtime_error("unexpected variant");
            }
            */
         rows.insert({ key_row, value_row });
         }
      }

   /*
   for(auto const& [key, value] : rows) {
      std::cout << key << " = " << value << "\n";
      }
   */
   return rows;
   }


void Read_Th(std::string const& strKeyFile, std::string const& strFile, std::ostream& out) {
   std::string strBuffer;
   static constexpr std::string_view delim{ "," };

   auto keys = read_keys(strKeyFile);

   for(auto const& line : my_lines { GetContent(strFile, strBuffer) }) {
      auto test = std::string_view{ line.begin(), line.end() } | std::views::split(delim) 
                           | std::views::transform([](auto d) { return std::string_view{ d.begin(), d.end() }; }) 
                           | std::ranges::to<std::vector>();
      key_type land = { string_to_int<int>(test[6]), 0, 0, 0, 0 };
      key_type seek = { string_to_int<int>(test[6]), string_to_int<int>(test[7]), string_to_int<int>(test[8]),
                        string_to_int<int>(test[9]), string_to_int<int>(test[10]) };
      out << "   { \"" << keys[land] << ", " << keys[seek] << ", " << test[16] << " " << test[12] << test[13] << "\"s, "
          << " { , }, " << "{ " << test[0] << ", " << test[1] << " } }, "
          << " // { " << test[14] << ", " << test[15] << " }"
          << "\n";
      
      //for (auto const& t : test) std::cout << t << " / ";
      //std::cout << "\n";
      }
   }

// 0  1  2         3   4    5    6    7    8    9    10   11     12     13   14   15      16       17       18       19       20       21   22   23  24  25  26  27
// X, Y, OBJECTID, OI, QUA, LAN, RBZ, KRS, GMD, OTT, SSS, GMDNR, STNNR, HNR, ADZ, HNRADZ, LANNAME, KRSNAME, AMTNAME, GMDNAME, OTTNAME, STN, PLZ, ONM, ZON, POT, PSN, AUD

void Read_Bb(std::string const& strKeyFile, std::string const& strFile, std::ostream & out) {
   std::string strBuffer;
   static constexpr std::string_view delim{ "," };
   data_type rows;

   for (auto const& line : my_lines{ GetContent(strFile, strBuffer) }) {
      auto test = std::string_view{ line.begin(), line.end() } | std::views::split(delim)
         | std::views::transform([](auto d) { return std::string_view{ d.begin(), d.end() }; })
         | std::ranges::to<std::vector>();

      key_type land     = { string_to_int<int>(test[5]), 0, 0, 0, 0 };
      key_type kreis    = { string_to_int<int>(test[5]), string_to_int<int>(test[6]), string_to_int<int>(test[7]), 0, 0 };
      key_type gemeinde = { string_to_int<int>(test[5]), string_to_int<int>(test[6]), string_to_int<int>(test[7]),
                            string_to_int<int>(test[8]), 0 };
      
      key_type ort      = { string_to_int<int>(test[5]), string_to_int<int>(test[6]), string_to_int<int>(test[7]),
                            string_to_int<int>(test[8]), string_to_int<int>(test[9]) };

      std::string strLand     = std::string { test[16].begin(), test[16].end() };
      std::string strKreis    = std::string { test[17].begin(), test[17].end() };
      std::string strGemeinde = std::string { test[19].begin(), test[19].end() };
      std::string strOrt      = std::string { test[20].begin(), test[20].end() };

      if(std::get<4>(ort) > 0) {
         if (auto ort_it = rows.find(ort); ort_it != rows.end()) {
            if (ort_it->second != strOrt)
               std::cerr << "error Ort: " << ort_it->first << " = " << ort_it->second << " / " << strOrt << "\n";
            }
         else {
            out << ort << ": " << strOrt << "\n";
            rows.insert({ ort, strOrt });
            }
         }

      if (std::get<3>(gemeinde) > 0) {
         if (auto gemeinde_it = rows.find(gemeinde); gemeinde_it != rows.end()) {
            if (gemeinde_it->second != strGemeinde)
               std::cerr << "error Gemeinde: " << gemeinde_it->first << " = " << gemeinde_it->second << " / " << strGemeinde << "\n";
            }
         else {
            out << gemeinde << ": " << strGemeinde << "\n";
            rows.insert({ gemeinde, strGemeinde });
            }
         }

      if (auto kreis_it = rows.find(kreis); kreis_it != rows.end()) {
         if (kreis_it->second != strKreis)
            std::cerr << "error Kreis: " << kreis_it->first << " = " << kreis_it->second << " / " << strKreis << "\n";
      }
      else {
         out << kreis << ": " << strKreis << "\n";
         rows.insert({ kreis, strKreis });
      }


      if (auto land_it = rows.find(land); land_it != rows.end() ) {
         if (land_it->second != strLand) 
            std::cerr << "error Land: " << land_it->first << " = " << land_it->second << " / " << strLand << "\n";
         }     
      else {
         out << land << ": " << strLand << "\n";
         rows.insert({ land, strLand });
         }

      }

   for (auto const& [key, value] : rows) {
      std::cout << key << " = " << value << "\n";
      }

   }

} // end namespace Geodaten
