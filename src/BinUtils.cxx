#include <vector>
#include <bin.h>

namespace BinUtils {
    std::vector<double> to_edges(const std::vector<bin> bins) {
        std::vector<double> edges;
        edges.reserve(bins.size() + 1);

        edges.push_back(bins[0].lower());
        for (int i = 0; i < bins.size(); i++) {
            edges.push_back(bins[i].upper());
        }

        return edges;
    }

    std::vector<bin> create_bin_list_from_array(const std::vector<double> array) {
        std::vector<bin> bin_list;
        bin_list.reserve(array.size() - 1);
        for(int i = 0; i < array.size() - 1; i++) {
            bin_list.push_back({array[i], array[i + 1]});
        }
        return bin_list;
    }
}