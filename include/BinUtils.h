#pragma once

#include <vector>
#include "bin.h"

namespace BinUtils{
    std::vector<double> to_edges(const std::vector<bin> bins);
    std::vector<bin> create_bin_list_from_array(const std::vector<double> bin_array);
}