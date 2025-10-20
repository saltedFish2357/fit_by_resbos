#pragma once

#include "th_histo.h"
#include "bin.h"
#include <ROOT/RDataFrame.hxx>

void get_th_forward_backward_histo(const std::string &flavor, TFile *root_file, std::vector<ROOT::RDataFrame> df, bin Rapidity_bin, bin Qt_bin, bin Mass_bin, int mass_bin_idx, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col, const std::string &costhe_q, const std::string &weight_col);