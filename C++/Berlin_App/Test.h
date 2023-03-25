#pragma once

#include "Data.h"
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using tplTestData   = std::tuple<std::wstring, std::wstring, std::wstring, std::wstring, double, double>;
using vecTestData   = std::vector<tplTestData>;
using tplTestResult = std::tuple<tplTestData, tplTestData, double, double>;
using vecTestResult = std::vector<tplTestResult>;

inline pairLocation to_loc(tplTestData const& val) { return std::make_pair(std::get<4>(val), std::get<5>(val)); }

void ReadTest(std::string const& strFile, vecTestData& values);
void Test(std::string const& strFile, vecTestData const& values, vecTestResult& results);
void TestAusgabe(std::wostream& out, std::string const& strFile);
void TestCalculate(std::string const& strPath);

void Helper(std::string const& strPath);