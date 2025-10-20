#pragma once

#include <atomic>
#include <string>

inline std::string generate_unique_name(const std::string &base)
{
    static std::atomic<int> counter{0};
    return base + "_" + std::to_string(counter.fetch_add(1, std::memory_order_relaxed));
}