#pragma once


#include <string>
#include <concepts>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

using myClock = std::chrono::high_resolution_clock;
using myTimePoint = std::chrono::time_point<std::chrono::system_clock>;

inline [[nodiscard]] std::string get_current_time_and_date(auto now) {
   auto const timew = std::chrono::current_zone()->to_local(now);
   const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
   return std::format("{:%d.%m.%Y %X},{:03d}", timew, millis.count());
}

inline [[nodiscard]] std::string get_current_timestamp(auto now) {
   return std::format("{:%Y%m%d%H%M}", std::chrono::current_zone()->to_local(now));
}

inline [[nodiscard]] std::string get_current_time_and_date(void) {
   return get_current_time_and_date(std::chrono::system_clock::now());
}

inline [[nodiscard]] std::string get_current_timestamp(void) {
   return get_current_timestamp(std::chrono::system_clock::now());
}


template <typename ty>
   requires std::floating_point<ty>
using  measurements_type = std::vector<std::vector<ty>>;


template <typename ty>
   requires std::floating_point<ty>
struct TTestData {
   std::locale              loc;
   std::string              strTest;
   std::string              strDescription;
   std::vector<std::string> captions;
   std::vector<std::string> captions_short;
   int                      iTestCases;
   fs::path                 strDirectory;
   fs::path                 strProtocol;
   fs::path                 strOverview;
   myTimePoint              start_time;
   myTimePoint              end_time;
   measurements_type<ty>    measurements;
   int                      prec;
   };


template <typename ty>
   requires std::floating_point<ty>
struct Statistical_Data {
   ty minimum = 0.0;
   ty mean = 0.0;
   ty median = 0.0;
   ty maximum = 0.0;
   ty variance = 0.0;
   ty std_deviation = 0.0;
   size_t count = 0u;
};

template <typename ty>
   requires std::floating_point<ty>
auto operator + (Statistical_Data<ty> const a, Statistical_Data<ty> const b) {
   static constexpr auto my_round = [](ty const& val) {
      return std::round(val * 1000000.0) / 1000000.0;
      };
   Statistical_Data<ty> result;
   result.minimum = my_round(my_round(a.minimum) + my_round(b.minimum));
   result.mean = my_round(my_round(a.mean) + my_round(b.mean));
   result.median = my_round(my_round(a.median) + my_round(b.median));
   result.maximum = my_round(my_round(a.maximum) + my_round(b.maximum));
   return result;
}

template <typename ty>
   requires std::floating_point<ty>
auto CalculateStats(std::vector<ty>& series) {
   Statistical_Data<ty> result;
   std::sort(series.begin(), series.end());
   result.count = series.size();
   result.minimum = *std::min_element(series.begin(), series.end());
   result.mean = std::accumulate(series.begin() + 1, series.end(), *series.begin());
   result.mean /= static_cast<ty>(result.count);
   size_t half_size = result.count / 2;
   if (series.size() % 2 == 1) result.median = series[half_size];
   else result.median = (series[half_size] + series[half_size + 1]) / 2.0;
   result.maximum = *std::max_element(series.begin(), series.end());
   result.variance = 0.0;
   for (auto const& val : series) result.variance += (val - result.mean) * (val - result.mean);
   result.variance /= (result.count - 1);
   result.std_deviation = std::sqrt(result.variance);

   return result;
   }


template <typename ty>
   requires std::floating_point<ty>
void WriteStart(std::ostream& out, TTestData<ty> test) {
   out << "\n\nstatistical analysis of the test: " << test.strTest << "\n"
      << test.strDescription << "\n\n"
      << " - directory for test:        " << test.strDirectory << "\n"
      << " - count of test runs:        " << test.iTestCases << "\n"
      << " - start time:                " << get_current_time_and_date(test.start_time) << "\n"s
      << " - detail data saved in file: \""s << test.strProtocol << "\"\n"s
      << " - overview saved in file:    \""s << test.strOverview << "\"\n"s;

   //out << std::format
}

template <typename ty>
   requires std::floating_point<ty>
void WriteProtocol(std::ostream& out, TTestData<ty> test) {
   out.setf(std::ios::fixed);
   out.setf(std::ios::showpoint);

   out << std::format("{}", *test.captions_short.cbegin());
   for (auto const& caption : std::ranges::subrange(test.captions_short.cbegin() + 1, test.captions_short.cend())) out << std::format(";{}", caption);
   out << std::endl;
   for (auto const& v : test.measurements) {
      out << std::format(test.loc, "{1:.{0}f}", test.prec, *v.cbegin());
      std::ranges::subrange w(v.cbegin() + 1, v.cend());
      for (auto const& val : w) out << std::format(test.loc, ";{1:.{0}f}", test.prec, val);
      out << std::endl;
      }
}

template <typename ty>
   requires std::floating_point<ty>
void WriteStatistic(std::ostream& out, TTestData<ty> test) {
   static auto constexpr print_caption = [](std::ostream& out) {
      out << std::format("| {0:>46}{1:>7}{0:>2}{2:>11}{0:>2}{3:>11}{0:>2}{4:>11}{0:>2}{5:>11}{0:>2}{6:>11}{0:>2}{7:>11} |\n", 
                         "|", "count", "minimum", "mean", "median", "maximum", "variance", "std_dev");
      };
   static auto constexpr print_line = [](std::ostream& out) {
      out << std::format("+ {0:->46}{0:->9}{0:->13}{0:->13}{0:->13}{0:->13}{0:->13}{1:->12}+\n", "+", "-");
      };

   auto constexpr print_data = [&test](std::ostream& out, std::string const& test_name, Statistical_Data<ty> const& stats) {
      out << std::format("| {2:<45s}{1}{3:>7d}{1:^3}{4:10.{0}f}{1:^3}{5:10.{0}f}{1:^3}{6:10.{0}f}{1:^3}{7:10.{0}f}{1:^3}{8:10.{0}f}{1:^3}{9:10.{0}f} |\n",
         test.prec, "|", test_name, stats.count, stats.minimum, stats.mean, stats.median, stats.maximum, stats.variance, stats.std_deviation);
      };

   auto constexpr print_sum = [&test](std::ostream& out, std::string const& test_name, Statistical_Data<ty> const& stats) {
      out << std::format("| {2:<45s}{1}{3:>7}{1:^3}{4:10.{0}f}{1:^3}{5:10.{0}f}{1:^3}{6:10.{0}f}{1:^3}{7:10.{0}f}{1:^3}{3:10}{1:^3}{1:>12}\n",
         test.prec, "|", test_name, " ", stats.minimum, stats.mean, stats.median, stats.maximum);
      };

   out.setf(std::ios::fixed);
   out.setf(std::ios::showpoint);

   WriteStart(out, test);
   out << " - end time:                  " << get_current_time_and_date(test.end_time) << "\n\n"s;

   print_line(out);
   print_caption(out);
   print_line(out);
   
   std::vector<std::vector<double>> random_values;
   for (int i = 0; i < test.captions.size() + 1; ++i) {
      std::vector<double> series;
      for (size_t j = 0u; j < test.measurements.size(); ++j) series.emplace_back(test.measurements[j][i]);
      random_values.emplace_back(series);
   }

   Statistical_Data<double> sum;
   for (int i = 0; auto const& test_name : test.captions) {
      const auto stats = CalculateStats(random_values[i++]);
      sum = sum + stats;
      print_data(out, test_name, stats);
      }

   auto stats = CalculateStats(random_values[test.captions.size()]);
   print_line(out);
   print_data(out, "total time", stats);
   print_sum(out, "cumulated time", sum);
   print_line(out);
   }