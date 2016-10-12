#include <iterator>
#include <functional>
#include <cassert>

#ifdef RUN_TEST
#   include <vector>
#   include <iostream>
#   include <limits>
#   include <type_traits>
#   include "catch.hpp"
#endif


namespace tz {

namespace detail {

template<class ElemType>
class RangeIterator : public std::iterator<std::forward_iterator_tag, ElemType> {
public:
    enum class Direction { FORWARD, BACKWARD };
    typedef std::function<void (ElemType &)> IncFunc;

    RangeIterator(const ElemType &cur, const ElemType &stop, Direction dir, const IncFunc &inc)
        : cur(cur), stop(stop), dir(dir), inc(inc)
    {}

    // Pre-increment
    RangeIterator &operator++ () {
        this->inc(this->cur);
        if (this->_check_overflow()) {
            this->cur = this->stop;
        }

        return *this;
    }

    // Post-increment
    RangeIterator operator++ (int) {
        RangeIterator tmp(*this);
        ++(*this);

        return tmp;
    }

    // comparison
    bool operator == (const RangeIterator &rhs) const {
        return this->cur == rhs.cur;
    }

    bool operator != (const RangeIterator &rhs) const {
        return !(*this == rhs);
    }

    // dereference
    const ElemType &operator *() const {
        assert(!this->_check_overflow());
        return this->cur;
    }

    const ElemType *operator->() const {
        return &this->operator *();
    }

private:
    ElemType cur;
    const ElemType stop;
    const Direction dir;
    const IncFunc inc;

    bool _check_overflow() const {
        if (this->dir == Direction::FORWARD) {
            return this->cur >= this->stop;
        } else {
            return this->cur <= this->stop;
        }
    }
};

template<class ElemType>
class RangeObject {
private:
    const ElemType start;
    const ElemType stop;
    const typename RangeIterator<ElemType>::Direction dir;
    const typename RangeIterator<ElemType>::IncFunc inc;

public:
    RangeObject(
        const ElemType &start, const ElemType &stop,
        typename RangeIterator<ElemType>::Direction dir,
        const typename RangeIterator<ElemType>::IncFunc inc
    ) : start(start), stop(stop), dir(dir), inc(inc)
    {}

    RangeIterator<ElemType> begin() const {
        return RangeIterator<ElemType>(this->start, this->stop, this->dir, this->inc);
    }

    RangeIterator<ElemType> end() const {
        return RangeIterator<ElemType>(this->stop, this->stop, this->dir, this->inc);
    }
};


template<class ElemType>
RangeObject<ElemType> range_backward(const ElemType &start, const ElemType &stop) {
    return RangeObject<ElemType>(
        start, stop,
        RangeIterator<ElemType>::Direction::BACKWARD,
        [](ElemType &el) { --el; });
}

template<class ElemType, class StepType>
RangeObject<ElemType> range_step(const ElemType &start, const ElemType &stop, const StepType &step) {
    typename RangeIterator<ElemType>::Direction dir;
    if (step < 0) {
        dir = RangeIterator<ElemType>::Direction::BACKWARD;
    } else {
        dir = RangeIterator<ElemType>::Direction::FORWARD;
    }

    return RangeObject<ElemType>(
        start, stop,
        dir,
        [=](ElemType &el) { el += step; });
}

} // namespace detail


// exposed function begin

template<class ElemType>
detail::RangeObject<ElemType> range(const ElemType &stop) {
    return detail::RangeObject<ElemType>(
        ElemType(), stop,
        detail::RangeIterator<ElemType>::Direction::FORWARD,
        [](ElemType &el) { ++el; });
}

template<class ElemType>
detail::RangeObject<ElemType> range(const ElemType &start, const ElemType &stop) {
    return detail::RangeObject<ElemType>(
        start, stop,
        detail::RangeIterator<ElemType>::Direction::FORWARD,
        [](ElemType &el) { ++el; });
}

template<class ElemType, class StepType>
detail::RangeObject<ElemType> range(const ElemType &start, const ElemType &stop, const StepType &step) {
    return detail::range_step(start, stop, step);
}

template<class ElemType>
detail::RangeObject<ElemType> range(const ElemType &start, const ElemType &stop, int step) {
    assert(step != 0);

    if (step == 1) {
        return range(start, stop);
    } else if (step == -1) {
        return detail::range_backward(start, stop);
    } else {
        return detail::range_step(start, stop, step);
    }
}

// exposed function end

} // namespace tz


#ifdef RUN_TEST
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
    CHECK(float_vec_comp(to_vec(range(5.0)), expected));

    expected = { 0.5, 1.5, 2.5, 3.5 };
    CHECK(float_vec_comp(to_vec(range(0.5, 4.0)), expected));

    expected = { 0.5, 0.6, 0.7, 0.8, 0.9 };
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

    const int *ptr = vec.data();
    ans.clear();
    expected = { 5, 3 };
    for (auto p : range(ptr + vec.size() - 1, ptr, -2)) {
        ans.push_back(*p);
    }
    CHECK(ans == expected);
}

TEST_CASE("Performance test: loop") {
    int N = numeric_limits<int>::max();
    int sum = 0;
    for (int i = 0; i < N; i++) {
        sum += i;
    }
    CHECK(sum != 0);
}

TEST_CASE("Performance test: range") {
    int N = numeric_limits<int>::max();
    int sum = 0;
    for (int i : range(N)) {
        sum += i;
    }
    CHECK(sum != 0);
}
#endif
