#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <functional>

#include "Data.h"
#include "Spots.h"

extern std::string strPath;
extern std::string strFile_data;
extern std::string strFile_data_out;
extern std::string strFile_dist_out;
extern std::string strFile_spots;
extern std::string strFile_spots_out;
extern std::string strFile_points_out;
extern std::string strFile_control;

extern size_t iBufferPages1, iBufferPages2, iBufferPages3;

extern std::vector<std::function<void(TData&, std::string_view const&)>> funcs_read_f;
extern std::vector<std::function<void(TData&, std::string_view const&)>> funcs_read_d;
extern std::vector<std::function<void(TSpots&, std::string&&)>> funcs_spots;
