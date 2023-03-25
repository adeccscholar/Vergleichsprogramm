#pragma once

#include "Data.h"
#include "GeoCalc.h"
#include "sort_din.h"
#include <tuple>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <execution>
#include <utility> 
#include <stdexcept>

using tplStatistic = std::tuple<double,       //  0 - minimum Latitude
                                double,       //  1 - maximum Latitude 
                                double,       //  2 . center Latitude
                                double,       //  3 - average Latitude
                                double,       //  4 - median Latitude
                                double,       //  5 - minimum Longitude
                                double,       //  6 - maximum Longitude 
                                double,       //  7 - center Longitude
                                double,       //  8 - average Longitude
                                double,       //  9 - median Longitude
                                double,       // 10 - width
                                double,       // 11 - height
                                size_t>;      // 12 - count

using tplGeoPoint = std::tuple<std::string,  // name of the point
                               std::string,  // category of the point
                               int,          // level
                               pairLocation, // geografic position
                               TData>;       // address closed to the point

using vecGeoPoints = std::vector<tplGeoPoint>;

inline std::ostream& operator << (std::ostream& out, tplGeoPoint const& data) {
   out << std::setw(30) << std::left << std::get<1>(data) << " / "
       << std::setw(70) << std::left << std::get<0>(data) << ": "
       << std::setw(14) << std::setprecision(9) << std::right << std::get<3>(data).first << ", "
       << std::setw(14) << std::setprecision(9) << std::right << std::get<3>(data).second << " -> "
       << std::setw(10) << std::setprecision(3) << std::right << std::get<4>(data).Distance() << " / "
       << std::setw( 5) << std::setprecision(1) << std::right << std::get<4>(data).Angle() << " ... "
       << std::get<4>(data);
   return out;
}

inline tplStatistic CalculatingMeasure(myData::const_iterator first, myData::const_iterator last) {
   tplStatistic retval;
   size_t data_size = std::distance(first, last);
   std::vector<pairLocation> positions;
   positions.reserve(data_size);
   double flSumLatitude = 0.0, flSumLongitude = 0.0;
   if (data_size < 1) {
      throw std::runtime_error("no data to calculate for statistic");
   }
   std::get<0>(retval) = first->Latitude();
   std::get<1>(retval) = first->Latitude();
   std::get<5>(retval) = first->Longitude();
   std::get<6>(retval) = first->Longitude();
   std::for_each(first, last, [&flSumLatitude, &flSumLongitude, &positions, &retval](auto val) {
      flSumLatitude += val.Latitude();
      flSumLongitude += val.Longitude();
      positions.emplace_back(val);
      if (val.Latitude() < std::get<0>(retval)) {
         std::get<0>(retval) = val.Latitude();
      }
      else if (val.Latitude() > std::get<1>(retval))   std::get<1>(retval) = val.Latitude();
      if (val.Longitude() < std::get<5>(retval))      std::get<5>(retval) = val.Longitude();
      else if (val.Longitude() > std::get<6>(retval)) std::get<6>(retval) = val.Longitude();
      });

   std::get<2>(retval) = (std::get<1>(retval) + std::get<0>(retval)) / 2.0;
   std::get<7>(retval) = (std::get<6>(retval) + std::get<5>(retval)) / 2.0;
   std::get<3>(retval) = flSumLatitude / data_size;
   std::get<8>(retval) = flSumLongitude / data_size;
   auto half_size = data_size / 2;
   std::sort(std::execution::par, positions.begin(), positions.end(), [](auto left, auto right) { return left.first < right.first; } );
   if (positions.size() % 2 == 1) {  // 0 1 2 3 4 5   -- 6 / 2 = 3
      std::get<4>(retval) = positions[half_size].first;
      }
   else {
      std::get<4>(retval) = (positions[half_size].first + positions[half_size - 1].first) / 2.0;
      }

   std::sort(std::execution::par, positions.begin(), positions.end(), [](auto left, auto right) { return left.second < right.second; });
   if (positions.size() % 2 == 1) {  // 0 1 2 3 4 5   -- 6 / 2 = 3
      std::get<9>(retval) = positions[half_size].second;
      }
   else {
      std::get<9>(retval) = (positions[half_size].second + positions[half_size - 1].second) / 2.0;;
      }

   auto dist = Calculate({ std::get<1>(retval), std::get<5>(retval) }, { std::get<1>(retval), std::get<6>(retval) });
   std::get<10>(retval) = dist.first;

   dist = Calculate({ std::get<1>(retval), std::get<5>(retval) }, { std::get<0>(retval), std::get<5>(retval) });
   std::get<11>(retval) = dist.first;
   std::get<12>(retval) = data_size;
   return retval;
}

