#include "MyFit.h"

namespace MyAnalysis
{
    namespace MyFit
    {
        Chi2FCN_in_linear_fit::Chi2FCN_in_linear_fit(const std::vector<double> &x,
                                                     const std::vector<double> &y,
                                                     const std::vector<double> &sigma)
            : x_(x), y_(y), sigma_(sigma) {}

        double Chi2FCN_in_linear_fit::operator()(const std::vector<double> &par) const
        {
            double a = par[0];
            double b = par[1];
            double chi2 = 0.0;

            for (size_t i = 0; i < x_.size(); i++)
            {
                double model_val = a * x_[i] + b;
                double residual = y_[i] - model_val;

                double weighted_residual = residual / sigma_[i];

                chi2 += weighted_residual * weighted_residual;
            }
            return chi2;
        }

        double Chi2FCN_in_linear_fit::Up() const
        {
            return 1.0;
        }
        std::pair<double, double> linear_fit(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &sigma)
        {
            Chi2FCN_in_linear_fit fcn(x, y, sigma);

            ROOT::Minuit2::MnUserParameters upar;
            upar.Add("slope", 0.0, 0.1);
            upar.Add("intercept", 0.0, 0.001);

            ROOT::Minuit2::MnMigrad migrad(fcn, upar);
            ROOT::Minuit2::FunctionMinimum min = migrad();

            if (min.IsValid())
            {
                double slope = min.UserParameters().Value("slope");
                double intercept = min.UserParameters().Value("intercept");
                return std::make_pair(slope, intercept);
            }

            return std::pair<double, double>(0.0, 0.0);
        }

        result_col fit_sineff_and_pu_pd(const std::vector<exp_parameter> &exp_Afb_list, const std::vector<th_parameter> &th_parameter_list)
        {
            // 预先提取并缓存实验数据值和误差，避免在循环中重复访问
            std::vector<double> exp_Afb_values;
            std::vector<double> exp_Afb_errors;
            exp_Afb_values.reserve(exp_Afb_list.size());
            exp_Afb_errors.reserve(exp_Afb_list.size());
            
            for (const auto& exp_Afb : exp_Afb_list)
            {
                exp_Afb_values.push_back(exp_Afb.Afb);
                exp_Afb_errors.push_back(exp_Afb.Afb_error);
            }
            
            // 预分配结果列表内存
            std::vector<result_col> result_list;
            result_list.reserve(th_parameter_list[0].delta_d.size());
            
            // 预先提取并缓存理论参数数据，减少循环中的重复内存分配
            size_t param_size = th_parameter_list.size();
            std::vector<std::vector<double>> th_slope_u(param_size);
            std::vector<std::vector<double>> th_intercept_u(param_size);
            std::vector<std::vector<double>> th_slope_d(param_size);
            std::vector<std::vector<double>> th_intercept_d(param_size);
            std::vector<std::vector<double>> th_sigma_total(param_size);
            
            for (size_t i = 0; i < param_size; ++i)
            {
                th_slope_u[i] = th_parameter_list[i].slope_u;
                th_intercept_u[i] = th_parameter_list[i].intercept_u;
                th_slope_d[i] = th_parameter_list[i].slope_d;
                th_intercept_d[i] = th_parameter_list[i].intercept_d;
                th_sigma_total[i] = th_parameter_list[i].sigma_total;
            }
            
            // 对每个误差索引进行拟合
            for (size_t error_idx = 0; error_idx < th_parameter_list[0].delta_d.size(); ++error_idx)
            {
                // 只提取当前误差索引所需的delta值
                std::vector<std::vector<double>> th_delta_u(param_size);
                std::vector<std::vector<double>> th_delta_d(param_size);
                
                for (size_t i = 0; i < param_size; ++i)
                {
                    th_delta_u[i] = th_parameter_list[i].delta_u[error_idx];
                    th_delta_d[i] = th_parameter_list[i].delta_d[error_idx];
                }
                
                // 创建拟合函数对象
                Chi2FCN_in_sineff_fit fcn(exp_Afb_values,
                                          exp_Afb_errors,
                                          th_slope_u,
                                          th_intercept_u,
                                          th_slope_d,
                                          th_intercept_d,
                                          th_sigma_total,
                                          th_delta_u,
                                          th_delta_d);
                ROOT::Minuit2::MnUserParameters upar;
                upar.Add("sineff", 0.23, 0.001);
                upar.Add("p_u", 0.0, 0.0001);
                upar.Add("p_d", 0.0, 0.0001);
                ROOT::Minuit2::MnMigrad migrad(fcn, upar);
                ROOT::Minuit2::FunctionMinimum min = migrad();

                if (min.IsValid())
                {
                    const ROOT::Minuit2::MnUserCovariance &cov = min.UserCovariance();
                    int sineff_index = min.UserParameters().Index("sineff");
                    int pu_index = min.UserParameters().Index("p_u");
                    int pd_index = min.UserParameters().Index("p_d");

                    result_col result;
                    result.sineff = min.UserParameters().Value("sineff");
                    result.sineff_exp_error = min.UserParameters().Error("sineff");
                    result.p_u = min.UserParameters().Value("p_u");
                    result.p_u_exp_error = min.UserParameters().Error("p_u");
                    result.p_d = min.UserParameters().Value("p_d");
                    result.p_d_exp_error = min.UserParameters().Error("p_d");
                    
                    // 计算相关性系数
                    if (result.sineff_exp_error > 0 && result.p_u_exp_error > 0)
                        result.correlation_sineff_pu = cov(sineff_index, pu_index) / (result.sineff_exp_error * result.p_u_exp_error);
                    if (result.sineff_exp_error > 0 && result.p_d_exp_error > 0)
                        result.correlation_sineff_pd = cov(sineff_index, pd_index) / (result.sineff_exp_error * result.p_d_exp_error);
                    if (result.p_u_exp_error > 0 && result.p_d_exp_error > 0)
                        result.correlation_pu_pd = cov(pu_index, pd_index) / (result.p_u_exp_error * result.p_d_exp_error);

                    result_list.push_back(std::move(result)); // 使用移动语义减少复制
                }
            }

            double delta_sineff = 0.0;
            double delta_pu = 0.0;
            double delta_pd = 0.0;
            for (int i = 1; i < (result_list.size() - 1) / 2; i++)
            {
                delta_sineff += pow(result_list[2 * i].sineff - result_list[2 * i - 1].sineff, 2);
                delta_pu += pow(result_list[2 * i].p_u - result_list[2 * i - 1].p_u, 2);
                delta_pd += pow(result_list[2 * i].p_d - result_list[2 * i - 1].p_d, 2);
            }
            delta_sineff = sqrt(delta_sineff) / 2;
            delta_pu = sqrt(delta_pu) / 2;
            delta_pd = sqrt(delta_pd) / 2;
            result_col result = result_list[0];
            result.sineff_delta_error = delta_sineff;
            result.p_u_delta_error = delta_pu;
            result.p_d_delta_error = delta_pd;

            return result;
        }

