#include "bin.h"
#include <cmath>

bin::bin(double lower, double upper) : m_lower(lower), m_upper(upper)
{
}

double bin::lower() const { return m_lower; }
double bin::upper() const { return m_upper; }

double bin::width() const { return std::abs(m_upper - m_lower); }

void bin::set_lower(double lower) { m_lower = lower; }
void bin::set_upper(double upper) { m_upper = upper; }

bool bin::is_in_bin(double num) const
{
    if (num >= m_lower && num <= m_upper)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<bin> bin::div_bin(int count) const
{
    std::vector<bin> small_bin_list;
    small_bin_list.reserve(count);

    int index = 0;
    double width = this->width() / count;
    for (auto &small_bin : small_bin_list)
    {
        small_bin.set_lower(index * width + m_lower);
        small_bin.set_upper((index + 1) * width + m_lower);
    }

    return small_bin_list;
}

int bin::get_adaptive_bins(int max_bins) const
{
    double range = m_upper - m_lower;
    if (range <= 0)
        return 10; // 保护性检查

    // 根据范围自动计算合理的bin数量
    int calculated_bins = static_cast<int>(std::sqrt(range) * 30);
    return std::min(max_bins, std::max(10, calculated_bins));
}
