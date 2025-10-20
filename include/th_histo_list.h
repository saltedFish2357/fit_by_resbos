#pragma once

#include "th_histo.h"

struct th_histo_list
{
    /* data */
    std::vector<th_histo> th_u_histo_list;
    std::vector<th_histo> th_d_histo_list;
    std::vector<th_histo> th_total_histo_list;

    // 构造函数
    th_histo_list(std::vector<th_histo> u_list, std::vector<th_histo> d_list, std::vector<th_histo> total_list)
        : th_u_histo_list(u_list), th_d_histo_list(d_list), th_total_histo_list(total_list) {}

    // 存储histogram
    void save_all(const std::string &dir_path, const std::string &file_name_prefix = "th_f_histo") const;
};