        Chi2FCN_in_sineff_fit::Chi2FCN_in_sineff_fit(const std::vector<double> &exp_Afb_values,
                                                     const std::vector<double> &exp_Afb_errors,
                                                     const std::vector<std::vector<double>> &th_slope_u,
                                                     const std::vector<std::vector<double>> &th_intercept_u,
                                                     const std::vector<std::vector<double>> &th_slope_d,
                                                     const std::vector<std::vector<double>> &th_intercept_d,
                                                     const std::vector<std::vector<double>> &th_sigma_total,
                                                     const std::vector<std::vector<double>> &th_delta_u,
                                                     const std::vector<std::vector<double>> &th_delta_d)
            : exp_Afb_values_(exp_Afb_values),
              exp_Afb_errors_(exp_Afb_errors),
              th_slope_u_(th_slope_u),
              th_intercept_u_(th_intercept_u),
              th_slope_d_(th_slope_d),
              th_intercept_d_(th_intercept_d),
              th_sigma_total_(th_sigma_total),
              th_delta_u_(th_delta_u),
              th_delta_d_(th_delta_d) {}

        double Chi2FCN_in_sineff_fit::operator()(const std::vector<double> &par) const
        {
            const double sineff = par[0];
            const double pu = par[1];
            const double pd = par[2];

            double chi2 = 0.0;
            const size_t n_exp = exp_Afb_values_.size();
            
            // 展开内循环以减少嵌套循环的开销
            for (size_t i = 0; i < n_exp; ++i)
            {
                double th_Afb = 0.0;
                double sigma_total = 0.0;
                const size_t n_th = th_slope_u_[i].size();
                
                // 使用预计算和局部变量缓存来加速循环
                const auto& slope_u_i = th_slope_u_[i];
                const auto& intercept_u_i = th_intercept_u_[i];
                const auto& slope_d_i = th_slope_d_[i];
                const auto& intercept_d_i = th_intercept_d_[i];
                const auto& sigma_total_i = th_sigma_total_[i];
                const auto& delta_u_i = th_delta_u_[i];
                const auto& delta_d_i = th_delta_d_[i];
                
                for (size_t j = 0; j < n_th; ++j)
                {
                    const double th_Afb_u = slope_u_i[j] * sineff + intercept_u_i[j];
                    const double th_Afb_d = slope_d_i[j] * sineff + intercept_d_i[j];
                    const double sigma = sigma_total_i[j];
                    th_Afb += sigma * (th_Afb_u * (delta_u_i[j] + pu) + th_Afb_d * (delta_d_i[j] + pd));
                    sigma_total += sigma;
                }
                
                if (sigma_total > 0) { // 避免除零错误
                    th_Afb /= sigma_total;
                    const double residual = exp_Afb_values_[i] - th_Afb;
                    const double error = exp_Afb_errors_[i];
                    if (error > 0) { // 避免除零错误
                        const double weighted_residual = residual / error;
                        chi2 += weighted_residual * weighted_residual;
                    }
                }
            }
            return chi2;
        }
        double Chi2FCN_in_sineff_fit::Up() const
        {
            return 1.0;
        }
    }

}