#pragma once

#include "fmt.h"

namespace Karm::Fmt {

enum struct Case {
    DEFAULT,
    CAMEL,
    CAPITAL,
    CONSTANT,
    DOT,
    HEADER,
    NO,
    PARAM,
    PASCAL,
    PATH,
    SENTENCE,
    SNAKE,
    TITLE,
    SWAP,
    LOWER,
    LOWER_FIRST,
    UPPER,
    UPPER_FIRST,
    SPONGE,
};

Res<String> toDefaultCase(Str str);

Res<String> toCamelCase(Str str);

Res<String> toCapitalCase(Str str);

Res<String> toConstantCase(Str str);

Res<String> toDotCase(Str str);

Res<String> toHeaderCase(Str str);

Res<String> toNoCase(Str str);

Res<String> toParamCase(Str str);

Res<String> toPascalCase(Str str);

Res<String> toPathCase(Str str);

Res<String> toSentenceCase(Str str);

Res<String> toSnakeCase(Str str);

Res<String> toTitleCase(Str str);

Res<String> toSwapCase(Str str);

Res<String> toLowerCase(Str str);

Res<String> toLowerFirstCase(Str str);

Res<String> toUpperCase(Str str);

Res<String> toUpperFirstCase(Str str);

Res<String> toSpongeCase(Str str);

Res<String> changeCase(Str str, Case toCase);

template <typename T>
struct Cased {
    T _inner;
    Case _case;
};

inline auto cased(auto inner, Case cased) {
    return Cased<decltype(inner)>{inner, cased};
}

enum struct Align {
    LEFT,
    RIGHT,
    CENTER,
};

template <typename T>
struct Aligned {
    T _inner;
    Align _align;
    usize _width;
};

inline auto aligned(auto inner, Align align, usize width) {
    return Aligned<decltype(inner)>{inner, align, width};
}

} // namespace Karm::Fmt

template <typename T>
struct Karm::Fmt::Formatter<Karm::Fmt::Cased<T>> {
    Formatter<T> _innerFmt{};

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          _innerFmt.parse(scan);
                      }) {
            _innerFmt.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, Karm::Fmt::Cased<T> val) {
        auto result = try$(changeCase(val._inner, val._case));
        return writer.writeStr(result);
    }
};

template <typename T>
struct Karm::Fmt::Formatter<Karm::Fmt::Aligned<T>> {
    Formatter<T> _innerFmt{};

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          _innerFmt.parse(scan);
                      }) {
            _innerFmt.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, Karm::Fmt::Aligned<T> val) {
        Io::StringWriter buf;
        try$(_innerFmt.format(buf, val._inner));
        usize width = buf.len();
        usize result = 0;

        if (width < val._width) {
            usize pad = val._width - width;
            switch (val._align) {
            case Align::LEFT:
                result += try$(writer.writeStr(buf.str()));
                for (usize i = 0; i < pad; i++)
                    result += try$(writer.writeRune(' '));
                break;
            case Align::RIGHT:
                for (usize i = 0; i < pad; i++)
                    result += try$(writer.writeRune(' '));
                result += try$(writer.writeStr(buf.str()));
                break;
            case Align::CENTER:
                for (usize i = 0; i < pad / 2; i++)
                    result += try$(writer.writeRune(' '));
                result += try$(writer.writeStr(buf.str()));
                for (usize i = 0; i < pad / 2; i++)
                    result += try$(writer.writeRune(' '));
                break;
            }
        } else {
            result += try$(writer.writeStr(buf.str()));
        }

        return Ok(result);
    }
};
