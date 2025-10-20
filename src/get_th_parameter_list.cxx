#include "get_th_parameter_list.h"
#include "AnalysisUtils.h"
#include "MyFit.h"
#include <TFile.h>
#include <TH3.h>

std::vector<th_parameter> get_th_parameter_list(const std::string &out_file, const std::vector<const char *> &sineff_test, std::vector<bin> Mass_bin_list)
{
    // Implementation goes here
    std::vector<th_parameter> th_parameter_list;

    TFile *root_file = TFile::Open(out_file.c_str(), "READ");
    if (!root_file || root_file->IsZombie())
    {
        throw std::runtime_error("Failed to open ROOT file: " + out_file);
    }

    // Loop over Mass_bin_list and extract th_parameter for each bin
    for (const auto &mass_bin : Mass_bin_list)
    {
        int mass_bin_idx = &mass_bin - &Mass_bin_list[0];
        th_parameter param;
        // Fill param with data from root_file based on Rapidity_bin, Qt_bin, and mass_bin
        const std::string &histo_name_total = "total_" + std::to_string(mass_bin_idx) + "_total";
        TH3D *total_hist = root_file->Get<TH3D>(histo_name_total.c_str());
        param.sigma_total.reserve(total_hist->GetNbinsX() * total_hist->GetNbinsY() * total_hist->GetNbinsZ());
        for (int ix = 1; ix <= total_hist->GetNbinsX(); ++ix)
        {
            for (int iy = 1; iy <= total_hist->GetNbinsY(); ++iy)
            {
                for (int iz = 1; iz <= total_hist->GetNbinsZ(); ++iz)
                {
                    double sigma_total = total_hist->GetBinContent(ix, iy, iz);
                    param.sigma_total.emplace_back(sigma_total);

                    param.delta_u.resize(59);
                    param.delta_d.resize(59);
                    for (int error_idx = 0; error_idx < 59; error_idx++)
                    {

                        std::string u_correct_name = "u_massbin_" + std::to_string(mass_bin_idx) + "_correct_" + std::to_string(error_idx);
                        std::string u_wrong_name = "u_massbin_" + std::to_string(mass_bin_idx) + "_wrong_" + std::to_string(error_idx);
                        TH3D *u_correct_hist = root_file->Get<TH3D>(u_correct_name.c_str());
                        TH3D *u_wrong_hist = root_file->Get<TH3D>(u_wrong_name.c_str());
                        double u_correct = u_correct_hist->GetBinContent(ix, iy, iz);
                        double u_wrong = u_wrong_hist->GetBinContent(ix, iy, iz);
                        double c_u = (u_correct + u_wrong) != 0 ? (u_correct - u_wrong) / (u_correct + u_wrong) : 0.0;
                        param.delta_u[error_idx].emplace_back(c_u);

                        std::string d_correct_name = "d_massbin_" + std::to_string(mass_bin_idx) + "_correct_" + std::to_string(error_idx);
                        std::string d_wrong_name = "d_massbin_" + std::to_string(mass_bin_idx) + "_wrong_" + std::to_string(error_idx);
                        TH3D *d_correct_hist = root_file->Get<TH3D>(d_correct_name.c_str());
                        TH3D *d_wrong_hist = root_file->Get<TH3D>(d_wrong_name.c_str());
                        double d_correct = d_correct_hist->GetBinContent(ix, iy, iz);
                        double d_wrong = d_wrong_hist->GetBinContent(ix, iy, iz);
                        double c_d = (d_correct + d_wrong) != 0 ? (d_correct - d_wrong) / (d_correct + d_wrong) : 0.0;
                        param.delta_d[error_idx].emplace_back(c_d);
                    }

                    std::vector<double> sineff_test_values;
                    for (const auto &sineff : sineff_test)
                    {
                        sineff_test_values.emplace_back(std::stod(sineff));
                    }
                    std::vector<double> u_Afb_values;
                    std::vector<double> u_Afb_error_values;
                    for (int sineff_idx = 0; sineff_idx < sineff_test.size(); sineff_idx++)
                    {
                        const std::string u_forward_histo_name = "u_massbin_" + std::to_string(mass_bin_idx) + "_forward_" + std::to_string(sineff_idx);
                        const std::string u_backward_histo_name = "u_massbin_" + std::to_string(mass_bin_idx) + "_backward_" + std::to_string(sineff_idx);
                        TH3D *u_forward_hist = root_file->Get<TH3D>(u_forward_histo_name.c_str());
                        TH3D *u_backward_hist = root_file->Get<TH3D>(u_backward_histo_name.c_str());
                        TH3D *u_Afb_hist = MyAnalysis::Utils::get_Afb_hist(u_forward_hist, u_backward_hist, "u_Afb_hist", "u_Afb_hist");
                        double u_Afb = u_Afb_hist->GetBinContent(ix, iy, iz);
                        double u_Afb_error = u_Afb_hist->GetBinError(ix, iy, iz);
                        u_Afb_values.emplace_back(u_Afb);
                        u_Afb_error_values.emplace_back(u_Afb_error);
                    }
                    // Perform linear fit to get slope and intercept for u quark
                    auto [slope_u, intercept_u] = MyAnalysis::MyFit::linear_fit(sineff_test_values, u_Afb_values, u_Afb_error_values);
                    param.slope_u.emplace_back(slope_u);
                    param.intercept_u.emplace_back(intercept_u);

                    std::vector<double> d_Afb_values;
                    std::vector<double> d_Afb_error_values;
                    for (int sineff_idx = 0; sineff_idx < sineff_test.size(); sineff_idx++)
                    {
                        const std::string d_forward_histo_name = "d_massbin_" + std::to_string(mass_bin_idx) + "_forward_" + std::to_string(sineff_idx);
                        const std::string d_backward_histo_name = "d_massbin_" + std::to_string(mass_bin_idx) + "_backward_" + std::to_string(sineff_idx);
                        TH3D *d_forward_hist = root_file->Get<TH3D>(d_forward_histo_name.c_str());
                        TH3D *d_backward_hist = root_file->Get<TH3D>(d_backward_histo_name.c_str());
                        TH3D *d_Afb_hist = MyAnalysis::Utils::get_Afb_hist(d_forward_hist, d_backward_hist, "d_Afb_hist", "d_Afb_hist");
                        double d_Afb = d_Afb_hist->GetBinContent(ix, iy, iz);
                        double d_Afb_error = d_Afb_hist->GetBinError(ix, iy, iz);
                        d_Afb_values.emplace_back(d_Afb);
                        d_Afb_error_values.emplace_back(d_Afb_error);
                    }
                    // Perform linear fit to get slope and intercept for d quark
                    auto [slope_d, intercept_d] = MyAnalysis::MyFit::linear_fit(sineff_test_values, d_Afb_values, d_Afb_error_values);
                    param.slope_d.emplace_back(slope_d);
                    param.intercept_d.emplace_back(intercept_d);
                }
            }
        }

        for (int error_idx = 0; error_idx < 59; error_idx++)
        {
            double p_u = 0.0;
            double volume_total = 0.0;
            for (int ix = 1; ix <= total_hist->GetNbinsX(); ++ix)
            {
                for (int iy = 1; iy <= total_hist->GetNbinsY(); ++iy)
                {
                    for (int iz = 1; iz <= total_hist->GetNbinsZ(); ++iz)
                    {
                        double volume = total_hist->GetXaxis()->GetBinWidth(ix) * total_hist->GetYaxis()->GetBinWidth(iy) * total_hist->GetZaxis()->GetBinWidth(iz);
                        p_u += param.delta_u[error_idx][(ix - 1) * total_hist->GetNbinsY() * total_hist->GetNbinsZ() + (iy - 1) * total_hist->GetNbinsZ() + (iz - 1)] * volume;
                        volume_total += volume;
                    }
                }
            }
            p_u /= volume_total;
            for (auto &delta_u_val : param.delta_u[error_idx])
            {
                delta_u_val -= p_u;
            }

            double p_d = 0.0;
            for (int ix = 1; ix <= total_hist->GetNbinsX(); ++ix)
            {
                for (int iy = 1; iy <= total_hist->GetNbinsY(); ++iy)
                {
                    for (int iz = 1; iz <= total_hist->GetNbinsZ(); ++iz)
                    {
                        double volume = total_hist->GetXaxis()->GetBinWidth(ix) * total_hist->GetYaxis()->GetBinWidth(iy) * total_hist->GetZaxis()->GetBinWidth(iz);
                        p_d += param.delta_d[error_idx][(ix - 1) * total_hist->GetNbinsY() * total_hist->GetNbinsZ() + (iy - 1) * total_hist->GetNbinsZ() + (iz - 1)] * volume;
                    }
                }
            }
            p_d /= volume_total;
            for (auto &delta_d_val : param.delta_d[error_idx])
            {
                delta_d_val -= p_d;
            }
            th_parameter_list.emplace_back(param);
        }
    }

    // Fill th_parameter_list based on the provided parameters and out_file
    return th_parameter_list;
}