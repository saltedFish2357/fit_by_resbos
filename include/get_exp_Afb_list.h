#pragma once

#include <ROOT/RDataFrame.hxx>
#include "bin.h"
#include "exp_parameter.h"

std::vector<exp_parameter> get_exp_Afb_list(ROOT::RDataFrame exp_df, bin Rapidity_bin, bin Qt_bin, std::vector<bin> Mass_bin_list);