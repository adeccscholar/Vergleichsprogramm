
#include "Processes_Data.h"
#include "Data.h"

#include <stdexcept>

std::string strPath;
std::string strFile_data;
std::string strFile_data_out;
std::string strFile_dist_out;
std::string strFile_spots;
std::string strFile_spots_out;
std::string strFile_points_out;
std::string strFile_control;

size_t iBufferPages1, iBufferPages2, iBufferPages3;

std::vector<std::function<void(TData&, std::string_view const&)>> funcs_read_f = {
      [](TData& data, std::string_view const& val) { data.City(val); },
      [](TData& data, std::string_view const& val) { data.Street(val); },
      [](TData& data, std::string_view const& val) { data.StreetNumber(val); },
      [](TData& data, std::string_view const& val) { data.ZipCode(val); },
      [](TData& data, std::string_view const& val) { data.Urban_Unit(val); },
      [](TData& data, std::string_view const& val) { data.Old_Unit(val); },
      [](TData& data, std::string_view const& val) { data.District(val); },
      [](TData& data, std::string_view const& val) { data.Latitude(val); },
      [](TData& data, std::string_view const& val) { data.Longitude(val); },
      [](TData& data, std::string_view const& val) { throw std::runtime_error("file corruped"); }
};

std::vector<std::function<void(TData&, std::string_view const&)>> funcs_read_d = {
      [](TData& data, std::string_view const& val) { data.StreetNumber(val); },
      [](TData& data, std::string_view const& val) { data.ZipCode(val); },
      [](TData& data, std::string_view const& val) { data.Old_Unit(val); },
      [](TData& data, std::string_view const& val) { data.Latitude(val); },
      [](TData& data, std::string_view const& val) { data.Longitude(val); },
      [](TData& data, std::string_view const& val) { data.Distance(val); },
      [](TData& data, std::string_view const& val) { data.Angle(val); },
      [](TData& data, std::string_view const& val) { throw std::runtime_error("file corruped"); }
};

std::vector<std::function<void(TSpots&, std::string&&)>> funcs_spots = {
      [](TSpots& data, std::string&& val) { data.Name(std::forward<std::string>(val)); },
      [](TSpots& data, std::string&& val) { data.Category(std::forward<std::string>(val)); },
      [](TSpots& data, std::string&& val) { data.Latitude(std::forward<std::string>(val)); },
      [](TSpots& data, std::string&& val) { data.Longitude(std::forward<std::string>(val)); }
};

