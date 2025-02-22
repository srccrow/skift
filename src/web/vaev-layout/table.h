#pragma once

#include "flow.h"

namespace Vaev::Layout {

struct TableFlow : public Flow {
    static constexpr auto TYPE = TABLE;

    using Flow::Flow;

    struct Dim {
        usize frag;
    };

    Vec<Dim> _rows;
    Vec<Dim> _cols;

    Type type() const override {
        return TYPE;
    }

    void layout(RectPx bound) override {
        Flow::layout(bound);
    }
};

} // namespace Vaev::Layout
