#pragma once

#include <ROOT/RDataFrame.hxx>
#include "bin.h"

namespace MyAnalysis
{
    namespace ParticleAnalysis
    {
        ROOT::RDF::RNode apply_Rapidity_cut(ROOT::RDF::RNode df, const bin &Rapidity_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col);

        ROOT::RDF::RNode apply_Qt_cut(ROOT::RDF::RNode df, const bin &Qt_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col);

        ROOT::RDF::RNode apply_Mass_cut(ROOT::RDF::RNode df, const bin &Mass_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col);

        // 字符串表达式版本（用于 th/float 分支）
        ROOT::RDF::RNode apply_Rapidity_cut_expr(ROOT::RDF::RNode df, const bin &Rapidity_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col);
        ROOT::RDF::RNode apply_Qt_cut_expr(ROOT::RDF::RNode df, const bin &Qt_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col);
        ROOT::RDF::RNode apply_Mass_cut_expr(ROOT::RDF::RNode df, const bin &Mass_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col);

        ROOT::RDF::RNode apply_particle_cut(ROOT::RDF::RNode df, const bin &Rapidity_bin, const bin &Qt_bin, const bin &Mass_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col);
        // 字符串表达式版本的整体切割流程
        ROOT::RDF::RNode apply_particle_cut_expr(ROOT::RDF::RNode df, const bin &Rapidity_bin, const bin &Qt_bin, const bin &Mass_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col);
    }
}