#ifndef RANGE_HPP
#define RANGE_HPP

#include <iterator>
#include <functional>
#include <cassert>


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

#endif  // RANGE_HPP
