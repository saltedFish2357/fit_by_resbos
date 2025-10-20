#include <iostream>
#include "get_th_forward_backward_histo.h"
#include "ParticleAnalysis.h"
#include "generate_unique_name.h"

void get_th_forward_backward_histo(const std::string &flavor, TFile *root_file,
                                   std::vector<ROOT::RDataFrame> df_list,
                                   bin Rapidity_bin, bin Qt_bin, bin Mass_bin,
                                   int mass_bin_idx,
                                   const std::string &px_col, const std::string &py_col,
                                   const std::string &pz_col, const std::string &e_col,
                                   const std::string &costhe_q, const std::string &weight_col)
{
    th_histo output_histo;

    int rap_bins = Rapidity_bin.get_adaptive_bins();
    int qt_bins = Qt_bin.get_adaptive_bins();
    int mass_bins = Mass_bin.get_adaptive_bins();

    // 使用resize预分配
    output_histo.forward.resize(df_list.size());
    output_histo.backward.resize(df_list.size());

    for (size_t index = 0; index < df_list.size(); ++index)
    {
        // const auto &df = df_list[index];
        ROOT::RDF::RNode df_with_partical_cut =
            MyAnalysis::ParticleAnalysis::apply_particle_cut_expr(
                df_list[index], Rapidity_bin, Qt_bin, Mass_bin,
                px_col, py_col, pz_col, e_col); // testing

        // 使用 mass_bin_idx 和 index 构造确定性名称（线程安全）
        const std::string mass_suffix = std::to_string(mass_bin_idx);
        std::string forward_name = std::string("forward_hist_") + mass_suffix + "_" + std::to_string(index);
        std::string backward_name = std::string("backward_hist_") + mass_suffix + "_" + std::to_string(index);

        auto forward_histo = df_with_partical_cut
                                 .Filter(costhe_q + " >= 0")
                                 .Histo3D(
                                     {forward_name.c_str(), "forward_histogram",
                                      rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                                      qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                                      mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                                     "Rapidity", "Qt", "Mass", weight_col);

        auto backward_histo = df_with_partical_cut
                                  .Filter(costhe_q + " < 0")
                                  .Histo3D(
                                      {backward_name.c_str(), "backward_histogram",
                                       rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                                       qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                                       mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                                      "Rapidity", "Qt", "Mass", weight_col);

        auto forward_hist = std::make_shared<TH3D>(*forward_histo.GetPtr());
        output_histo.forward[index] = forward_hist;

        auto backward_hist = std::make_shared<TH3D>(*backward_histo.GetPtr());
        output_histo.backward[index] = backward_hist;

        if (index == 3)
        {
            const std::string &total_name = std::string("total_hist_") + mass_suffix;

            // 直接拷贝构造，避免手动内存管理
            output_histo.total = std::make_shared<TH3D>(*forward_hist);
            output_histo.total->SetName(total_name.c_str());

            // 添加 backward_hist
            if (backward_hist)
            {
                output_histo.total->Add(backward_hist.get());
            }
        }

        output_histo.save(root_file, flavor + "_massbin_" + mass_suffix);
    }
}