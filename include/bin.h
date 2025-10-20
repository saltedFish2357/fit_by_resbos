#ifndef BIN_H
#define BIN_H

#include <vector>

class bin
{
private:
    double m_lower;
    double m_upper;

public:
    bin(double lower, double upper);
    double lower() const;
    double upper() const;
    double width() const;
    void set_lower(double lower);
    void set_upper(double upper);
    bool is_in_bin(double num) const;
    std::vector<bin> div_bin(int count) const;
    int get_adaptive_bins(int max_bins = 100) const;
};

#endif