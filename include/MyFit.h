#pragma once

#include <vector>
#include <Minuit2/FCNBase.h>
#include <Minuit2/MnUserParameters.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/FunctionMinimum.h>
#include "exp_parameter.h"
#include "th_parameter.h"
#include "result_col.h"

namespace MyAnalysis
{
    namespace MyFit
    {
        class Chi2FCN_in_linear_fit : public ROOT::Minuit2::FCNBase
        {
        public:
            Chi2FCN_in_linear_fit(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &sigma);
            double operator()(const std::vector<double> &par) const override;
            double Up() const override;

        private:
            const std::vector<double> &x_;
            const std::vector<double> &y_;
            const std::vector<double> &sigma_;
        };

        std::pair<double, double> linear_fit(const std::vector<double> &x,
                                             const std::vector<double> &y,
                                             const std::vector<double> &sigma);

        class Chi2FCN_in_sineff_fit : public ROOT::Minuit2::FCNBase
        {
        public:
            Chi2FCN_in_sineff_fit(const std::vector<double> &exp_Afb_values,
                                  const std::vector<double> &exp_Afb_errors,
                                  const std::vector<std::vector<double>> &th_slope_u,
                                  const std::vector<std::vector<double>> &th_intercept_u,
                                  const std::vector<std::vector<double>> &th_slope_d,
                                  const std::vector<std::vector<double>> &th_intercept_d,
                                  const std::vector<std::vector<double>> &th_sigma_total,
                                  const std::vector<std::vector<double>> &th_delta_u,
                                  const std::vector<std::vector<double>> &th_delta_d);
            double operator()(const std::vector<double> &par) const override;
            double Up() const override;

        private:
            const std::vector<double> &exp_Afb_values_;
            const std::vector<double> &exp_Afb_errors_;
            const std::vector<std::vector<double>> &th_slope_u_;
            const std::vector<std::vector<double>> &th_intercept_u_;
            const std::vector<std::vector<double>> &th_slope_d_;
            const std::vector<std::vector<double>> &th_intercept_d_;
            const std::vector<std::vector<double>> &th_sigma_total_;
            const std::vector<std::vector<double>> &th_delta_u_;
            const std::vector<std::vector<double>> &th_delta_d_;
        };

        result_col fit_sineff_and_pu_pd(const std::vector<exp_parameter> &exp_Afb_list,
                                        const std::vector<th_parameter> &th_parameter_list);
    }
}