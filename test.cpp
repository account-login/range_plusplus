#include <vector>
#include <iostream>
#include <limits>
#include <type_traits>

#include "catch.hpp"
#include "range.hpp"


using namespace std;
using namespace tz;
using namespace tz::detail;


template<class ElemType>
vector<ElemType> to_vec(const RangeObject<ElemType> &ranger) {
    vector<ElemType> ans;
    for (auto i : ranger) {
        ans.push_back(i);
    }

    return ans;
}

template<class FloatType>
bool float_vec_comp(const vector<FloatType> &v1, const vector<FloatType> &v2) {
    for (int i : range(v1.size())) {
        if (v1[i] != Approx(v2[i])) {
            return false;
        }
    }

    return true;
}

namespace std {
    template <typename T>
    std::ostream &operator << (std::ostream& out, const std::vector<T> &vec) {
        auto comma = "";
        out << "vector { ";
        for (auto &v : vec) {
            out << comma;
            out << v;
            comma = ", ";
        }
        out << " }";
        return out;
    }
}


TEST_CASE("Test on int") {
    vector<int> expected = { 0, 1, 2, 3, 4 };
    CHECK(to_vec(range(5)) == expected);

    expected = { 1, 3, 5, 7 };
    CHECK(to_vec(range(1, 8, 2)) == expected);

    expected = { 5, 6, 7 };
    CHECK(to_vec(range(5, 8)) == expected);

    expected = {};
    CHECK(to_vec(range(5, 5)) == expected);

    expected = { 5, 4, 3, 2, 1 };
    CHECK(to_vec(range(5, 0, -1)) == expected);

    expected = { 0 };
    CHECK(to_vec(range(0, 2, 4)) == expected);
}

TEST_CASE("Test on double") {
    CHECK((is_same<double, decltype(1.0)>::value));

    vector<double> expected = { 0, 1, 2, 3, 4 };
    CAPTURE(expected);
    CHECK(float_vec_comp(to_vec(range(4.99)), expected));

    expected = { 0.5, 1.5, 2.5, 3.5 };
    CAPTURE(expected);
    CHECK(float_vec_comp(to_vec(range(0.5, 4.0)), expected));

    expected = { 0.5, 0.6, 0.7, 0.8, 0.9 };
    CAPTURE(expected);
    CHECK(float_vec_comp(to_vec(range(0.5, 1.0 - 1e-5, 0.1)), expected));
}

TEST_CASE("Test on iterator/pointer") {
    vector<int> vec = { 1, 2, 3, 4, 5 };
    vector<int> expected = { 5, 4 ,3, 2, 1 };
    vector<int> ans;
    for (auto it : range(vec.rbegin(), vec.rend())) {
        ans.push_back(*it);
    }
    CHECK(ans == expected);

    ans.clear();
    expected = { 1, 3, 5 };
    for (auto it : range(vec.begin(), vec.end(), 2)) {
        ans.push_back(*it);
    }
    CHECK(ans == expected);

    ans.clear();
    expected = { 1, 2, 3, 4, 5 };
    for (auto it : range(vec.begin(), vec.end(), 1)) {
        ans.push_back(*it);
    }
    CHECK(ans == expected);

    const int *ptr = vec.data();
    ans.clear();
    expected = { 5, 3 };
    for (auto p : range(ptr + vec.size() - 1, ptr, -2)) {
        ans.push_back(*p);
    }
    CHECK(ans == expected);
}

TEST_CASE("Performance test: loop") {
    int N = numeric_limits<int>::max() / 100;
    int sum = 0;
    for (int i = 0; i < N; i++) {
        sum += i;
    }
    CHECK(sum != 0);
}

TEST_CASE("Performance test: range") {
    int N = numeric_limits<int>::max() / 100;
    int sum = 0;
    for (int i : range(N)) {
        sum += i;
    }
    CHECK(sum != 0);
}
