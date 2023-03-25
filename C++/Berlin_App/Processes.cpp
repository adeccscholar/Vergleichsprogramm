#define _CRT_SECURE_NO_WARNINGS
#include "Processes.h"
#include "Processes_Data.h"
#include "MyBuffer.h"
#include "My_Lines_Iterator.h"
#include "My_Double.h"

#include <iomanip>
#include <string_view>
#include <string>
#include <set>
#include <fstream>
#include <sstream>
#include <iterator>
#include <functional>
#include <stdexcept>
#include <tuple>
#include <algorithm>
#include <execution>
#include <map>
#include <set>
#include <array>
#include <filesystem>
#include <tuple>
#include <format>
#include <mutex>
#include <ranges>

std::mutex ins_mutex, write_mutex;


using namespace std;
namespace fs = std::filesystem;

extern size_t iBufferPages1, iBufferPages2;

void Prepare(std::string const& strPath) {
   strFile_data       = strPath + "\\berlin_infos.dat";
   strFile_data_out   = strPath + "\\output_streets.txt";
   strFile_dist_out   = strPath + "\\output_distance.txt";
   strFile_spots      = strPath + "\\berlin_spots.txt";
   strFile_spots_out  = strPath + "\\output_spots.txt";
   strFile_points_out = strPath + "\\output_points.txt";
   strFile_control    = strPath + "\\output_control.txt";
   }

void ReadFile(std::string const& strFile, myData& vec_data) {
   ifstream ifs(strFile);
   if (!ifs.is_open()) {
      ostringstream os;
      os << "error while opeining file \"" << strFile << "\"" << endl;
      throw runtime_error(os.str().c_str());
      }

   const auto iSize = fs::file_size(strFile);
   std::string strBuff(iSize, '\0');
   ifs.read(strBuff.data(), iSize);
   ifs.close();
   std::string_view test(strBuff.data(), strBuff.size());
   /*
   stringstream strStream;
   strStream << ifs.rdbuf();
   ifs.close();
   string_view test(strStream.view());
   */
   my_line_count::reset();
   vector<my_line_count> lines;
   lines.reserve(count(test.begin(), test.end(), '\n'));
   my_lines file_data(test);
   copy(file_data.begin(), file_data.end(), back_inserter(lines));
   vec_data.resize(lines.size());

   std::for_each(execution::par, lines.cbegin(), lines.cend(), [&vec_data](auto const& val) {
      size_t iPos = 0, iEnd;
      int iCount = 0;
      do {
         iEnd = val.view.find(";", iPos);
         funcs_read_f[iCount++](vec_data[val.index], val.view.substr(iPos, iEnd - iPos));
         if (iEnd != string_view::npos) iPos = iEnd + 1;
      } while (iEnd != string_view::npos);
      });
}


void CleanData(myData& vec_data) {
   vec_data.clear();
   vec_data.shrink_to_fit();
}

void Statistic(ostream& out, myData const& vec_data) {
   map<tuple<string, string, string, string>, int> counting;
   std::for_each(vec_data.begin(), vec_data.end(), [&counting](TData const& data) { counting[make_tuple(data.Street(), data.Urban_Unit(), data.District(), data.Street())] += 1;	 });
   for (auto [key, value] : counting) {
      out << get<0>(key) << " - " << get<1>(key) << " / " << get<2>(key) << " " << get<3>(key) << " = " << value << endl;
   }
}


