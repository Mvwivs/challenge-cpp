
#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <cstddef>

int maxKarma(int k, std::vector<std::pair<int, bool>> contests) {
    if (contests.size() == 0) {
	    return 0;
    }
	assert(k >= 0 && k <= contests.size());

    std::size_t important_count = std::count_if(contests.begin(), contests.end(), 
        [] (const auto& v) { return v.second; });

    if (k == 0) {
        return std::accumulate(contests.begin(), contests.end(), 0, 
            [] (const auto& a, const auto& v) { return v.second ? a : a + v.first ; });
    }
    else if (important_count <= k) {
	    return std::accumulate(contests.begin(), contests.end(), 0, 
            [] (const auto& a, const auto& v) { return a + v.first; });
    }
    else {
        std::nth_element(contests.begin(), contests.begin() + k - 1, contests.end(),
            [](const auto& a, const auto& b) {
                if (!a.second && !b.second) {
                    return a.first > b.first;
                }
                if (!a.second) {
                    return false;
                }
                if (!b.second) {
                    return true;
                }
                return a.first > b.first;
        });
        int kth = contests[k].first;

        std::size_t greater_kth_count = 0;
        int total_karma = 0;
        for (const auto& [karma, important] : contests) {
            if (important && (karma > kth)) {
                total_karma += karma;
                ++greater_kth_count;
            }
            else if (!important) {
                total_karma += karma;
            }
        }
        total_karma += (k - greater_kth_count) * kth;
        return total_karma;
    }
}
