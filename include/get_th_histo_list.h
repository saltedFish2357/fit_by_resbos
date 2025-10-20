#pragma once

#include <vector>
#include <string>
#include "th_histo.h"
#include "th_histo_list.h"
#include "bin.h"

void get_th_histo_list(const std::string &out_file, const std::string &th_data_dir, const std::vector<const char *> &sineff_test, bin Rapidity_bin, bin Qt_bin, std::vector<bin> Mass_bin_list);