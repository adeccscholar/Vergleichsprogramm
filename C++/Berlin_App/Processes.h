#pragma once

#include "Data.h"
#include "Spots.h"
#include "statistic.h"

#include <string>
#include <filesystem>
#include <vector>
#include <iterator>


void Prepare(std::string const& strPath);
void ReadFile(std::string const& strFile, myData& vec_data);
void Read3(std::string const& strFile, myData& vec_data);
void Read2(std::string const& strFile, myData& vec_data);
void Read1(std::string const& strFile, myData& vec_data);
void Read1_templ(std::string const& strFile, myData& vec_data);
void Read(std::string const& strFile, myData& vec_data);
void Read0(std::string const& strFile, myData& vec_data);
void Statistic(std::ostream& out, myData const& vec_data);
void CleanData(myData& vec_data);
void Write_Streets_parallel(std::string const& strFile, size_t iMaxCount, myData const& vec_data);
void Write_Streets(std::string const& strFile, myData const& vec_data);
void Write_Part_of_Streets(std::string const& strFile, size_t iMaxCount, myData::const_iterator begin_it, myData::const_iterator end_it);
void Write_Part_of_Addresses(std::string const& strFile, myData::const_iterator begin_it, myData::const_iterator end_it);
void Read_Directory(std::string const& strPath, myData& vec_data);
void Clean_Directory(std::string const& strPath);
void WriteTo_Directory(std::string const& strPath, size_t iMaxCount, size_t iBorder, myData &vec_data);
void WriteTo_Directory_fast(std::string const& strPath, size_t iMaxCount, size_t iBorder, myData& vec_data);
void CalculatingDistance(std::pair<double, double> const& point, myData& vec_data);
void Partion_Distance(myData& vec_data, int iDist, myData::iterator& it);
void Sorting4Standard(myData& vec_data);
void Sorting4Districts(myData& vec_data);
void Sorting4Distance(myData::iterator it_begin, myData::iterator it_end);

void ReadSpots(std::string const& strFile, mySpots& vec_data);
void SortSpots(mySpots& vec_data);
void WriteSpots(std::ostream& out, mySpots const& vec_spots);

