#pragma once

#include <iostream>
#include <string>
#include <map>
#include <tuple>

// schluessel-th.txt

namespace Geodaten {

// Land, Region, Kreis, Gemeinde, Ort, ( Ortsteil ? )
using key_type = std::tuple<int, int, int, int, int>;
using value_type = std::string;
using data_type = std::map<key_type, value_type>;

data_type read_keys(std::string const& strFile);

void Read_Th(std::string const& strKeyFile, std::string strFile, std::ostream& out = std::cout);
void Read_Bb(std::string const& strKeyFile, std::string strFile, std::ostream& out = std::cout);

}