void Write_Streets_parallel(std::string const& strFile, size_t iMaxCount, myData const& vec_data) {
   static locale loc(locale("de_DE"), new TMyNum);
   using tplParts = std::tuple<citData, citData, std::ostringstream>;

   std::vector<tplParts> vecParts;

   size_t iSteps = vec_data.size() / iMaxCount;
   
   for(size_t i = 0; i < iMaxCount; ++i) { 
      vecParts.emplace_back(vec_data.cbegin() + i * iSteps, vec_data.cbegin() + (i + 1) * iSteps, std::ostringstream() );
      }
   std::get<1>(vecParts[iMaxCount-1]) = vec_data.cend();

   std::for_each(execution::par, vecParts.begin(), vecParts.end(), [](auto& part) mutable {
      //std::get<2>(part).imbue(loc);
      //std::get<2>(part).precision(3);
      //std::get<2>(part).setf(ios::showpoint);
      //std::get<2>(part).setf(ios::fixed);
      std::ranges::for_each(std::get<0>(part), std::get<1>(part), [&part](auto& val) mutable {
         std::string strCity   = val.City() + " - "s + val.District() + ", "s;
         std::string strStreet = val.Street() + " "s + val.StreetNumber();
         std::get<2>(part) << left << setw(5) << val.ZipCode() << " "s
                           << setw(75) << strCity 
                           << setw(65) << strStreet << "="s 
                          // << right << setw(15) << val.Distance() << "m / "s
                          // << setw(7) << val.Angle() << "°"s 
                           << my_Double_to_String_G(val.Distance(), 3) << "m / "s
                           << my_Double_to_String_G(val.Angle(), 3) << "°"s
                           << endl;
         });
      });

   ofstream ofs(strFile); //, std::ios::binary);
   std::vector<char> buffer(iBufferPages1 * 4'096);
   ofs.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
   //for (auto& val : vecParts) ofs << std::get<2>(val).str();
   std::for_each(vecParts.cbegin(), vecParts.cend(), [&ofs](auto const& part) { ofs << std::get<2>(part).str(); } );
   }


void Write_Streets(std::string const& strFile, myData const& vec_data) {
   static locale loc(locale("de_DE"), new TMyNum);
   ofstream of(strFile);
   string dummy;
   dummy.reserve(176 * vec_data.size());
   ostringstream os(forward<string>(dummy));
   std::for_each(vec_data.cbegin(), vec_data.cend(), [&os](auto& val) {
      std::string strCity = val.City() + " - "s + val.District() + ", "s;
      std::string strStreet = val.Street() + " "s + val.StreetNumber();
      os << left  << setw( 5) << val.ZipCode() << " "
         << setw(75) << strCity 
         << setw(65) << strStreet << "="s 
         << my_Double_to_String_G(val.Distance(), 3) << "m / "s
         << my_Double_to_String_G(val.Angle(), 3) << "°"s
         << endl;
      });

   of << os.str();
   }


void Write_Part_of_Streets(std::string const& strFile, size_t iMaxCount, myData::const_iterator begin_it, myData::const_iterator end_it) {
   static locale loc(locale("de_DE"), new TMyNum);

   using tplParts = std::tuple<citData, citData, std::ostringstream>;
   std::vector<tplParts> vecParts;

   size_t iSteps = (end_it - begin_it) / iMaxCount;

   for (size_t i = 0; i < iMaxCount; ++i) {
      vecParts.emplace_back(begin_it + i * iSteps, begin_it + (i + 1) * iSteps, std::ostringstream());
   }
   std::get<1>(vecParts[iMaxCount - 1]) = end_it;

   std::for_each(execution::par, vecParts.begin(), vecParts.end(), [](auto& part)  {
      std::for_each(std::get<0>(part), std::get<1>(part), [&part](auto& val) mutable {
         std::string strCity   = val.City() + " - "s + val.District() + ", "s;
         std::string strStreet = val.Street() + " "s + val.StreetNumber();
         std::get<2>(part) << left << setw( 5) << val.ZipCode() << " "s
                           << setw(75) << strCity   // (val.City() + " - "s + val.District() + ", "s)
                           << setw(65) << strStreet << "="s // (val.Street() + " "s + val.StreetNumber()) << "="s
                           << right << setw(15) << my_Double_to_String_G(val.Distance(),3) << "m /"s
                           << setw(7) << my_Double_to_String_G(val.Angle(),3) << "°"s
                           << endl;
         });
      });

   ofstream ofs(strFile, std::ios::binary);
   //char buffer[65'536];    // 16'384]
   std::vector<char> buffer(iBufferPages2 * 4'096);
   ofs.rdbuf()->pubsetbuf(buffer.data(), sizeof(buffer).size());
   for (auto& val : vecParts) ofs << std::get<2>(val).str();
   }


void Write_Part_of_Addresses(std::string const& strFile, myData::const_iterator begin_it, myData::const_iterator end_it) {
   ostringstream os;
   std::vector<char> buffer(iBufferPages2 * 4'096);
   ofstream ofs(strFile);
   ofs.rdbuf()->pubsetbuf(buffer.data(), sizeof(buffer).size());
   ofs.imbue(locale());
   ofs.setf(ios::fixed);
   std::for_each(begin_it, end_it, [&ofs](auto& val) {
      ofs << val.City() << ";" << val.Street() + ";" + val.StreetNumber() << ";" << val.ZipCode() << ";" << val.Urban_Unit() + ";"
          << val.Old_Unit() << ";" << val.District() + ";" 
          << setprecision(9) << val.Latitude() << ";" << setprecision(9) << val.Longitude() << ";"
          << setprecision(3) << val.Distance() << ";" << setprecision(3) << val.Angle() << endl;
      });
   }


template<typename Iter, typename Pred, typename Op>
void for_each_if(Iter first, Iter last, Pred p, Op op) {
   std::for_each(first, last, [&p, &op](auto it) mutable { if (p(it)) op(it);  });
   }

void Read_Directory(string const& strPath, myData& vec_data) {

   using detail_data  = std::tuple<std::string, std::string, std::string, std::string>;
   //using data_to_read = std::tuple<fs::path, stringstream, vector<my_line_count>, detail_data>;
   using data_to_read = std::tuple<fs::path, string, vector<my_line_count>, detail_data>;
   using to_read_vec  = std::vector<data_to_read>;
   to_read_vec to_read;

   try {
      std::vector<fs::path> cities;
      std::copy(fs::directory_iterator(strPath), fs::directory_iterator(), back_inserter(cities));
      auto it_cities = std::partition(execution::par, cities.begin(), cities.end(), [](auto& d) { return fs::is_directory(d); });
      std::for_each(cities.begin(), it_cities, [&to_read](auto& d) {
         std::string strCity = d.stem().string();
         std::vector<fs::path> units;
         std::copy(fs::directory_iterator(d), fs::directory_iterator(), back_inserter(units));
         auto it_units = std::partition(execution::par, units.begin(), units.end(), [](auto& d) { return fs::is_directory(d); });
         std::for_each(units.begin(), it_units, [&to_read, &strCity](auto& d) {
            std::string strUnit = d.stem().string();
            std::vector<fs::path> districts;
            std::copy(fs::directory_iterator(d), fs::directory_iterator(), back_inserter(districts));
            auto it_districts = std::partition(execution::par, districts.begin(), districts.end(), [](auto& d) { return fs::is_directory(d); });
            std::for_each(districts.begin(), it_districts, [&to_read, &strCity, &strUnit](auto& d) {
               std::string strDistrict = d.stem().string();
               std::vector<fs::path> streets;
               std::copy(fs::directory_iterator(d), fs::directory_iterator(), back_inserter(streets));
               auto it_streets = std::partition(execution::par, streets.begin(), streets.end(), [](auto& d) { return fs::is_regular_file(d); });
               std::for_each(streets.begin(), it_streets, [&to_read, &strCity, &strUnit, &strDistrict](auto& d) {
                  std::string strStreet = d.stem().string();
                  detail_data data = { strCity, strUnit, strDistrict, strStreet  };
                  //to_read.emplace_back(d, stringstream(), vector<my_line_count>(), forward<detail_data>(data) );
                  to_read.emplace_back(d, std::string(), vector<my_line_count>(), forward<detail_data>(data));
                  });
               });
            });
         });
      }
   catch (exception& ex) {
      cerr << "error while preparing reading: " << ex.what() << endl;
      return;
      }

   my_line_count::reset(); /// !!!
   std::for_each(execution::par, to_read.begin(), to_read.end(), [](auto& val) mutable {
      std::ifstream ifs(get<0>(val).string());

      if (ifs.is_open()) {
         /*
         get<1>(val) << ifs.rdbuf();
         string_view test(get<1>(val).view());
         */
         auto iSize = fs::file_size(get<0>(val).string());
         get<1>(val).resize(iSize);
         ifs.read(get<1>(val).data(), iSize);
         ifs.close();
         std::string_view test(get<1>(val).data(), get<1>(val).size());

         get<2>(val).reserve(count(test.begin(), test.end(), '\n'));   /// !!!
         my_lines file_data(test);
         copy(file_data.begin(), file_data.end(), back_inserter(get<2>(val)));
         }
      } );
   vec_data.resize(my_line_count::GetCounter());
   
   try {
      std::for_each(execution::par, to_read.begin(), to_read.end(), [&vec_data](auto& val)  {
         /*
         std::for_each(execution::par, get<2>(val).begin(), get<2>(val).end(), [&vec_data, &val](auto& val_) mutable {
            vec_data[val_.index].City(get<0>(get<3>(val)));
            vec_data[val_.index].Urban_Unit(get<1>(get<3>(val)));
            vec_data[val_.index].District(get<2>(get<3>(val)));
            vec_data[val_.index].Street(get<3>(get<3>(val)));
         */
         auto const& [strCity, strUnit, strDistrict, strStreet] = get<3>(val);
         std::for_each(execution::par, get<2>(val).begin(), get<2>(val).end(), [&vec_data, &strCity, &strUnit, &strDistrict, &strStreet](auto& val_) mutable {
            vec_data[val_.index].City(strCity);
            vec_data[val_.index].Urban_Unit(strUnit);
            vec_data[val_.index].District(strDistrict);
            vec_data[val_.index].Street(strStreet);

            size_t iPos = 0, iEnd, iCount = 0;
            do {
               iEnd = val_.view.find(";", iPos);
               funcs_read_d[iCount++](vec_data[val_.index], val_.view.substr(iPos, iEnd - iPos));
               if (iEnd != string_view::npos) iPos = iEnd + 1;
            } while (iEnd != string_view::npos);
            });
         });
      }
   catch(exception& ex) {
      cerr << "error while preparing reading: " << ex.what() << endl;
      }

   }


void Clean_Directory(string const& strPath) {
   static function<void (fs::path const&, bool)> ClearDir = [](fs::path const& p, bool boDelete) {
      std::vector<fs::path> files;
      std::copy(fs::directory_iterator(p), fs::directory_iterator(), std::back_inserter(files));
      auto it_dir = std::partition(std::execution::par, files.begin(), files.end(), [](auto& p) { return !fs::is_directory(p); });
      if (boDelete) std::for_each(std::execution::par, files.begin(), it_dir, [](auto& p) { fs::remove(p); }); // delete files
      std::for_each(std::execution::par, it_dir, files.end(), [](auto& p2) { ClearDir(p2, true); });
      if (boDelete) fs::remove(p);
   };


   ClearDir(strPath, false);
   //for_each(fs::directory_iterator(strPath), fs::directory_iterator(), [](auto& d) { if (fs::is_directory(d)) fs::remove_all(d.path()); });
   }


//#define MY_DEBUG
void WriteTo_Directory_fast(string const& strPath, size_t iMaxCount, size_t iBorder, myData& vec_data) {
   using tplParts = std::tuple<citData, citData, std::ostringstream>;
   using myIterPair = tuple<myData::const_iterator, myData::const_iterator, fs::path, std::vector<tplParts>>;

   static constexpr auto compare = [](TData const& a, TData const& b) noexcept {
      //if (auto cmp = std::tie(a.City(), a.Urban_Unit(), a.District(), a.Street()) <=> std::tie(b.City(), b.Urban_Unit(), b.District(), b.Street());  cmp != 0) return cmp < 0;
      ///*
      if (auto cmp = a.City() <=> b.City(); cmp != 0) return cmp < 0;
      if (auto cmp = a.Urban_Unit() <=> b.Urban_Unit(); cmp != 0) return cmp < 0;
      if (auto cmp = a.District() <=> b.District(); cmp != 0) return cmp < 0;
      if (auto cmp = a.Street() <=> b.Street(); cmp != 0) return cmp < 0;
      //*/
      string::size_type szA, szB;
      int iAN = stoi(a.StreetNumber(), &szA), iBN = stoi(b.StreetNumber(), &szB);
      if (auto cmp = iAN <=> iBN; cmp != 0) return cmp < 0;
      if (auto cmp = a.StreetNumber().substr(szA) <=> b.StreetNumber().substr(szB); cmp != 0) return cmp < 0;
      return (a.ZipCode() <=> b.ZipCode()) < 0;
      };

   // nicht zwangsläufig sortiert, also vorher sortieren !!!!   
   #if defined MY_DEBUG
   auto func_start = chrono::high_resolution_clock::now();
   #endif
   std::sort(execution::par, vec_data.begin(), vec_data.end(), compare );
   #if defined MY_DEBUG
   auto func_ende = chrono::high_resolution_clock::now();
   auto time = chrono::duration_cast<chrono::microseconds>(func_ende - func_start).count();
   cout << endl << "   --> sorting " << setprecision(3) << time / 1000.0 << endl;
 
   func_start = chrono::high_resolution_clock::now();
   #endif
   vector<myIterPair> positions;
   fs::path myOldPath = "";
   for (auto first = vec_data.cbegin(); first != vec_data.cend(); ) {
      TData const& curr = *first;
      fs::path myPath = fs::path(strPath) / first->City() / first->Urban_Unit() / first->District();
      if(myPath != myOldPath) {
         fs::create_directories(myPath);
         myOldPath = myPath;
         }

      auto last = find_if(execution::par, first, vec_data.cend(), [&first](auto const& val) {
                return first->Street() != val.Street() ? true : (first->District() != val.District() ? true : (first->Urban_Unit() != val.Urban_Unit() ? true : (first->City() != val.City() ? true : false)));
                });
      
      positions.emplace_back(first, last, myPath / (first->Street() + ".csv"s), std::vector<tplParts>());
      first = last;
   }

   #if defined MY_DEBUG
   func_ende = chrono::high_resolution_clock::now();
   time = chrono::duration_cast<chrono::microseconds>(func_ende - func_start).count();
   cout << "   --> preparing " << setprecision(3) << time / 1000.0 << endl;

   func_start = chrono::high_resolution_clock::now();
   #endif
   std::for_each(execution::par, positions.begin(), positions.end(), [&iMaxCount, &iBorder](myIterPair& val) {
      if(auto size = (size_t)std::distance(get<0>(val), get<1>(val)); size > iBorder) {
         size_t iSteps = size / iMaxCount;
         for (size_t i = 0; i < iMaxCount; ++i) {
            scoped_lock<mutex> guard(ins_mutex);
            get<3>(val).emplace_back(get<0>(val) + i * iSteps, get<0>(val) + (i + 1) * iSteps, std::ostringstream());
            }
         std::get<1>(get<3>(val)[iMaxCount - 1]) = get<1>(val);

         std::for_each(execution::par, get<3>(val).begin(), get<3>(val).end(), [](auto& part) mutable {
            std::for_each(std::get<0>(part), std::get<1>(part), [&part](auto& value) mutable {
                std::get<2>(part) << value.StreetNumber() << ";" << value.ZipCode() << ";" << value.Old_Unit() << ";"
                    << my_Double_to_String(value.Latitude(), 9) << ";"
                    << my_Double_to_String(value.Longitude(), 9) << ";"
                    << my_Double_to_String(value.Distance(), 3) << ";"
                    << my_Double_to_String(value.Angle(), 3)
                    << endl;
               });
            });

         scoped_lock<mutex> write_block(write_mutex);
         ofstream ofs(get<2>(val)); //, std::ios::binary);
         std::vector<char> buffer(iBufferPages3 * 4'096);
         ofs.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
         std::for_each(get<3>(val).cbegin(), get<3>(val).cend(), [&ofs](auto const& part) { ofs << std::get<2>(part).str(); });
      }
      else {
         ofstream ofs(get<2>(val)); //, std::ios::binary);
         std::vector<char> buffer(iBufferPages3 * 4'096);
         ofs.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
         std::for_each(get<0>(val), get<1>(val), [&ofs](auto const& value) {
            ofs << value.StreetNumber() << ";" << value.ZipCode() << ";" << value.Old_Unit() << ";"
                << my_Double_to_String(value.Latitude()) << ";"
                << my_Double_to_String(value.Longitude()) << ";"
                << my_Double_to_String(value.Distance()) << ";"
                << my_Double_to_String(value.Angle())
                << endl;
            });
         }
      });
   #if defined MY_DEBUG
   func_ende = chrono::high_resolution_clock::now();
   time = chrono::duration_cast<chrono::microseconds>(func_ende - func_start).count();
   cout << "   --> writing " << setprecision(3) << time / 1000.0 << endl;
   #endif
}

void WriteTo_Directory(string const& strPath, size_t iMaxCount, size_t iBorder, myData& vec_data) {
   using tplParts = std::tuple<citData, citData, std::ostringstream>;

   static constexpr auto compare = [](TData const& a, TData const& b) noexcept {
      //if (auto cmp = std::tie(a.City(), a.Urban_Unit(), a.District(), a.Street()) <=> std::tie(b.City(), b.Urban_Unit(), b.District(), b.Street());  cmp != 0) return cmp < 0;
      ///*
      if (auto cmp = a.City() <=> b.City(); cmp != 0) return cmp < 0;
      if (auto cmp = a.Urban_Unit() <=> b.Urban_Unit(); cmp != 0) return cmp < 0;
      if (auto cmp = a.District() <=> b.District(); cmp != 0) return cmp < 0;
      if (auto cmp = a.Street() <=> b.Street(); cmp != 0) return cmp < 0;
      //*/
      string::size_type szA, szB;
      int iAN = stoi(a.StreetNumber(), &szA), iBN = stoi(b.StreetNumber(), &szB);
      if (auto cmp = iAN <=> iBN; cmp != 0) return cmp < 0;
      if (auto cmp = a.StreetNumber().substr(szA) <=> b.StreetNumber().substr(szB); cmp != 0) return cmp < 0;
      return (a.ZipCode() <=> b.ZipCode()) < 0;
   };

   // nicht zwangsläufig sortiert, also vorher sortieren !!!!   
#if defined MY_DEBUG
   auto func_start = chrono::high_resolution_clock::now();
#endif
   std::sort(execution::par, vec_data.begin(), vec_data.end(), compare);
#if defined MY_DEBUG
   auto func_ende = chrono::high_resolution_clock::now();
   auto time = chrono::duration_cast<chrono::microseconds>(func_ende - func_start).count();
   cout << endl << "   --> sorting " << setprecision(3) << time / 1000.0 << endl;

   func_start = chrono::high_resolution_clock::now();
#endif
   using myIterPair = tuple<myData::const_iterator, myData::const_iterator, fs::path>;
   vector<myIterPair> positions;
   fs::path myOldPath = "";
   for (auto first = vec_data.cbegin(); first != vec_data.cend(); ) {
      TData const& curr = *first;
      fs::path myPath = fs::path(strPath) / first->City() / first->Urban_Unit() / first->District();
      if (myPath != myOldPath) {
         fs::create_directories(myPath);
         myOldPath = myPath;
      }

      auto last = find_if(execution::par, first, vec_data.cend(), [&first](auto const& val) {
         return first->Street() != val.Street() ? true : (first->District() != val.District() ? true : (first->Urban_Unit() != val.Urban_Unit() ? true : (first->City() != val.City() ? true : false)));
         });

      positions.emplace_back(first, last, myPath / (first->Street() + ".csv"s));
      first = last;
   }

#if defined MY_DEBUG
   func_ende = chrono::high_resolution_clock::now();
   time = chrono::duration_cast<chrono::microseconds>(func_ende - func_start).count();
   cout << "   --> preparing " << setprecision(3) << time / 1000.0 << endl;

   func_start = chrono::high_resolution_clock::now();
#endif

   std::for_each(execution::par, positions.cbegin(), positions.cend(), [](myIterPair const& val)  {
         ofstream ofs(get<2>(val)); //, std::ios::binary);
         std::vector<char> buffer(iBufferPages3 * 4'096);
         ofs.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
         std::for_each(get<0>(val), get<1>(val), [&ofs](auto const& value) {
            ofs << value.StreetNumber() << ";" << value.ZipCode() << ";" << value.Old_Unit() << ";"
                << my_Double_to_String(value.Latitude()) << ";"
                << my_Double_to_String(value.Longitude()) << ";"
                << my_Double_to_String(value.Distance()) << ";"
                << my_Double_to_String(value.Angle()) 
                << endl;
            
            });
         });
#if defined MY_DEBUG
   func_ende = chrono::high_resolution_clock::now();
   time = chrono::duration_cast<chrono::microseconds>(func_ende - func_start).count();
   cout << "   --> writing " << setprecision(3) << time / 1000.0 << endl;
#endif
}


void CalculatingDistance(pair<double, double> const& point, myData& vec_data) {
   CalculateFor(vec_data.begin(), vec_data.end(), point);
   }

void Partion_Distance(myData& vec_data, int iDist, myData::iterator& it) {
   it = partition(execution::par, vec_data.begin(), vec_data.end(), [iDist](TData const& val) { return val.Distance() < iDist; });
}

void Sorting4Standard(myData& vec_data) {
   std::sort(execution::par, vec_data.begin(), vec_data.end(),[](auto const& left, auto const& right) { return left < right; } );
   }


void Sorting4Districts(myData& vec_data) {
   //static const std::string special_chars = "äÄöÖüÜßáèéçñ"s;
   static const std::string special_chars = "äöüßÄÖÜáèéçñ"s;
   static const std::map<char, std::string> special_values = { {'ä',"ae"s}, {'Ä',"Ae"s },{'ü',"ue"s},{'Ü',"Ue"s},{'ß',"ss"s},
                                                               {'ö',"oe"s}, {'Ö',"Oe"s },{'á',"a"s },{'è',"e"s },{'é',"e"s },{'ç',"c"s },{'ñ',"n"s } };

   static constexpr auto normalize = [](std::string const& param, size_t pos) noexcept {
      std::string text = param;
      //size_t pos = 0;
      text.replace(pos, 1, special_values.find(text[pos++])->second);
      while ((pos = text.find_first_of(special_chars, pos)) != string::npos) {
         //auto s = special_values.find(text[pos]);
         text.replace(pos, 1, special_values.find(text[pos])->second);
         //++pos;
         }
      return text;
      };

   static constexpr auto compare = [](std::string const& left, std::string const& right) noexcept {  
      auto ret = left <=> right;
      if( ret == 0) return ret;
      auto cleft_pos  = left.find_first_of(special_chars);// != string::npos;
      auto cright_pos = right.find_first_of(special_chars);// != string::npos;
      auto cleft  = cleft_pos != string::npos;
      auto cright = cright_pos != string::npos;

      if (!cleft && cright) return left <=> normalize(right, cright_pos);
      else if (cleft && !cright)  return normalize(left, cleft_pos) <=> right;
      else if (cleft && cright)   return normalize(left, cleft_pos) <=> normalize(right, cright_pos);
      else return ret;
      };

   static auto lower = [](std::string&& strVal) {
      std::transform(execution::par, strVal.begin(), strVal.end(), strVal.begin(), [](char val) { return std::tolower(val); });
      return strVal;
      };

   static constexpr auto compare_streetnumber2 = [](string const& aSNr, string const& bSNr) noexcept {
      string::size_type szA, szB;
      int iAN = stoi(aSNr, &szA), iBN = stoi(bSNr, &szB);
      if(auto cmp = iAN <=> iBN; cmp != 0) return cmp < 0;
      //return lower(aSNr.substr(szA)) < lower(bSNr.substr(szB)) ? true : false;
      return aSNr.substr(szA) < bSNr.substr(szB) ? true : false;
      };

   sort(execution::par, vec_data.begin(), vec_data.end(), [](TData const& a, TData const& b) {
      if (auto cmp = compare(a.City(), b.City()); cmp != 0) return cmp < 0;
      if (auto cmp = compare(a.Urban_Unit(), b.Urban_Unit()); cmp != 0) return cmp < 0;
      if (auto cmp = compare(a.District(), b.District()); cmp != 0) return cmp < 0;
      if (auto cmp = a.ZipCode() <=> b.ZipCode(); cmp != 0) return cmp < 0;
      if (auto cmp = compare(a.Street(), b.Street()); cmp != 0) return cmp < 0;
      return compare_streetnumber2(a.StreetNumber(), b.StreetNumber());
      });

}

void Sorting4Distance(myData::iterator it_begin, myData::iterator it_end) {
   sort(execution::par, it_begin, it_end, [](TData const& a, TData const& b) {
         return a.Distance() == b.Distance() ? a.Angle() < b.Angle() : a.Distance() < b.Distance();
      });
   return;
}


void ReadSpots(std::string const& strFile, mySpots& vec_spots) {
   static vector<int> positions = { 72, 20, 10, 10 };

   std::locale loc("de_DE");
   ifstream ifs(strFile);
   ifs.imbue(loc);
   TSpots data;
   if (ifs.is_open()) {
      vec_spots.resize(count(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>(), '\n'));
      ifs.seekg(0, ios::beg);
      string strRow;
      int iRow = 0;
      while (getline(ifs, strRow)) {
         size_t pos = 0;
         int i = 0;
         for (auto len : positions) {
            string strPart = strRow.substr(pos, len);
            strPart.erase(0, strPart.find_first_not_of(" "));
            strPart.erase(strPart.find_last_not_of(" ") + 1, strPart.length());
            funcs_spots[i++](data, forward<string>(strPart));
            pos += len;
         }
         vec_spots[iRow++] = forward<TSpots>(data);
      }
   }
}


void SortSpots(mySpots& vec_data) {
   std::sort(vec_data.begin(), vec_data.end(), [](auto const& a, auto const& b) { return a < b; });
}


void WriteSpots(ostream& out, mySpots const& vec_spots) {
   std::for_each(vec_spots.cbegin(), vec_spots.cend(), [&out](auto const& data) { 
                   out << setw(70) << left << data.Name()
                       << setw(20) << left << data.Category()
                       << setw(10) << setprecision(6) << right << data.Latitude()
                       << setw(10) << setprecision(6) << right << data.Longitude()
                       << endl;
                   });
}

