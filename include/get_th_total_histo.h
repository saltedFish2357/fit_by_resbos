#pragma once
#include "th_histo.h"
#include <vector>
#include <string>
#include <ROOT/RDataFrame.hxx>
#include "bin.h"
#include "th_col.h"
#include "th_histo_list.h"

void get_th_total_histo(TFile *root_file,
                        th_histo &output_histo,
                        ROOT::RDataFrame total_df,
                        bin Rapidity_bin, bin Qt_bin, bin Mass_bin,
                        int mass_bin_idx,
                        const std::string &px_col, const std::string &py_col,
                        const std::string &pz_col, const std::string &e_col,
                        const std::string &weight_col);