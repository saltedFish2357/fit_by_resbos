#include "get_th_histo_list.h"
#include "th_histo.h"
#include "th_histo_list.h"
#include "get_th_dilution_histo.h"
#include "th_col.h"
#include "get_th_forward_backward_histo.h"
#include "get_th_total_histo.h"
#include <ROOT/RDataFrame.hxx>
#include <TFile.h>

void get_th_histo_list(const std::string &out_file, const std::string &th_data_dir, const std::vector<const char *> &sineff_test, bin Rapidity_bin, bin Qt_bin, std::vector<bin> Mass_bin_list)
{
    std::vector<const char *> event_type = {"zd_bbarb", "zd_dbard", "zd_sbars", "zu_cbarc", "zu_ubaru", "zd_bbbar", "zd_ddbar", "zd_ssbar", "zu_ccbar", "zu_uubar"};

    int Mass_bin_list_length = Mass_bin_list.size();

    std::vector<std::vector<std::string>> th_df_path_list;
    th_df_path_list.reserve(sineff_test.size());

    for (int sineff_idx = 0; sineff_idx < sineff_test.size(); sineff_idx++)
    {
        std::vector<std::string> inner_list;
        inner_list.reserve(event_type.size());

        for (int event_type_idx = 0; event_type_idx < event_type.size(); event_type_idx++)
        {
            std::string file_path = "/Data/xyzhou/sineffcheck/SINT2W0." +
                                    std::string(sineff_test[sineff_idx]) +
                                    "/CT18NNLO.00_" +
                                    std::string(event_type[event_type_idx]) +
                                    "_BLNY/40_350GeV/merged_resbos.root";

            inner_list.emplace_back(std::move(file_path));
        }
        th_df_path_list.push_back(std::move(inner_list));
    }

    th_col col_list = {"Px_V", "Py_V", "Pz_V", "E_V", "costhe_q", "WT00"};
    const std::string &tree_name = "h10";

    // 打开一次输出 ROOT 文件以提升 I/O 效率
    // const std::string file_prefix = "th_histo";
    TFile *root_file = TFile::Open(out_file.c_str(), "UPDATE");
    if (!root_file || root_file->IsZombie())
    {
        throw std::runtime_error("Failed to open or create ROOT file: " + out_file);
    }

    // 在每个Mass_bin下进行循环
    for (int mass_bin_idx = 0; mass_bin_idx < Mass_bin_list_length; mass_bin_idx++)
    {

        // 处理u

        std::vector<ROOT::RDataFrame> u_df_list;
        u_df_list.reserve(sineff_test.size());
        for (int sineff_idx = 0; sineff_idx < sineff_test.size(); sineff_idx++)
        {
            u_df_list.emplace_back(
                tree_name,
                std::vector<std::string>{
                    th_df_path_list[sineff_idx][9],
                    th_df_path_list[sineff_idx][4]},
                std::vector<std::string>{col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.costhe_q_col, col_list.weight});
        }

        get_th_forward_backward_histo("u", root_file, u_df_list, Rapidity_bin, Qt_bin, Mass_bin_list[mass_bin_idx], mass_bin_idx, col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.costhe_q_col, col_list.weight);

        ROOT::RDataFrame u_ubar_df(tree_name, th_df_path_list[3][9], {col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.weight});
        ROOT::RDataFrame ubar_u_df(tree_name, th_df_path_list[3][4], {col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.weight});

        get_th_dilution_histo("u", root_file, u_ubar_df, ubar_u_df, Rapidity_bin, Qt_bin, Mass_bin_list[mass_bin_idx], mass_bin_idx, col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.weight);

        // 处理d

        std::vector<ROOT::RDataFrame> d_df_list;
        d_df_list.reserve(sineff_test.size());
        for (int sineff_idx = 0; sineff_idx < sineff_test.size(); sineff_idx++)
        {
            d_df_list.emplace_back(
                tree_name,
                std::vector<std::string>{
                    th_df_path_list[sineff_idx][6],
                    th_df_path_list[sineff_idx][1]},
                std::vector<std::string>{col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.costhe_q_col, col_list.weight});
        }

        get_th_forward_backward_histo("d", root_file, d_df_list, Rapidity_bin, Qt_bin, Mass_bin_list[mass_bin_idx], mass_bin_idx, col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.costhe_q_col, col_list.weight);

        ROOT::RDataFrame d_dbar_df(tree_name, th_df_path_list[3][6], {col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.weight});
        ROOT::RDataFrame dbar_d_df(tree_name, th_df_path_list[3][1], {col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.weight});

        get_th_dilution_histo("d", root_file, d_dbar_df, dbar_d_df, Rapidity_bin, Qt_bin, Mass_bin_list[mass_bin_idx], mass_bin_idx, col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.weight);

        // 添加total event
        th_histo th_total_histo;

        ROOT::RDataFrame total_df(tree_name,
                                  std::vector<std::string>{
                                      th_df_path_list[3][0],
                                      // th_df_path_list[3][1],
                                      th_df_path_list[3][2],
                                      th_df_path_list[3][3],
                                      // th_df_path_list[3][4],
                                      th_df_path_list[3][5],
                                      // th_df_path_list[3][6],
                                      th_df_path_list[3][7],
                                      th_df_path_list[3][8],
                                      // th_df_path_list[3][9]
                                  },
                                  std::vector<std::string>{col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.weight});

        TH3D *u_histo_total = root_file->Get<TH3D>(("u_massbin_" + std::to_string(mass_bin_idx) + "_total").c_str());
        TH3D *d_histo_total = root_file->Get<TH3D>(("d_massbin_" + std::to_string(mass_bin_idx) + "_total").c_str());
        th_total_histo.total = std::make_shared<TH3D>(*u_histo_total);
        th_total_histo.total->Add(d_histo_total);

        get_th_total_histo(root_file, th_total_histo, total_df, Rapidity_bin, Qt_bin, Mass_bin_list[mass_bin_idx], mass_bin_idx, col_list.px_col, col_list.py_col, col_list.pz_col, col_list.e_col, col_list.weight);
    }
    // 关闭并删除一次性打开的 ROOT 文件
    root_file->Close();
    delete root_file;
}