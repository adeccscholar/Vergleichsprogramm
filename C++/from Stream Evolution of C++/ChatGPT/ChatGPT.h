#pragma once

#include "MyData.h"
#include <string>

struct TMyNum : public std::numpunct<char> {
   char_type   do_decimal_point() const { return ','; }
   char_type   do_thousands_sep() const { return '.'; }
   std::string do_grouping()      const { return "\3"; }
};

class TChatGPT {
public:
   static void read_addresses_from_file(data_vector& addresses, std::string const& strFilename);
   static void write_part_addresses_to_file(data_vector const& addresses, std::string const& strFilename, int const& count);
   static void calculate_addresses(data_vector& addresses, Location const& point);
   static void sort_DIN5007(data_vector& addresses);
   static void sort_DIN5007_Var2(data_vector& addresses);
   static void write_addresses_to_file(data_vector& addresses, std::string const& strFile);

   static void write_addresses_to_directories(const std::string& root_dir, const data_vector& addresses);
   static void write_addresses_to_directory_sorted(const std::string& root_dir, data_vector& addresses);
   static void read_addresses_from_directory(std::string directoryPath, data_vector& addresses);
   static void delete_directory(const std::string& path);

   static void delete_addresses(data_vector& addresses);

   static int  push_matching_to_front(data_vector& addresses, double matchValue);
   static void sort_addresses_in_range(data_vector& addresses, int const& endIndex);

};