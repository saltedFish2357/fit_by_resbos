#include <iostream>
#include "get_th_total_histo.h"
#include "ParticleAnalysis.h"
#include "generate_unique_name.h"

void get_th_total_histo(TFile *root_file,
                        th_histo &output_histo,
                        ROOT::RDataFrame total_df,
                        bin Rapidity_bin, bin Qt_bin, bin Mass_bin,
                        int mass_bin_idx,
                        const std::string &px_col, const std::string &py_col,
                        const std::string &pz_col, const std::string &e_col,
                        const std::string &weight_col)
{

    int rap_bins = Rapidity_bin.get_adaptive_bins();
    int qt_bins = Qt_bin.get_adaptive_bins();
    int mass_bins = Mass_bin.get_adaptive_bins();

    auto total_with_partical_cut = MyAnalysis::ParticleAnalysis::apply_particle_cut_expr(
        total_df, Rapidity_bin, Qt_bin, Mass_bin, px_col, py_col, pz_col, e_col); // testing

    std::string total_name = std::string("total_hist") + std::to_string(mass_bin_idx);

    auto total_histo = total_with_partical_cut
                           .Histo3D(
                               {total_name.c_str(), "total_histogram",
                                rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                                qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                                mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                               "Rapidity", "Qt", "Mass", weight_col);

    auto total_hist = std::make_shared<TH3D>(*total_histo.GetPtr());
    output_histo.total = total_hist;

    std::string histo_name = "total_" + std::to_string(mass_bin_idx);
    output_histo.save(root_file, histo_name);
    // ...existing code...

    // ...existing code...
}