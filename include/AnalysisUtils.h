#pragma once

#include <vector>
#include <TH1.h>
#include <ROOT/RDataFrame.hxx>
#include "bin.h"
#include <string>

namespace MyAnalysis
{
    namespace Utils
    {
        double get_Rapidity(const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E);
        double get_Qt(const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E);
        double get_M(const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E);

        double get_cos_Theta_h(const double &Z_Pz, const double &M, const double &Qt, const double &Lepton_Pz, const double &AntiLepton_Pz, const double &Lepton_E, const double &AntiLepton_E);

        std::pair<double, double> calculate_Afb(const double &sigma_forward, const double &sigma_backward, const double &error_f, const double &error_b);

        TH3D *get_Afb_hist(const TH3D *forward_hist, const TH3D *backward_hist, const std::string &name = "Afb_hist", const std::string &title = "Forward-Backward Asymetry;Rapidity;Qt;Mass");

        TH1D *get_Afb_hist(const TH1D *forward_hist, const TH1D *backward_hist, const std::string &name = "Afb_hist", const std::string &title = "Forward-Backward Asymetry;Mass(GeV);A_{FB}");

        // 字符串表达式接口，便于RDataFrame直接识别
        std::string get_Rapidity_expr(const std::string &Px, const std::string &Py, const std::string &Pz, const std::string &E);
        std::string get_Qt_expr(const std::string &Px, const std::string &Py);
        std::string get_M_expr(const std::string &Px, const std::string &Py, const std::string &Pz, const std::string &E);
        std::string get_cos_Theta_h_expr(const std::string &Z_Pz, const std::string &M, const std::string &Qt, const std::string &Lepton_Pz, const std::string &AntiLepton_Pz, const std::string &Lepton_E, const std::string &AntiLepton_E);
        std::string calculate_Afb_expr(const std::string &sigma_forward, const std::string &sigma_backward);
        std::string get_x1_expr(const std::string &Rapidity, const std::string &M, const std::string &Q_t, const std::string &s = "13000.0*13000.0");
        std::string get_x2_expr(const std::string &Rapidity, const std::string &M, const std::string &Q_t, const std::string &s = "13000.0*13000.0");
    }
}