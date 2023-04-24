#pragma once

#include "Tool_Helper.h"

#include "MyData.h"
#include "MyFileIterator.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <tuple>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <filesystem>
#include <ranges>
#include <concepts>

using namespace std::literals;
namespace fs = std::filesystem;

inline void Compare(fs::path const& strDirectory1, fs::path const& strDirectory2) {
   static auto constexpr is_directory = [](auto const& entry) { return entry.is_directory();  };

   static std::function<void(fs::path const&, std::vector<fs::path>&)> build_dirs = [](fs::path const& p, std::vector<fs::path>& dirs) {
      for(auto const& dir: fs::directory_iterator { p } | std::ranges::views::filter(is_directory)) {
         dirs.emplace_back(dir);
         build_dirs(dir, dirs);
         }
      };

   static auto build_files = [](fs::path const& p) {
      return fs::directory_iterator{ p } | std::ranges::views::filter([](fs::directory_entry const& entry) { return entry.is_regular_file(); }) |
         std::views::transform([](fs::directory_entry const& entry) { return entry.path().filename(); }) |
         std::ranges::to<std::vector>();
      };

   static auto Compare = [](fs::path const& d1, fs::path const& d2, fs::path const& subdir) {
      auto files1 = build_files(d1 / subdir);
      auto files2 = build_files(d2 / subdir);

      std::ranges::sort(files1);
      std::ranges::sort(files2);
      std::vector<fs::path> vFiles;
      std::ranges::set_intersection(files1, files2, std::back_inserter(vFiles));
      std::ranges::sort(vFiles);

      if(files1.size() > vFiles.size()) {
         std::vector<fs::path> vDiffFiles;
         std::ranges::set_difference(files1, vFiles, std::back_inserter(vDiffFiles));
         std::cout << "following files in directory " << d1 << " not in " << d2 << std::endl;
         std::ranges::for_each(vDiffFiles, [](auto const& f) { std::cout << f << std::endl; });
         }
         
      if (files2.size() > vFiles.size()) {
         std::vector<fs::path> vDiffFiles;
         std::ranges::set_difference(files2, vFiles, std::back_inserter(vDiffFiles));
         std::cout << "following files in directory " << d2 << " not in " << d1 << std::endl;
         //std::ranges::for_each(vDiffFiles, [](auto const& f) { std::cout << f << std::endl; });
         std::ranges::copy(vDiffFiles, std::ostream_iterator<fs::path>(std::cout, "\n"));

         static auto compare_files = [](fs::path const& file, fs::path const& dir, std::vector<my_line> const& lines1, std::vector<my_line> const& lines2) {
            for (auto [i, z1, z2] = std::make_tuple(1, lines1.begin(), lines2.begin()); z1 != lines1.end() && z2 != lines2.end(); ++i, ++z1, ++z2) {
               if (z1->view.compare(z2->view) != 0) [[unlikely]] {
                  std::cout << "difference in directory "s << dir << " in file "s << file << " line "s << i << std::endl
                            << z1->view << std::endl
                            << z2->view << std::endl;
                  return false;
                  }
               }
            return true;
            };
      for(auto const& file : vFiles) {
         std::string buffer1, buffer2;
         auto lines1 = my_lines{ GetContent(d1 / subdir / file, buffer1) } | std::ranges::to<std::vector<my_line>>();
         auto lines2 = my_lines{ GetContent(d2 / subdir / file, buffer2) } | std::ranges::to<std::vector<my_line>>();
         compare_files(file, subdir, lines1, lines2);
      }

      }

   };

   }


