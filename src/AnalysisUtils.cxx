#include <TLorentzVector.h>
#include <string>
#include <TH1D.h>
#include "AnalysisUtils.h"

// 字符串表达式接口实现
std::string MyAnalysis::Utils::get_Rapidity_expr(const std::string &Px, const std::string &Py, const std::string &Pz, const std::string &E)
{
    return "0.5 * log((" + E + " + " + Pz + ")/(" + E + " - " + Pz + "))";
}

std::string MyAnalysis::Utils::get_Qt_expr(const std::string &Px, const std::string &Py)
{
    return "sqrt(" + Px + "*" + Px + " + " + Py + "*" + Py + ")";
}

std::string MyAnalysis::Utils::get_M_expr(const std::string &Px, const std::string &Py, const std::string &Pz, const std::string &E)
{
    return "sqrt(" + E + "*" + E + " - (" + Px + "*" + Px + " + " + Py + "*" + Py + " + " + Pz + "*" + Pz + "))";
}

std::string MyAnalysis::Utils::get_cos_Theta_h_expr(const std::string &Z_Pz, const std::string &M, const std::string &Qt, const std::string &Lepton_Pz, const std::string &AntiLepton_Pz, const std::string &Lepton_E, const std::string &AntiLepton_E)
{
    std::string a = "(" + Lepton_E + " + " + Lepton_Pz + ")/sqrt(2)";
    std::string b = "(" + Lepton_E + " - " + Lepton_Pz + ")/sqrt(2)";
    std::string c = "(" + AntiLepton_E + " + " + AntiLepton_Pz + ")/sqrt(2)";
    std::string d = "(" + AntiLepton_E + " - " + AntiLepton_Pz + ")/sqrt(2)";
    std::string numerator = "2*(" + a + "*" + d + " - " + b + "*" + c + ")";
    std::string denominator = M + "*sqrt(" + M + "*" + M + " + " + Qt + "*" + Qt + ")";
    return "((" + Z_Pz + ") > 0 ? (" + numerator + ")/" + denominator + " : -(" + numerator + ")/" + denominator + ")";
}

std::string MyAnalysis::Utils::calculate_Afb_expr(const std::string &sigma_forward, const std::string &sigma_backward)
{
    return "(" + sigma_forward + " - " + sigma_backward + ")/(" + sigma_forward + " + " + sigma_backward + ")";
}

std::string MyAnalysis::Utils::get_x1_expr(const std::string &Rapidity, const std::string &M, const std::string &Q_t, const std::string &s)
{
    return "sqrt(" + M + "*" + M + " + " + Q_t + "*" + Q_t + ")*exp(" + Rapidity + ")/sqrt(" + s + ")";
}

std::string MyAnalysis::Utils::get_x2_expr(const std::string &Rapidity, const std::string &M, const std::string &Q_t, const std::string &s)
{
    return "sqrt(" + M + "*" + M + " + " + Q_t + "*" + Q_t + ")*exp(-" + Rapidity + ")/sqrt(" + s + ")";
}

double MyAnalysis::Utils::get_Rapidity(const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E)
{
    TLorentzVector v_Z(Z_Px, Z_Py, Z_Pz, Z_E);
    return v_Z.Rapidity();
}

double MyAnalysis::Utils::get_Qt(const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E)
{
    TLorentzVector v_Z(Z_Px, Z_Py, Z_Pz, Z_E);
    return v_Z.Pt();
}

double MyAnalysis::Utils::get_M(const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E)
{
    TLorentzVector v_Z(Z_Px, Z_Py, Z_Pz, Z_E);
    return v_Z.M();
}

double MyAnalysis::Utils::get_cos_Theta_h(const double &Z_Pz, const double &M, const double &Qt, const double &Lepton_Pz, const double &AntiLepton_Pz, const double &Lepton_E, const double &AntiLepton_E)
{
    double a = (Lepton_E + Lepton_Pz) / sqrt(2);
    double b = (Lepton_E - Lepton_Pz) / sqrt(2);
    double c = (AntiLepton_E + AntiLepton_Pz) / sqrt(2);
    double d = (AntiLepton_E - AntiLepton_Pz) / sqrt(2);
    double cos_Theta_h;

    if (Z_Pz > 0)
    {
        cos_Theta_h = 2 * (a * d - b * c) / M / sqrt(M * M + Qt * Qt);
    }
    else if (Z_Pz < 0)
    {
        cos_Theta_h = -2 * (a * d - b * c) / M / sqrt(M * M + Qt * Qt);
    }

    return cos_Theta_h;
}

std::pair<double, double> MyAnalysis::Utils::calculate_Afb(const double &sigma_forward, const double &sigma_backward, const double &error_f, const double &error_b)
{
    double total = sigma_forward + sigma_backward;
    if (total == 0)
        return std::make_pair(0, 0);

    double Afb = (sigma_forward - sigma_backward) / total;

    double dAFBdf = 2 * sigma_backward / pow(total, 2);
    double dAFBdb = -2 * sigma_forward / pow(total, 2);
    double Afb_error = sqrt(pow(dAFBdf * error_f, 2) + pow(dAFBdb * error_b, 2));

    return std::make_pair(Afb, Afb_error);
}

TH3D *MyAnalysis::Utils::get_Afb_hist(const TH3D *forward_hist, const TH3D *backward_hist, const std::string &name, const std::string &title)
{
    TH3D *Afb_hist = dynamic_cast<TH3D *>(forward_hist->Clone(name.c_str()));
    Afb_hist->Reset();
    Afb_hist->SetTitle(title.c_str());

    for (int ix = 1; ix <= forward_hist->GetNbinsX(); ix++)
    {
        for (int iy = 1; iy <= forward_hist->GetNbinsY(); iy++)
        {
            for (int iz = 1; iz <= forward_hist->GetNbinsZ(); iz++)
            {
                double sigma_f = forward_hist->GetBinContent(ix, iy, iz);
                double sigma_b = backward_hist->GetBinContent(ix, iy, iz);
                double error_f = forward_hist->GetBinError(ix, iy, iz);
                double error_b = backward_hist->GetBinError(ix, iy, iz);

                auto [Afb, Afb_error] = MyAnalysis::Utils::calculate_Afb(sigma_f, sigma_b, error_f, error_b);

                Afb_hist->SetBinContent(ix, iy, iz, Afb);
                Afb_hist->SetBinError(ix, iy, iz, Afb_error);
            }
        }
    }
    return Afb_hist;
}

TH1D *MyAnalysis::Utils::get_Afb_hist(const TH1D *forward_hist, const TH1D *backward_hist, const std::string &name, const std::string &title)
{
    TH1D *Afb_hist = dynamic_cast<TH1D *>(forward_hist->Clone(name.c_str()));
    Afb_hist->Reset();
    Afb_hist->SetTitle(title.c_str());

    for (int i = 1; i <= forward_hist->GetNbinsX(); i++)
    {
        double Afb = MyAnalysis::Utils::calculate_Afb(forward_hist->GetBinContent(i), backward_hist->GetBinContent(i), forward_hist->GetBinError(i), backward_hist->GetBinError(i)).first;
        double Afb_error = MyAnalysis::Utils::calculate_Afb(forward_hist->GetBinContent(i), backward_hist->GetBinContent(i), forward_hist->GetBinError(i), backward_hist->GetBinError(i)).second;
        Afb_hist->SetBinContent(i, Afb);
        Afb_hist->SetBinError(i, Afb_error);
    }
    return Afb_hist;
}
