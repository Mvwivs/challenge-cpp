
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

    if (k == 0) { // only not important events can be lost
        return std::accumulate(contests.begin(), contests.end(), 0, 
            [] (const auto& a, const auto& v) { return v.second ? (a - v.first) : (a + v.first); });
    }
    else if (important_count <= k) { // all events can be lost
	    return std::accumulate(contests.begin(), contests.end(), 0, 
            [] (const auto& a, const auto& v) { return a + v.first; });
    }
    else { // some events can be lost
        // find kth largest event among important ones
        std::nth_element(contests.begin(), contests.begin() + k - 1, contests.end(),
            [](const auto& a, const auto& b) {
                // next three conditions are for not important events,
                // they are always less than imported ones
                if (!a.second && !b.second) {
                    return a.first > b.first;
                }
                if (!a.second) {
                    return false;
                }
                if (!b.second) {
                    return true;
                }
                // compare important events
                return a.first > b.first;
        });
        int kth = contests[k - 1].first;

        // additionally there can be several elements equal to k-th,
        // we need to find how many of them can be lost or won
        std::size_t greater_kth_count = 0;
        std::size_t less_kth_count = 0;
        int total_karma = 0;
        for (const auto& [karma, important] : contests) {
            if (important) {
                if (karma > kth) { // lost important event
                    total_karma += karma;
                    ++greater_kth_count;
                }
                else if (karma < kth) { //  won important event
                    ++less_kth_count;
                    total_karma -= karma;
                }
            }
            else if (!important) { // sum all not important events
                total_karma += karma;
            }
        }
        std::size_t kth_count = important_count - less_kth_count - greater_kth_count;
        std::size_t lost_kth = k - greater_kth_count;
        std::size_t won_kth = kth_count - lost_kth;
        // take into account events with karma equal to k-th event
        total_karma += (lost_kth - won_kth) * kth;
        return total_karma;
    }
    // Total complexity O(N):
    // count important events O(N), find k-th element O(N) on average, sum karma O(N),
}
