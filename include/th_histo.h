#pragma once

#include <TH3.h>
#include <memory>
#include <vector>
#include <string>
#include <TFile.h>

struct th_histo
{
    std::vector<std::shared_ptr<TH3D>> correct;
    std::vector<std::shared_ptr<TH3D>> wrong;
    std::vector<std::shared_ptr<TH3D>> forward;
    std::vector<std::shared_ptr<TH3D>> backward;
    std::shared_ptr<TH3D> total;

    // save
    void save(TFile *root_file, const std::string &histo_name) const;
};
