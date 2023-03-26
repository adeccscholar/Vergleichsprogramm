#pragma once

#include "MyData.h"
#include <string>

class TChatGPT {
public:
   static void read_from_File(data_vector& addresses, std::string const& strFilename);
   static void write_to_File(data_vector const& addresses, std::string const& strFilename, int count = -1);
   static void calculate_addresses(data_vector& addresses, Location const& point);
   static void sort_DIN5007(data_vector& addresses);
   static void sort_DIN5007_Var2(data_vector& addresses);

   static void write_addresses_to_directories(const std::string& root_dir, const data_vector& addresses);
   static void write_addresses_to_directory_sorted(const std::string& root_dir, data_vector& addresses);
   static void read_addresses_from_directory(std::string directoryPath, data_vector& addresses);
   static void delete_directory(const std::string& path);

   static void delete_addresses(data_vector& addresses);

   static int  pushMatchingToFront(data_vector& addresses, double matchValue);
   static void sortAddressesInRange(data_vector& addresses, int endIndex);

};