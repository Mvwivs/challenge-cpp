
#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>
#include <cstddef>

int maxKarma(int k, std::vector<std::pair<int, bool>> contests) {
    if (contests.size() == 0) {
	    return 0;
    }
	assert(k >= 0 && k <= contests.size());
    
	std::nth_element(contests.begin(), contests.begin() + k, contests.end(),
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
	std::size_t not_important = 0;
	int total_karma = 0;
	for (const auto& [karma, important] : contests) {
		if (important && (karma > kth)) {
			total_karma += karma;
			++greater_kth_count;
		}
		else if (!important) {
			total_karma += karma;
			++not_important;
		}
	}
    if ((contests.size() - not_important) >= k) {
	    total_karma += (k - greater_kth_count) * kth;
    }
	return total_karma;
}
