#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS
#include "Test.h"
#include "Data.h"
#include <fstream>
#include <sstream>
#include <iterator>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <codecvt>
#include <format>
#include <map>
#include <cwchar>

//#include "pugixml-1.11/src/pugixml.hpp"

void ReadTest(std::string const& strFile, vecTestData& values) {
   static std::locale loc("de_DE");

   static std::vector<std::function<void(tplTestData&, std::wstring const&)>> funcs = {
         [](tplTestData& data, std::wstring const& val) { get<0>(data) = val; },
         [](tplTestData& data, std::wstring const& val) { get<1>(data) = val; },
         [](tplTestData& data, std::wstring const& val) { get<2>(data) = val; },
         [](tplTestData& data, std::wstring const& val) { get<3>(data) = val; },
         [](tplTestData& data, std::wstring const& val) { get<4>(data) = std::stod(val); },
         [](tplTestData& data, std::wstring const& val) { get<5>(data) = std::stod(val); }
      };

   std::wifstream ifs(strFile);

   tplTestData data;
   if (ifs.is_open()) {
      ifs.imbue(std::locale(std::locale("de_DE"), new std::codecvt_utf8<wchar_t>{}));
      values.reserve(std::count(std::istreambuf_iterator<wchar_t>(ifs), std::istreambuf_iterator<wchar_t>(), '\n'));
      ifs.seekg(0, std::ios::beg);
      std::wstring strRow;
      while (getline(ifs, strRow)) {
         int iCount = 0;
         size_t iPos = 0, iEnd;
         do {
            iEnd = strRow.find(L";", iPos);
            funcs[iCount++](data, strRow.substr(iPos, iEnd - iPos));
            iPos = iEnd + 1;
         } while (iEnd != std::wstring::npos);
         values.emplace_back(move(data));
      }
   }
   else {
      std::ostringstream os;
      os << "error while opeining file \"" << strFile << "\"" << std::endl;
      throw std::runtime_error(os.str().c_str());
   }
   return;
   }


void Test(std::string const& strFile, vecTestData const& values, vecTestResult& results) {
   static std::locale loc(std::locale("de_DE"), new TMyNum);
   size_t max = values.size();

   pugi::xml_document doc;
   auto declarationNode = doc.append_child(pugi::node_declaration);
   declarationNode.append_attribute("version") = "1.0";
   declarationNode.append_attribute("encoding") = "ISO-8859-1";
   declarationNode.append_attribute("standalone") = "yes";

   auto root = doc.append_child("Locations");

   for (size_t i = 0; i < max; ++i) {
      pugi::xml_node nodeChild = root.append_child("Location");
      nodeChild.append_attribute("id") = i;
      pugi::xml_node nodeChildVal = nodeChild.append_child("Place");
      nodeChildVal.append_child(pugi::node_pcdata).set_value(pugi::as_utf8(get<0>(values[i])).c_str());
      nodeChildVal = nodeChild.append_child("State");
      nodeChildVal.append_child(pugi::node_pcdata).set_value(pugi::as_utf8(get<1>(values[i])).c_str());
      nodeChildVal = nodeChild.append_child("Country");
      nodeChildVal.append_child(pugi::node_pcdata).set_value(pugi::as_utf8(get<2>(values[i])).c_str());
      nodeChildVal = nodeChild.append_child("Continent");
      nodeChildVal.append_child(pugi::node_pcdata).set_value(pugi::as_utf8(get<3>(values[i])).c_str());
      nodeChildVal = nodeChild.append_child("Latitude");
      nodeChildVal.append_child(pugi::node_pcdata).set_value(std::to_string(get<4>(values[i])).c_str());
      nodeChildVal = nodeChild.append_child("Longitude");
      nodeChildVal.append_child(pugi::node_pcdata).set_value(std::to_string(get<5>(values[i])).c_str());
   }
 
   root = doc.append_child("Results");
   for (size_t i = 0; i < max; ++i) {
      for(size_t j = 0; j < max; ++j) {
         if(i != j) {
            auto [distance, angle] = Calculate(to_loc(values[i]), to_loc(values[j]));
            pugi::xml_node nodeChild = root.append_child("Result");
            nodeChild.append_attribute("from") = i;
            nodeChild.append_attribute("to")   = j;
            pugi::xml_node nodeChildVal = nodeChild.append_child("Distance");
            nodeChildVal.append_child(pugi::node_pcdata).set_value(std::to_string(distance).c_str());
            nodeChildVal = nodeChild.append_child("Angle");
            nodeChildVal.append_child(pugi::node_pcdata).set_value(std::to_string(angle).c_str());

            //results.emplace_back(std::make_tuple(values[i], values[j], distance, angle));
            }
      }
   }

   doc.save_file(strFile.c_str(), PUGIXML_TEXT("  "));

}