inline vecGeoPoints CreateMessurePoints(std::string const& strText, tplStatistic const& messure, int iLevel) {
   vecGeoPoints points;
   points.reserve(11);
   points.emplace_back(tplGeoPoint { "northwest corner", strText, iLevel, pairLocation { get< 1>(messure), get< 5>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "north center",     strText, iLevel, pairLocation { get< 1>(messure), get< 7>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "northeast corner", strText, iLevel, pairLocation { get< 1>(messure), get< 6>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "western center",   strText, iLevel, pairLocation { get< 2>(messure), get< 5>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "eastern center",   strText, iLevel, pairLocation { get< 2>(messure), get< 6>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "southwest corner", strText, iLevel, pairLocation { get< 0>(messure), get< 5>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "south center",     strText, iLevel, pairLocation { get< 0>(messure), get< 7>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "southeast corner", strText, iLevel, pairLocation { get< 0>(messure), get< 6>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "center",           strText, iLevel, pairLocation { get< 2>(messure), get< 7>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "average",          strText, iLevel, pairLocation { get< 3>(messure), get< 8>(messure) }, TData() });
   points.emplace_back(tplGeoPoint { "median",           strText, iLevel, pairLocation { get< 4>(messure), get< 9>(messure) }, TData() });
   return points;
   }




inline void Calculating(vecGeoPoints& points, myData::const_iterator first, myData::const_iterator last) {
   //if (points.empty() || first == last) {
   //   throw std::runtime_error("vector with points or catastre is empty, method aborted!");
   //   }

   std::for_each(std::execution::par, points.begin(), points.end(), [&first, &last](auto& point) mutable {
         auto result = Calculate(std::get<3>(point), (pairLocation)*first);
         std::get<4>(point) = *first;
         std::get<4>(point) = result;
         for_each(std::execution::par, first, last, [&point, &result](auto data) {
              auto value = Calculate(std::get<3>(point), (pairLocation)data);
              if (value < result) {
                 result = value;
                 std::get<4>(point) = data; 
                 std::get<4>(point) = value; // assignment of distance, not before copied in Points
                 }
              });
         } );
   }

inline void Write_Statistic(std::ostream& out, std::string const& strText, tplStatistic const& data, vecGeoPoints& points) {
   out << strText << std::endl
      << "count:  " << std::right << std::setw(10) << std::get<12>(data) << std::endl
      << "height: " << std::right << std::setw(10) << std::setprecision(3) << std::get<11>(data) << std::endl
      << "width:  " << std::right << std::setw(10) << std::setprecision(3) << std::get<10>(data) << std::endl;
   for_each(points.cbegin(), points.cend(), [&out](auto val) { out << val << std::endl; });
   out << std::endl;
   }

inline tplStatistic Build_Statistic(std::ostream& out, std::string const& strText, vecGeoPoints& points, int iLevel, myData::iterator it_start, myData::iterator it_end) {
   auto q = CalculatingMeasure(it_start, it_end);
   auto p = CreateMessurePoints(strText, q, iLevel);
   Calculating(p, it_start, it_end);
   Write_Statistic(out, strText, q, p);
   points.insert(points.end(), std::make_move_iterator(p.begin()), std::make_move_iterator(p.end()));
   return q;
   }

inline void BuildStatisticTotal(std::ostream& out, vecGeoPoints& points, myData& vec_data) {
   auto it_start = vec_data.begin();
   auto it_end = vec_data.end();

   auto m = Build_Statistic(out, "0th total", points, 0, it_start, it_end);
   pairLocation middle = std::make_pair(std::get< 2>(m), std::get< 7>(m));

   it_end = std::partition(std::execution::par, it_start, vec_data.end(), [&middle](auto val) { return val.Latitude() >= middle.first && val.Longitude() <= middle.second; });
   Build_Statistic(out, "1st Quarter", points, 1, it_start, it_end);

   it_start = it_end;
   it_end = std::partition(std::execution::par, it_start, vec_data.end(), [&middle](auto val) { return val.Latitude() >= middle.first && val.Longitude() > middle.second; });
   Build_Statistic(out, "2nd Quarter", points, 1, it_start, it_end);

   it_start = it_end;
   it_end = std::partition(std::execution::par, it_start, vec_data.end(), [&middle](auto val) { return val.Latitude() < middle.first && val.Longitude() <= middle.second; });
   Build_Statistic(out, "3rd Quarter", points, 1, it_start, it_end);

   it_start = it_end;
   it_end = vec_data.end();
   Build_Statistic(out, "4th Quarter", points, 1, it_start, it_end);
   }

inline void BuildStatisticUrban_Unit(std::ostream& out, vecGeoPoints& points, myData& vec_data) {
   std::sort(std::execution::par, vec_data.begin(), vec_data.end(), [](auto left, auto right) { return compare_DIN(left.Urban_Unit(), right.Urban_Unit()) < 0; });
   auto it_start = vec_data.begin();
   auto it_end = vec_data.end();

   do {
      std::string strUrban_Unit = it_start->Urban_Unit();
      it_end = std::find_if_not(std::execution::par, it_start, vec_data.end(), [&strUrban_Unit](auto data) { return data.Urban_Unit() == strUrban_Unit; });
      Build_Statistic(out, strUrban_Unit, points, 2, it_start, it_end);
      it_start = it_end;
      } while (it_end != vec_data.end());
   }


inline void ReadGeoSpots(std::string const& strFile, vecGeoPoints& points) {
   static std::vector<int> positions = { 72, 20, 10, 10 };
   static std::vector<std::function<void(tplGeoPoint&, std::string&&)>> funcs = {
         [](tplGeoPoint& data, std::string&& val) { std::get<0>(data) = std::forward<std::string>(val); },
         [](tplGeoPoint& data, std::string&& val) { std::get<1>(data) = std::forward<std::string>(val); },
         [](tplGeoPoint& data, std::string&& val) { std::get<3>(data).first  = std::stod(val); },
         [](tplGeoPoint& data, std::string&& val) { std::get<3>(data).second = std::stod(val); }
   };

   std::locale loc("de_DE");
   std::ifstream ifs(strFile);
   ifs.imbue(loc);
   if (ifs.is_open()) {
      points.reserve(points.size() + std::count(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), '\n'));
      ifs.seekg(0, std::ios::beg);
      std::string strRow;
      size_t iRow = points.size();
      while (getline(ifs, strRow)) {
         tplGeoPoint point;
         std::get<2>(point) = 3;
         size_t pos = 0;
         int i = 0;
         for (auto len : positions) {
            std::string strPart = strRow.substr(pos, len);
            strPart.erase(0, strPart.find_first_not_of(" "));
            strPart.erase(strPart.find_last_not_of(" ") + 1, strPart.length());
            funcs[i++](point, std::forward<std::string>(strPart));
            pos += len;
         }
         points.emplace_back(std::forward<tplGeoPoint>(point));
      }
   }
}


inline void ReadSpotsAddPoints(std::string const& strFile, vecGeoPoints& points) {
   ReadGeoSpots(strFile, points);
   }

inline void CalculatingSpotsPoints(vecGeoPoints& points, myData& vec_data) {
   Calculating(points, vec_data.cbegin(), vec_data.cend());
   }


inline void WriteSpotsFile(std::string const& strFile, vecGeoPoints const& points) {
   static std::locale loc(std::locale("de_DE"), new TMyNum);
   std::ofstream of(strFile);
   of.imbue(loc);
   of.setf(std::ios::showpoint);
   of.setf(std::ios::fixed);
   std::for_each(points.cbegin(), points.cend(), [&of](auto val) { of << val << std::endl; });
   }
