#pragma once

#include <karm-base/distinct.h>
#include <karm-base/std.h>

namespace Vaev {

// https://drafts.csswg.org/css-values/#frequency
// It represents the number of occurrences per second.
struct Frequency : public Distinct<f64, struct FrequencyTag> {
    using Distinct::Distinct;

    static Frequency fromHz(f64 hz) {
        return Frequency(hz);
    }

    static Frequency fromKHz(f64 khz) {
        return Frequency(khz * 1e3);
    }
};

} // namespace Vaev
