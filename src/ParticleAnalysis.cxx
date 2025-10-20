#include "ParticleAnalysis.h"
#include "AnalysisUtils.h"

ROOT::RDF::RNode MyAnalysis::ParticleAnalysis::apply_Rapidity_cut(ROOT::RDF::RNode df, const bin &Rapidity_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col)
{
    return df
        .Define(
            "Rapidity",
            [](const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E)
            {
                return std::abs(MyAnalysis::Utils::get_Rapidity(Z_Px, Z_Py, Z_Pz, Z_E)); // 取了绝对值
            },
            {px_col, py_col, pz_col, e_col})
        .Filter(
            [Rapidity_bin](const double &Rapidity)
            {
                return Rapidity_bin.is_in_bin(Rapidity);
            },
            {"Rapidity"},
            "Rapidity_cut");
}

// 字符串表达式版本，适用于分支类型为 float 的情况
ROOT::RDF::RNode MyAnalysis::ParticleAnalysis::apply_Rapidity_cut_expr(ROOT::RDF::RNode df, const bin &Rapidity_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col)
{
    // 定义 Rapidity（取绝对值）并使用字符串过滤条件
    std::string rap_expr = "fabs(" + MyAnalysis::Utils::get_Rapidity_expr(px_col, py_col, pz_col, e_col) + ")";
    df = df.Define("Rapidity", rap_expr);

    std::string cond = "Rapidity >= " + std::to_string(Rapidity_bin.lower()) + " && Rapidity < " + std::to_string(Rapidity_bin.upper());
    return df.Filter(cond, "Rapidity_cut");
}

ROOT::RDF::RNode MyAnalysis::ParticleAnalysis::apply_Qt_cut(ROOT::RDF::RNode df, const bin &Qt_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col)
{
    return df
        .Define(
            "Qt",
            [](const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E)
            {
                return MyAnalysis::Utils::get_Qt(Z_Px, Z_Py, Z_Pz, Z_E);
            },
            {px_col, py_col, pz_col, e_col})
        .Filter(
            [Qt_bin](const double &Qt)
            {
                return Qt_bin.is_in_bin(Qt);
            },
            {"Qt"},
            "Qt_cut");
}

// 字符串表达式版本
ROOT::RDF::RNode MyAnalysis::ParticleAnalysis::apply_Qt_cut_expr(ROOT::RDF::RNode df, const bin &Qt_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col)
{
    // get_Qt_expr 只需要 px, py
    std::string qt_expr = MyAnalysis::Utils::get_Qt_expr(px_col, py_col);
    df = df.Define("Qt", qt_expr);

    std::string cond = "Qt >= " + std::to_string(Qt_bin.lower()) + " && Qt < " + std::to_string(Qt_bin.upper());
    return df.Filter(cond, "Qt_cut");
}

ROOT::RDF::RNode MyAnalysis::ParticleAnalysis::apply_Mass_cut(ROOT::RDF::RNode df, const bin &Mass_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col)
{
    return df
        .Define(
            "Mass",
            [](const double &Z_Px, const double &Z_Py, const double &Z_Pz, const double &Z_E)
            {
                return MyAnalysis::Utils::get_M(Z_Px, Z_Py, Z_Pz, Z_E);
            },
            {px_col, py_col, pz_col, e_col})
        .Filter(
            [Mass_bin](const double &Z_M)
            { return Mass_bin.is_in_bin(Z_M); },
            {"Mass"},
            "Mass_cut");
}

// 字符串表达式版本
ROOT::RDF::RNode MyAnalysis::ParticleAnalysis::apply_Mass_cut_expr(ROOT::RDF::RNode df, const bin &Mass_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col)
{
    std::string mass_expr = MyAnalysis::Utils::get_M_expr(px_col, py_col, pz_col, e_col);
    df = df.Define("Mass", mass_expr);

    std::string cond = "Mass >= " + std::to_string(Mass_bin.lower()) + " && Mass < " + std::to_string(Mass_bin.upper());
    return df.Filter(cond, "Mass_cut");
}

ROOT::RDF::RNode MyAnalysis::ParticleAnalysis::apply_particle_cut(ROOT::RDF::RNode df, const bin &Rapidity_bin, const bin &Qt_bin, const bin &Mass_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col)
{
    ROOT::RDF::RNode df1 = MyAnalysis::ParticleAnalysis::apply_Rapidity_cut(df, Rapidity_bin, px_col, py_col, pz_col, e_col);
    ROOT::RDF::RNode df2 = MyAnalysis::ParticleAnalysis::apply_Qt_cut(df1, Qt_bin, px_col, py_col, pz_col, e_col);
    ROOT::RDF::RNode df3 = MyAnalysis::ParticleAnalysis::apply_Mass_cut(df2, Mass_bin, px_col, py_col, pz_col, e_col);
    return df3;
}

// 字符串表达式版本的整体切割流程，适用于 th（float）数据
ROOT::RDF::RNode MyAnalysis::ParticleAnalysis::apply_particle_cut_expr(ROOT::RDF::RNode df, const bin &Rapidity_bin, const bin &Qt_bin, const bin &Mass_bin, const std::string &px_col, const std::string &py_col, const std::string &pz_col, const std::string &e_col)
{
    // 先 Define 三个物理量，后 Filter，避免重复计算
    std::string rap_expr = "fabs(" + MyAnalysis::Utils::get_Rapidity_expr(px_col, py_col, pz_col, e_col) + ")";
    std::string qt_expr = MyAnalysis::Utils::get_Qt_expr(px_col, py_col);
    std::string mass_expr = MyAnalysis::Utils::get_M_expr(px_col, py_col, pz_col, e_col);

    auto df1 = df.Define("Rapidity", rap_expr)
                   .Define("Qt", qt_expr)
                   .Define("Mass", mass_expr);
    // ...existing code...

    std::string rap_cond = "Rapidity >= " + std::to_string(Rapidity_bin.lower()) + " && Rapidity < " + std::to_string(Rapidity_bin.upper());
    auto df2 = df1.Filter(rap_cond, "Rapidity_cut");
    // ...existing code...

    std::string qt_cond = "Qt >= " + std::to_string(Qt_bin.lower()) + " && Qt < " + std::to_string(Qt_bin.upper());
    auto df3 = df2.Filter(qt_cond, "Qt_cut");
    // ...existing code...

    std::string mass_cond = "Mass >= " + std::to_string(Mass_bin.lower()) + " && Mass < " + std::to_string(Mass_bin.upper());
    auto df4 = df3.Filter(mass_cond, "Mass_cut");
    // ...existing code...
    return df4;
}
