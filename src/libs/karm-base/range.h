#pragma once

#include "align.h"
#include "cons.h"

namespace Karm {

template <typename T, typename TAG = struct _RangeTag>
struct Range {
    T start{};
    T size{};

    static constexpr Range fromStartEnd(T start, T end) {
        return {start, end - start};
    }

    constexpr Range() = default;

    constexpr Range(T start, T size = 0)
        : start(start), size(size) {
    }

    constexpr T end() const {
        return start + size;
    }

    constexpr void end(T value) {
        size = value - start;
    }

    constexpr bool empty() const {
        return size == T{};
    }

    constexpr bool any() const {
        return not empty();
    }

    constexpr bool valid() const {
        return size >= T{};
    }

    constexpr bool contains(T value) const {
        return start <= value and value < end();
    }

    constexpr bool contains(Range other) const {
        return start <= other.start and other.end() <= end();
    }

    constexpr bool contigous(Range other) const {
        return end() == other.start or start == other.end();
    }

    constexpr bool overlaps(Range other) const {
        return start < other.end() and other.start < end();
    }

    constexpr Range merge(Range other) const {
        return fromStartEnd(
            min(start, other.start),
            max(end(), other.end())
        );
    }

    constexpr Range halfUnder(Range other) {
        if (overlaps(other) and start < other.start) {
            return {start, other.start - start};
        }

        return {};
    }

    constexpr Range slice(T off, T size) const {
        return {start + off, size};
    }

    constexpr Range slice(T off) const {
        return slice(off, size - off);
    }

    constexpr Range halfOver(Range other) {
        if (overlaps(other) and other.end() < end()) {
            return {other.end(), end() - other.end()};
        }

        return {};
    }

    constexpr Cons<Range> split(Range other) {
        return {halfUnder(other), halfOver(other)};
    }

    constexpr auto iter() const {
        return range(start, end());
    }

    constexpr auto iterRev() const {
        return range(end(), start);
    }

    template <typename U>
    constexpr auto as() const {
        return U{start, size};
    }

    Res<> ensureAligned(T alignment) const {
        if (not isAlign(start, alignment))
            return Error::invalidInput("start is not aligned");

        if (not isAlign(size, alignment))
            return Error::invalidInput("size is not aligned");

        return Ok();
    }

    std::strong_ordering operator<=>(Range const &other) const {
        if (start == other.start and size == other.size)
            return std::strong_ordering::equal;

        if (start < other.start)
            return std::strong_ordering::less;

        return std::strong_ordering::greater;
    }

    bool operator==(Range const &other) const {
        return start == other.start and size == other.size;
    }

    explicit operator bool() const {
        return any();
    }
};

using irange = Range<isize>;
using urange = Range<usize>;
using frange = Range<f64>;

} // namespace Karm