void TestAusgabe(std::wostream& out, std::string const& strFile) {
   auto  corr = [](std::wstring const& s) {
      std::mbstate_t state = std::mbstate_t();
      const wchar_t* cp = s.c_str();
      return wcsrtombs(nullptr, &cp, s.size(), &state) - wcslen(cp);
      };

   pugi::xml_document doc;
   pugi::xml_parse_result result = doc.load_file(strFile.c_str(), pugi::parse_default | pugi::parse_fragment);
   if (!result) {
      std::cerr << "XML [" << strFile << "] parsed with errors\n";
      std::cerr << "Error description: " << result.description() << "\n";
      std::cerr << "Error offset: " << result.offset
         << " (error at [..." << (strFile.c_str() + result.offset) << "]\n\n";
   }
   else {
      pugi::xml_node start = doc.find_node([](pugi::xml_node& node) { return strcmp(node.name(), "Locations") == 0; });
      if (start) {
         static std::locale loc(std::locale("de_DE"), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>);
         std::wostringstream outb;
         outb.imbue(loc);
         for (pugi::xml_node loc : start.children("Location")) {
             std::wstring strPlace = pugi::as_wide(loc.child_value("Place"));
             outb << std::left  << std::setw(50) << strPlace << L" "
                  << std::left  << std::setw(40) << pugi::as_wide(loc.child_value("Country")) << L" "
                  << std::right << std::setw(10) << std::setprecision(6) << loc.child("Latitude").text().as_double() << L" "
                  << std::right << std::setw(10) << std::setprecision(6) << loc.child("Longitude").text().as_double()
                  << std::endl;
             
            }
         out.write(outb.str().c_str(), outb.str().size());
         }
      }
   }

void convert(std::string const& target, std::wstring& source) {
   size_t length;
   mbstate_t mbs = std::mbstate_t();
   std::mbrlen(NULL, 0, &mbs);  /* initialize mbs */
   wchar_t dest;
   size_t iMax = target.length();
   const char* pt = target.c_str();
   source.clear();
   while (iMax > 0) {
      length = mbrtowc(&dest, pt, iMax, &mbs);
      if ((length == 0) || (length > iMax)) break;
     source.push_back(dest);
      pt += length; iMax -= length;
      }
   return;
   }

void Helper(std::string const& strPath) {
   static std::locale loc("de_DE");
   std::wstring strWPath;
   convert(strPath, strWPath);
   std::map<std::wstring, int> mapCountries;
   std::map<std::wstring, int> mapStates;

   std::string strFile1 = strPath + "\\raw references.txt";
   std::string strFile2 = strPath + "\\test spots.csv";

   std::wifstream ifs(strFile1);
   std::wofstream ofs(strFile2);
   ifs.imbue(std::locale(std::locale("de_DE"), new std::codecvt_utf8<wchar_t>{}));
   ofs.imbue(std::locale(std::locale(""), new std::codecvt_utf8<wchar_t>{}));

   if (ifs.is_open()) {
      ifs.imbue(std::locale(std::locale("de_DE"), new std::codecvt_utf8<wchar_t>{}));
      std::wstring strRow;
      while (getline(ifs, strRow)) {
         std::wstring strPlace = strRow.substr(0, strRow.find(L"/@"));
         std::wstring strCoordinates = strRow.substr(strRow.find(L"/@") + 2, strRow.length() - strRow.find(L"/@") + 2);
         std::transform(strCoordinates.begin(), strCoordinates.end(), strCoordinates.begin(), [](auto c) { return c == L',' ? L';' : c; });
         std::vector<std::wstring> vecParts;
         size_t iPos = 0, iEnd;
         do {
            iEnd = strPlace.find(L",+", iPos);
            std::wstring strPart = strPlace.substr(iPos, iEnd - iPos);
            strPart.erase(strPart.find_last_not_of(L"0123456789+-") + 1, strPart.length()); // right trim
            std::transform(strPart.begin(), strPart.end(), strPart.begin(), [](auto c) { return c == L'+' ? L' ' : c; });
            if(strPart.length() > 0) vecParts.push_back(strPart);
            iPos = iEnd + 2;
         } while (iEnd != std::wstring::npos);



         std::wstring strCity    = vecParts[0];
         std::wstring strCountry = L"";
         std::wstring strState   = L"";
         if(vecParts.size() > 1) strCountry = vecParts[vecParts.size() - 1];
         if(vecParts.size() > 2) strState   = vecParts[vecParts.size() - 2];
   
         if(strCountry == L"") std::wcout << strCity << std::endl;
         if(strCountry == L"USA") mapStates[strState] += 1;

        // std::wcout << strCity << std::endl;
         mapCountries[strCountry] += 1;
        ofs << strCity << L";" << strState << L";" << strCountry << L";" << strCoordinates << std::endl;
        }
      //for(auto [county, count] : mapCountries) std::wcout << county << " = " << count << std::endl;
      for (auto [state, count] : mapStates) std::wcout << state << " = " << count << std::endl;
      }
   }


void TestCalculate(std::string const& strPath) {
   std::wstring strWPath;
   convert(strPath, strWPath);

   // testfile: spots references.txt
   std::string strFile_testdata = strPath + "\\spots references.txt";
   std::string strFile_test_out = strPath + "\\test out.xml";
   std::wstring strFile_test_out2 = strWPath + L"\\test out2.txt";
   vecTestData   values;
   vecTestResult results;
   ReadTest(strFile_testdata, values);
   Test(strFile_test_out, values, results);

   //static std::locale loc(std::locale("de_DE"), new std::codecvt_utf16<wchar_t, 0x10ffff, std::generate_header>);
   std::wofstream of(strFile_test_out2);
   //of.imbue(loc);
   //of << (wchar_t)0xfeff; // BOM
   //_setmode(_fileno(stdout), _O_U16TEXT);
   of.setf(std::ios::fixed);
   of.setf(std::ios::showpoint);
   TestAusgabe(of, strFile_test_out);
   of.close();
}
