
#include <iostream>

#include "karma.hpp"


void test_empty() {
    std::vector<std::pair<int, bool>> contests = {};
    assert(maxKarma(0, contests) == 0);
}

void test_all_not_important() {
    std::vector<std::pair<int, bool>> contests = {
        {2, false},
        {1, false},
        {10, false},
        {5, false},
        {6, false}
    };
    int expected = 24;
    assert(maxKarma(0, contests) == expected);
    assert(maxKarma(1, contests) == expected);
    assert(maxKarma(2, contests) == expected);
    assert(maxKarma(4, contests) == expected);
    assert(maxKarma(5, contests) == expected);
}

void test_all_important() {
    std::vector<std::pair<int, bool>> contests = {
        {2, true},
        {1, true},
        {10, true},
        {5, true},
        {6, true}
    };
    assert(maxKarma(0, contests) == 0);
    assert(maxKarma(1, contests) == 10);
    assert(maxKarma(5, contests) == 24);
}

void test_one_not_important() {
    std::vector<std::pair<int, bool>> contests = {
        {2, false},
    };
    int expected = 2;
    assert(maxKarma(0, contests) == expected);
    assert(maxKarma(1, contests) == expected);
}

void test_one_important() {
    std::vector<std::pair<int, bool>> contests = {
        {2, true},
    };
    assert(maxKarma(0, contests) == 0);
    assert(maxKarma(1, contests) == 2);
}

void test_two_different() {
    std::vector<std::pair<int, bool>> contests = {
        {2, true},
        {3, false},
    };
    assert(maxKarma(0, contests) == 3);
    assert(maxKarma(1, contests) == 5);
    assert(maxKarma(2, contests) == 5);
}

void test_small_important_cnt() {
    std::vector<std::pair<int, bool>> contests = {
        {2, true},
        {1, false},
        {10, false},
        {5, true},
        {6, true}
    };
    int expected = 24;
    assert(maxKarma(4, contests) == expected);
    assert(maxKarma(5, contests) == expected);
}

int main() {
	test_empty();
	test_all_not_important();
	test_all_important();
	test_one_not_important();
	test_one_important();
	test_two_different();
	test_small_important_cnt();

	return 0;
}
