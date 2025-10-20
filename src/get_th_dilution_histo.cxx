#include <iostream>
#include "get_th_dilution_histo.h"
#include "bin.h"
#include "ParticleAnalysis.h"
#include "AnalysisUtils.h"
#include <string>
#include <LHAPDF/LHAPDF.h>

void get_th_dilution_histo(const std::string &flavor, TFile *root_file,
                           ROOT::RDataFrame f_fbar_df,
                           ROOT::RDataFrame fbar_f_df,
                           bin Rapidity_bin, bin Qt_bin, bin Mass_bin,
                           int mass_bin_idx,
                           const std::string &px_col, const std::string &py_col,
                           const std::string &pz_col, const std::string &e_col,
                           const std::string &weight_col)
{
    th_histo output_histo;
    output_histo.correct.resize(59);
    output_histo.wrong.resize(59);

    // 计算每个维度的 bin 数量（保持原有逻辑，别删）
    int rap_bins = Rapidity_bin.get_adaptive_bins();
    int qt_bins = Qt_bin.get_adaptive_bins();
    int mass_bins = Mass_bin.get_adaptive_bins();

    // 预先应用粒子分析切割
    auto f_fbar_with_cut = MyAnalysis::ParticleAnalysis::apply_particle_cut_expr(
        f_fbar_df, Rapidity_bin, Qt_bin, Mass_bin, px_col, py_col, pz_col, e_col); // use same range as total

    auto fbar_f_with_cut = MyAnalysis::ParticleAnalysis::apply_particle_cut_expr(
        fbar_f_df, Rapidity_bin, Qt_bin, Mass_bin, px_col, py_col, pz_col, e_col); // use same range as total

    // 使用 mass_bin_idx 构造线程安全的名称后缀
    const std::string mass_suffix = std::to_string(mass_bin_idx);

    // 分别处理两个数据集 - f_fbar数据集
    std::string p_p_name = std::string("p_p_histo_") + mass_suffix + "0";
    auto p_p_histo = f_fbar_with_cut
                         .Filter(pz_col + " >= 0")
                         .Histo3D(
                             {p_p_name.c_str(),
                              "positive_positive_histogram",
                              rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                              qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                              mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                             "Rapidity", "Qt", "Mass", weight_col);

    std::string p_n_name = std::string("p_n_histo_") + mass_suffix;
    std::string n_p_name = std::string("n_p_histo_") + mass_suffix;
    /* std::string p_n_name = std::string("p_n_histo_") + mass_suffix;
    auto p_n_histo = f_fbar_with_cut
                         .Filter(pz_col + " < 0")
                         .Histo3D(
                             {p_n_name.c_str(),
                              "positive_negative_histogram",
                              rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                              qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                              mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                             "Rapidity", "Qt", "Mass", weight_col);

    // 分别处理两个数据集 - fbar_f数据集
    std::string n_p_name = std::string("n_p_histo_") + mass_suffix;
    auto n_p_histo = fbar_f_with_cut
                         .Filter(pz_col + " >= 0")
                         .Histo3D(
                             {n_p_name.c_str(),
                              "negative_positive_histogram",
                              rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                              qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                              mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                             "Rapidity", "Qt", "Mass", weight_col); */

    std::string n_n_name = std::string("n_n_histo_") + mass_suffix + "0";
    auto n_n_histo = fbar_f_with_cut
                         .Filter(pz_col + " < 0")
                         .Histo3D(
                             {n_n_name.c_str(),
                              "negative_negative_histogram",
                              rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                              qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                              mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                             "Rapidity", "Qt", "Mass", weight_col);

    // 合并结果
    output_histo.correct[0] = std::make_shared<TH3D>(*p_p_histo.GetPtr());
    output_histo.correct[0]->Add(n_n_histo.GetPtr());

    /* auto wrong_hist = std::make_shared<TH3D>(*p_n_histo.GetPtr());
    wrong_hist->Add(n_p_histo.GetPtr());
    output_histo.wrong = wrong_hist; */

    TH3D *total_histo = root_file->Get<TH3D>((flavor + "_massbin_" + mass_suffix + "_total").c_str());
    output_histo.wrong[0] = std::make_shared<TH3D>(*total_histo);
    output_histo.wrong[0]->Add(output_histo.correct[0].get(), -1.0);

    output_histo.save(root_file, flavor + "_massbin_" + mass_suffix);

    output_histo.correct[0] = nullptr;
    output_histo.wrong[0] = nullptr;

    const LHAPDF::PDFSet pdf_set("CT18NNLO");
    LHAPDF::PDF *old_pdf = pdf_set.mkPDF(0);

    int quark_id;
    if (flavor == "d")
        quark_id = 1;
    else if (flavor == "u")
        quark_id = 2;
    else
    {
        std::cerr << "Unsupported flavor: " << flavor << std::endl;
        return;
    }

    auto f_fbar_oldx1x2 = f_fbar_with_cut
                              .Define("x1", MyAnalysis::Utils::get_x1_expr("Rapidity", "Mass", "Qt"))
                              .Define("x2", MyAnalysis::Utils::get_x2_expr("Rapidity", "Mass", "Qt"))
                              .Define("old_pdf1", [old_pdf, quark_id](double x1, float Q)
                                      { return old_pdf->xfxQ(quark_id, x1, static_cast<double>(Q)); },
                                      {"x1", "Mass"})
                              .Define("old_pdf2", [old_pdf, quark_id](double x2, float Q)
                                      { return old_pdf->xfxQ(-quark_id, x2, static_cast<double>(Q)); },
                                      {"x2", "Mass"})
                              .Define("old_pdf_product", "old_pdf1 * old_pdf2");

    auto fbar_f_oldx1x2 = fbar_f_with_cut
                              .Define("x1", MyAnalysis::Utils::get_x1_expr("Rapidity", "Mass", "Qt"))
                              .Define("x2", MyAnalysis::Utils::get_x2_expr("Rapidity", "Mass", "Qt"))
                              .Define("old_pdf1", [old_pdf, quark_id](double x1, float Q)
                                      { return old_pdf->xfxQ(-quark_id, x1, static_cast<double>(Q)); },
                                      {"x1", "Mass"})
                              .Define("old_pdf2", [old_pdf, quark_id](double x2, float Q)
                                      { return old_pdf->xfxQ(quark_id, x2, static_cast<double>(Q)); },
                                      {"x2", "Mass"})
                              .Define("old_pdf_product", "old_pdf1 * old_pdf2");
    for (int error_idx = 1; error_idx <= 58; error_idx++)
    {
        LHAPDF::PDF *new_pdf = pdf_set.mkPDF(error_idx);
        auto f_fbar_with_cut_reweight = f_fbar_oldx1x2
                                            .Define("new_pdf1", [new_pdf, quark_id](double x1, float Q)
                                                    { return new_pdf->xfxQ(quark_id, x1, static_cast<double>(Q)); },
                                                    {"x1", "Mass"})
                                            .Define("new_pdf2", [new_pdf, quark_id](double x2, float Q)
                                                    { return new_pdf->xfxQ(-quark_id, x2, static_cast<double>(Q)); },
                                                    {"x2", "Mass"})
                                            .Define("new_pdf_product", "new_pdf1 * new_pdf2")
                                            .Define("reweight", "WT00 * new_pdf_product / old_pdf_product");
        auto fbar_f_with_cut_reweight = fbar_f_oldx1x2
                                            .Define("new_pdf1", [new_pdf, quark_id](double x1, float Q)
                                                    { return new_pdf->xfxQ(-quark_id, x1, static_cast<double>(Q)); },
                                                    {"x1", "Mass"})
                                            .Define("new_pdf2", [new_pdf, quark_id](double x2, float Q)
                                                    { return new_pdf->xfxQ(quark_id, x2, static_cast<double>(Q)); },
                                                    {"x2", "Mass"})
                                            .Define("new_pdf_product", "new_pdf1 * new_pdf2")
                                            .Define("reweight", "WT00 * new_pdf_product / old_pdf_product");

        const std::string &weight_col = "reweight";

        auto p_p_histo = f_fbar_with_cut_reweight
                             .Filter(pz_col + " >= 0")
                             .Histo3D(
                                 {(p_p_name + std::to_string(error_idx)).c_str(),
                                  "positive_positive_histogram",
                                  rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                                  qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                                  mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                                 "Rapidity", "Qt", "Mass", weight_col);

        auto p_n_histo = f_fbar_with_cut_reweight
                             .Filter(pz_col + " < 0")
                             .Histo3D(
                                 {(p_n_name + std::to_string(error_idx)).c_str(),
                                  "positive_positive_histogram",
                                  rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                                  qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                                  mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                                 "Rapidity", "Qt", "Mass", weight_col);

        auto n_p_histo = fbar_f_with_cut_reweight
                             .Filter(pz_col + " >= 0")
                             .Histo3D(
                                 {(n_p_name + std::to_string(error_idx)).c_str(),
                                  "positive_positive_histogram",
                                  rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                                  qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                                  mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                                 "Rapidity", "Qt", "Mass", weight_col);

        auto n_n_histo = fbar_f_with_cut_reweight
                             .Filter(pz_col + " < 0")
                             .Histo3D(
                                 {(n_n_name + std::to_string(error_idx)).c_str(),
                                  "positive_positive_histogram",
                                  rap_bins, Rapidity_bin.lower(), Rapidity_bin.upper(),
                                  qt_bins, Qt_bin.lower(), Qt_bin.upper(),
                                  mass_bins, Mass_bin.lower(), Mass_bin.upper()},
                                 "Rapidity", "Qt", "Mass", weight_col);

        output_histo.correct[error_idx] = std::make_shared<TH3D>(*p_p_histo.GetPtr());
        output_histo.correct[error_idx]->Add(n_n_histo.GetPtr());

        output_histo.wrong[error_idx] = std::make_shared<TH3D>(*p_n_histo.GetPtr());
        output_histo.wrong[error_idx]->Add(n_p_histo.GetPtr());

        output_histo.save(root_file, flavor + "_massbin_" + mass_suffix);
    }
}