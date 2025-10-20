#include <ROOT/RDataFrame.hxx>
#include <utility>
#include <cmath>
#include <TH1.h>
#include "bin.h"
#include "AnalysisUtils.h"
#include "BinUtils.h"
#include "get_exp_Afb_list.h"

std::vector<exp_parameter> get_exp_Afb_list(ROOT::RDataFrame exp_df, bin Rapidity_bin, bin Qt_bin, std::vector<bin> Mass_bin_list)
{
    std::vector<exp_parameter> exp_Afb_list;
    // 预分配内存但不使用resize，避免后续push_back时的重复分配
    exp_Afb_list.reserve(Mass_bin_list.size());

    // 优化数据处理流程，减少中间变量的创建
    auto filtered_exp_df = exp_df
                               .Define("Rapidity", [](const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E)
                                       { return MyAnalysis::Utils::get_Rapidity(Z_Px, Z_Py, Z_Pz, Z_E); }, {"ZPx", "ZPy", "ZPz", "ZE"})
                               .Filter([Rapidity_bin](const double &Rapidity)
                                       { return Rapidity_bin.is_in_bin(std::abs(Rapidity)); },
                                       {"Rapidity"}, "Rapidity_cut")
                               .Define("Qt", [](const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E)
                                       { return MyAnalysis::Utils::get_Qt(Z_Px, Z_Py, Z_Pz, Z_E); }, {"ZPx", "ZPy", "ZPz", "ZE"})
                               .Filter([Qt_bin](const double &Qt)
                                       { return Qt_bin.is_in_bin(Qt); },
                                       {"Qt"}, "Qt_cut")
                               .Define("Z_M", [](const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E)
                                       { return MyAnalysis::Utils::get_M(Z_Px, Z_Py, Z_Pz, Z_E); }, {"ZPx", "ZPy", "ZPz", "ZE"})
                               .Filter([Mass_bin_list](const double &Z_M)
                                       { return Z_M > Mass_bin_list[0].lower() && Z_M < Mass_bin_list.back().upper(); },
                                       {"Z_M"}, "Z_M_cut")
                               .Define("cos_Theta_h", [](const double &Z_Pz, const double &Z_M, const double &Qt, const double &Lepton_Pz, const double &AntiLepton_Pz, const double &Lepton_E, const double &AntiLepton_E)
                                       { return MyAnalysis::Utils::get_cos_Theta_h(Z_Pz, Z_M, Lepton_Pz, Qt,
                                                                                   AntiLepton_Pz, Lepton_E, AntiLepton_E); }, {"ZPz", "Z_M", "Qt", "LeptonPz", "AntiLeptonPz", "LeptonE", "AntiLeptonE"});

    auto report_of_filtered_exp_df = filtered_exp_df.Report();

    // 使用智能指针管理直方图，避免内存泄漏
    std::unique_ptr<TH1D> forward_hist = nullptr;
    std::unique_ptr<TH1D> backward_hist = nullptr;
    std::unique_ptr<TH1D> Afb_hist = nullptr;

    // 获取质量区间的边界，避免重复计算
    auto mass_edges = BinUtils::to_edges(Mass_bin_list);

    // 分别处理正向和反向数据
    auto forward_hist_df = filtered_exp_df
                               .Filter([](double cos_Theta_h)
                                       { return cos_Theta_h > 0; },
                                       {"cos_Theta_h"}, "forward_cut")
                               .Histo1D({"exp_forward_hist", "exp_sigma_forward;Z_Mass(GeV);Counts",
                                         Mass_bin_list.size(), mass_edges.data()},
                                        "Z_M");

    auto backward_hist_df = filtered_exp_df
                                .Filter([](double cos_Theta_h)
                                        { return cos_Theta_h < 0; },
                                        {"cos_Theta_h"}, "backward_cut")
                                .Histo1D({"exp_backward_hist", "exp_sigma_backward;Z_Mass(GeV);Counts",
                                          Mass_bin_list.size(), mass_edges.data()},
                                         "Z_M");

    // 获取直方图指针并转移所有权到智能指针
    forward_hist.reset(static_cast<TH1D *>(forward_hist_df.GetPtr()->Clone("forward_hist_clone")));
    backward_hist.reset(static_cast<TH1D *>(backward_hist_df.GetPtr()->Clone("backward_hist_clone")));
    Afb_hist.reset(MyAnalysis::Utils::get_Afb_hist(forward_hist.get(), backward_hist.get(), "exp_Afb_hist"));

    // 提取结果到exp_Afb_list
    for (int i = 1; i <= Afb_hist->GetNbinsX(); i++)
    {
        exp_Afb_list.emplace_back(Afb_hist->GetBinContent(i), Afb_hist->GetBinError(i));
    }

    // 使用RAII模式管理文件资源
    {
        // 使用绝对路径确保文件位置正确
        TFile exp_Afb("/Data/xyzhou/fitByResBos/output/exp_Afb.root", "UPDATE");
        if (exp_Afb.IsOpen())
        {
            forward_hist->Write();
            backward_hist->Write();
            Afb_hist->Write();
            // 文件会在作用域结束时自动关闭
        }
        else
        {
            std::cerr << "Warning: Could not open output file for writing!" << std::endl;
        }
    }

    report_of_filtered_exp_df->Print();

    return exp_Afb_list;
}