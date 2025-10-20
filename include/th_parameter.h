#pragma once

#include <vector>
#include <TH3.h>

struct th_parameter
{
    /* data */
    std::vector<double> sigma_total;
    std::vector<std::vector<double>> delta_u;
    std::vector<std::vector<double>> delta_d;
    std::vector<double> slope_u;
    std::vector<double> slope_d;
    std::vector<double> intercept_u;
    std::vector<double> intercept_d;
};
