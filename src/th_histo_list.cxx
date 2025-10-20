#include "th_histo_list.h"
#include <TFile.h>

void th_histo_list::save_all(const std::string &dir_path, const std::string &file_name_prefix) const
{
    for (int i = 0; i < th_u_histo_list.size(); i++)
    {
        TFile *root_file = TFile::Open(dir_path.c_str(), "UPDATE");
        if (!root_file || root_file->IsZombie())
        {
            throw std::runtime_error("Failed to open or create ROOT file: " + dir_path);
        }
        std::string histo_name = file_name_prefix + "_u_" + std::to_string(i);
        th_u_histo_list[i].save(root_file, histo_name);
        histo_name = file_name_prefix + "_d_" + std::to_string(i);
        th_d_histo_list[i].save(root_file, histo_name);
        histo_name = file_name_prefix + "_total_" + std::to_string(i);
        th_total_histo_list[i].save(root_file, histo_name);
        root_file->Close();
        delete root_file;
    }
}