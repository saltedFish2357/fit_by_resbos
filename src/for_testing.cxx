#include "../include/th_parameter.h"
#include "../include/get_th_parameter_list.h"
#include <vector>
#include <iostream>
#include <string>
#include "../include/bin.h"
#include <TChain.h>
#include "../include/MyFit.h"
#include "../include/result_col.h"
#include "../include/BinUtils.h"
#include "../include/exp_parameter.h"
#include <ROOT/RDataFrame.hxx>
#include "../include/get_exp_Afb_list.h"

void for_testing()
{
    // 定义文件路径常量
    const std::string data_dir = "/Data/xyzhou/fitSin2ThetaW1/data/";
    const std::string output_dir = "/Data/xyzhou/fitByResBos/output/";
    const std::string th_data_dir = "/Data/xyzhou/sineffcheck_test";

    const bin Rapidity_bin(0, 1);
    const bin Qt_bin(0, 5);
    const std::vector<double> bin_array = {40, 50, 60, 76, 86, 96, 106, 120, 133, 155, 171, 200, 320};
    const std::vector<bin> Mass_bin_list = BinUtils::create_bin_list_from_array(bin_array);
    const std::string out_file = output_dir + "th_histo.root";
    const std::vector<const char *> sineff_test = {"2300", "2305", "2310", "2315", "2320", "2325", "2330"};

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

    std::vector<exp_parameter> exp_Afb_list = get_exp_Afb_list(exp_df, Rapidity_bin, Qt_bin, Mass_bin_list);

    // 获取理论参数列表
    std::vector<th_parameter> th_parameter_list = get_th_parameter_list(out_file, sineff_test, Mass_bin_list);
    std::cout << "Theoretical parameter list size: " << th_parameter_list.size() << std::endl;

    // 执行拟合
    std::cout << "Starting fit process..." << std::endl;

    result_col result = MyAnalysis::MyFit::fit_sineff_and_pu_pd(exp_Afb_list, th_parameter_list);

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
}
