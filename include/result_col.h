#pragma once

struct result_col
{
    /* data */
    double sineff;
    double sineff_exp_error;
    double sineff_delta_error;
    double p_u;
    double p_u_exp_error;
    double p_u_delta_error;
    double p_d;
    double p_d_exp_error;
    double p_d_delta_error;
    double correlation_sineff_pu;
    double correlation_sineff_pd;
    double correlation_pu_pd;
};