inline bool Compare_Input(fs::path const& strDirectory1, fs::path const& strDirectory2) {

   static auto compare_files = [](fs::path const& file, std::vector<my_line> const& lines1, std::vector<my_line> const& lines2) {
      for (auto [i, z1, z2] = std::make_tuple(1, lines1.begin(), lines2.begin()); z1 != lines1.end() && z2 != lines2.end(); ++i, ++z1, ++z2) {
         if (z1->view.compare(z2->view) != 0) [[unlikely]] {
            std::cout << std::format("difference in file {} at line {}\n{}\n{}\n", file.string(), i, z1->view, z2->view);
            return false;
         }
      }
      return true;
   };

   // check files in root
   bool boRetVal = true;
   for (auto const& file : { "berlin_infos.dat", "berlin_spots.txt" }) {
      std::string strBuffer1, strBuffer2;
      auto view1 = GetContent(strDirectory1 / file, strBuffer1);
      auto view2 = GetContent(strDirectory2 / file, strBuffer2);
      size_t size = std::ranges::count(view1, '\n');
      std::vector<my_line> lines1;
      lines1.reserve(size);
      my_lines file_data1(view1);
      std::ranges::copy(file_data1, std::back_inserter(lines1));

      std::vector<my_line> lines2;
      lines2.reserve(size);
      my_lines file_data2(view2);
      std::ranges::copy(file_data2, std::back_inserter(lines2));

      if (!compare_files(file, lines1, lines2)) boRetVal = false;

   }

   return boRetVal;
}

inline bool Compare_Output(fs::path const& strDirectory1, fs::path const& strDirectory2) {
   static auto find_directories = [](fs::path const& p) {
      return fs::directory_iterator{ p } |
         std::views::filter([](auto const& entry) { return entry.is_directory(); }) |
         std::views::transform([](auto const& entry) { return entry.path().filename();  }) |
         std::ranges::to<std::vector>();
   };

   static auto compare_files = [](fs::path const& file, std::vector<std::string_view> const& lines1, std::vector<std::string_view> const& lines2) {
      for (auto [i, z1, z2] = std::make_tuple(1, lines1.begin(), lines2.begin()); z1 != lines1.end() && z2 != lines2.end(); ++i, ++z1, ++z2) {
         if (z1->compare(*z2) != 0) [[unlikely]] {
            std::cout << std::format("difference in file {} at line {}\n{}\n{}\n", file.string(), i, *z1, *z2);
            return false;
         }
      }
      if (lines1.size() != lines2.size()) [[unlikely]] {
         std::cout << std::format("file {} is incompatible, size is different.\n", file.string());
         return false;
      }
      else return true;
   };


   bool boRetVal = true;
   for (auto const& file : { "testausgabe.txt", "testausgabe_alle.txt" }) {
      std::string strBuffer1, strBuffer2;
      auto lines1 = my_lines{ GetContent(strDirectory1 / file, strBuffer1) } | std::ranges::to<std::vector>();
      auto lines2 = my_lines{ GetContent(strDirectory2 / file, strBuffer2) } | std::ranges::to<std::vector>();
      if (!compare_files(file, lines1, lines2)) boRetVal = false;
      }

   auto directories1 = find_directories(strDirectory1);
   auto directories2 = find_directories(strDirectory2);

   std::ranges::sort(directories1);
   std::ranges::sort(directories2);

   std::vector<fs::path> vDirectories;
   std::ranges::set_intersection(directories1, directories2, std::back_inserter(vDirectories));

   std::ranges::sort(vDirectories);

   if (directories1.size() > vDirectories.size() || directories2.size() > vDirectories.size()) {
      std::cout << std::format("directories in root {} and {} are incompatible\n", strDirectory1.string(), strDirectory2.string());
      for (auto const& dir : { std::make_pair(directories1, strDirectory1), std::make_pair(directories2, strDirectory2) }) {
         if (dir.first.size() > vDirectories.size()) {
            std::cout << std::format("directories only in {}\n", dir.second.string());
            std::ranges::set_difference(dir.first, vDirectories, std::ostream_iterator<fs::path>(std::cout, "\n"));
         }
      }
      boRetVal = false;
   }
   // Compare 
   std::ranges::for_each(vDirectories, [&strDirectory1, &strDirectory2](auto const& d) { Compare(strDirectory1 / d, strDirectory2 / d); });
   return boRetVal;
}