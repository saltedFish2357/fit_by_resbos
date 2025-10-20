#include <ROOT/RDataFrame.hxx>
#include <vector>
#include <iostream>
#include <chrono>
#include <memory>
#include "bin.h"
#include "BinUtils.h"
#include "get_exp_Afb_list.h"
#include "get_th_histo_list.h"
#include "th_parameter.h"
#include "th_histo_list.h"
#include "get_th_parameter_list.h"
#include "result_col.h"
#include "MyFit.h"
#include "exp_parameter.h"

int main()
{
    // 记录开始时间，用于性能监控
    auto start_time = std::chrono::high_resolution_clock::now();

    // 设置多线程数量，根据实际硬件优化
    ROOT::EnableImplicitMT();

    // 输出线程使用情况
    // std::cout << "Using " << ROOT::GetImplicitMTPoolSize() << " threads for parallel processing" << std::endl;

    // 定义数据处理的参数
    const bin Rapidity_bin(0, 1);
    const bin Qt_bin(0, 5);
    const std::vector<double> bin_array = {40, 50, 60, 76, 86, 96, 106, 120, 133, 155, 171, 200, 320};
    const std::vector<bin> Mass_bin_list = BinUtils::create_bin_list_from_array(bin_array);

    // 输出数据配置信息
    std::cout << "Mass bins: " << Mass_bin_list.size() << std::endl;

    const std::vector<const char *> sineff_test = {"2300", "2305", "2310", "2315", "2320", "2325", "2330"};

    // 定义文件路径常量
    const std::string data_dir = "/Data/xyzhou/fitSin2ThetaW1/data/";
    const std::string output_dir = "/Data/xyzhou/fitByResBos/output/";
    const std::string th_data_dir = "/Data/xyzhou/sineffcheck_test";

    // 使用智能指针管理TChain，避免内存泄漏
    std::unique_ptr<TChain> exp_chain = std::make_unique<TChain>("ZeeTree");
    exp_chain->Add((data_dir + "test*.root").c_str());

    // 只读取需要的列，减少内存使用
    const std::vector<std::string> needed_columns = {"ZPx", "ZPy", "ZPz", "ZE",
                                                     "LeptonPz", "AntiLeptonPz",
                                                     "LeptonE", "AntiLeptonE"};

    // 创建RDataFrame并只选择需要的列
    ROOT::RDataFrame exp_df(*exp_chain, needed_columns);

    std::cout << "Starting data processing..." << std::endl;

    // 处理实验数据
    std::cout << "Processing experimental data..." << std::endl;
    auto exp_data_start = std::chrono::high_resolution_clock::now();

    std::vector<exp_parameter> exp_Afb_list = get_exp_Afb_list(exp_df, Rapidity_bin, Qt_bin, Mass_bin_list);

    auto exp_data_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> exp_data_time = exp_data_end - exp_data_start;
    std::cout << "Experimental data processing completed in " << exp_data_time.count() << " seconds" << std::endl;
    std::cout << "Experimental data points: " << exp_Afb_list.size() << std::endl;

    // 处理理论数据
    std::cout << "Processing theoretical data..." << std::endl;
    auto th_data_start = std::chrono::high_resolution_clock::now();

    const std::string out_file = output_dir + "th_histo.root";
    get_th_histo_list(out_file, th_data_dir, sineff_test, Rapidity_bin, Qt_bin, Mass_bin_list);

    auto th_data_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> th_data_time = th_data_end - th_data_start;
    std::cout << "Theoretical data processing completed in " << th_data_time.count() << " seconds" << std::endl;

    // 获取理论参数列表
    std::vector<th_parameter> th_parameter_list = get_th_parameter_list(out_file, sineff_test, Mass_bin_list);
    std::cout << "Theoretical parameter list size: " << th_parameter_list.size() << std::endl;

    // 执行拟合
    std::cout << "Starting fit process..." << std::endl;
    auto fit_start = std::chrono::high_resolution_clock::now();

    result_col result = MyAnalysis::MyFit::fit_sineff_and_pu_pd(exp_Afb_list, th_parameter_list);

    auto fit_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> fit_time = fit_end - fit_start;
    std::cout << "Fit completed in " << fit_time.count() << " seconds" << std::endl;

    // 输出结果
    std::cout << "\nFit Results:" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Fitted sineff: " << result.sineff << " +/- " << result.sineff_exp_error
              << " +/- " << result.sineff_delta_error << std::endl;
    std::cout << "Fitted p_u: " << result.p_u << " +/- " << result.p_u_exp_error
              << " +/- " << result.p_u_delta_error << std::endl;
    std::cout << "Fitted p_d: " << result.p_d << " +/- " << result.p_d_exp_error
              << " +/- " << result.p_d_delta_error << std::endl;
    std::cout << "Correlation sineff-pu: " << result.correlation_sineff_pu << std::endl;
    std::cout << "Correlation sineff-pd: " << result.correlation_sineff_pd << std::endl;
    std::cout << "Correlation pu-pd: " << result.correlation_pu_pd << std::endl;
    std::cout << "------------------------------------" << std::endl;

    // 计算总时间
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_time = end_time - start_time;
    std::cout << "Total processing time: " << total_time.count() << " seconds" << std::endl;

    return 0;
}