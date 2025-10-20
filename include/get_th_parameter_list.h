#pragma once

#include <string>
#include "bin.h"
#include "th_parameter.h"

std::vector<th_parameter> get_th_parameter_list(const std::string &out_file, const std::vector<const char *> &sineff_test, std::vector<bin> Mass_bin_list);