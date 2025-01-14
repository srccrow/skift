#pragma once

#include <karm-base/rc.h>
#include <karm-base/res.h>
#include <karm-base/vec.h>

#include "async.h"

namespace Karm::Sys {

struct Service {
    virtual ~Service() = default;
};

struct Context :
    Meta::Static {
    Vec<Strong<Service>> _srvs;

    template <typename T>
    T &use() {
        for (auto &hook : _srvs)
            if (hook.is<T>())
                return hook.unwrap<T>();

        panic("no such service");
    }

    template <typename T, typename... Args>
    void add(Args &&...args) {
        _srvs.pushBack(makeStrong<T>(std::forward<Args>(args)...));
    }
};

Context &globalContext();

struct ArgsHook :
    public Service {

    usize _argc;
    char const **_argv;

    ArgsHook(usize argc, char const **argv)
        : _argc(argc), _argv(argv) {}

    Str self() const {
        return _argv[0];
    }

    usize len() const {
        return _argc - 1;
    }

    Str operator[](usize i) const {
        if (i >= len()) {
            panic("out of range");
        }
        return _argv[i + 1];
    }

    bool has(Str arg) const {
        for (usize i = 0; i < len(); ++i)
            if (operator[](i) == arg)
                return true;
        return false;
    }
};

inline auto &useArgs(Context &ctx = globalContext()) {
    return ctx.use<ArgsHook>();
}

enum struct FormFactor {
    DESKTOP,
    MOBILE,
};

inline FormFactor useFormFactor(Context &ctx = globalContext()) {
    if (useArgs(ctx).has("--mobile"))
        return FormFactor::MOBILE;
    return FormFactor::DESKTOP;
}

} // namespace Karm::Sys

Async::Task<> entryPointAsync(Sys::Context &);
