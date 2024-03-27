#include <karm-base/ctype.h>

#include "url.h"

namespace Karm::Mime {

/* --- Path ----------------------------------------------------------------- */

Path Path::parse(Io::SScan &s, bool inUrl, bool stopAtWhitespace) {
    Path path;

    if (s.skip(SEP)) {
        path.rooted = true;
    }

    s.begin();
    while (not s.ended() and (inUrl or (s.curr() != '?' and s.curr() != '#')) and
           (not stopAtWhitespace or not isAsciiSpace(s.curr()))) {
        if (s.curr() == SEP) {
            path._parts.pushBack(s.end());
            s.next();
            s.begin();
        } else {
            s.next();
        }
    }

    auto last = s.end();
    if (last.len() > 0)
        path._parts.pushBack(last);

    return path;
}

Path Path::parse(Str str, bool inUrl, bool stopAtWhitespace) {
    Io::SScan s{str};
    return parse(s, inUrl, stopAtWhitespace);
}

void Path::normalize() {
    Vec<String> parts;
    for (auto part : iter()) {
        if (part == ".")
            continue;

        if (part == "..") {
            if (parts.len() > 0) {
                parts.popBack();
            } else if (not rooted) {
                parts.pushBack(part);
            }
        } else
            parts.pushBack(part);
    }

    _parts = parts;
}

Str Path::basename() const {
    if (not _parts.len())
        return {};

    return last(_parts);
}

Path Path::join(Path const &other) const {
    if (other.rooted) {
        return other;
    }

    Path path = *this;
    path._parts.pushBack(other._parts);
    path.normalize();
    return path;
}

Path Path::join(Str other) const {
    return join(parse(other));
}

Path Path::parent(usize n) const {
    Path path = *this;
    if (path._parts.len() >= n) {
        for (usize i = 0; i < n; i++)
            path._parts.popBack();
    }
    return path;
}

Res<usize> Path::write(Io::TextWriter &writer) const {
    usize written = 0;

    if (not rooted)
        written += try$(writer.writeRune('.'));

    if (rooted and len() == 0)
        written += try$(writer.writeRune(SEP));

    for (auto part : iter())
        written += try$(Io::format(writer, "{c}{}", SEP, part));

    return Ok(written);
}

String Path::str() const {
    Io::StringWriter writer;
    write(writer).unwrap();
    return writer.str();
}

} // namespace Karm::Mime
