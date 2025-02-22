#pragma once

#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-mime/mime.h>
#include <karm-mime/url.h>
#include <vaev-css/lexer.h>

#include "font-props.h"
#include "media.h"
#include "props.h"
#include "select.h"

namespace Vaev::Style {

struct Rule;

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
struct StyleRule {
    Selector selector;
    Vec<Prop> props;

    void repr(Io::Emit &e) const;

    bool match(Dom::Element const &el) const {
        return selector.match(el);
    }
};

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
struct ImportRule {
    Mime::Url url;

    void repr(Io::Emit &e) const;
};

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
struct MediaRule {
    MediaQuery media;
    Vec<Rule> rules;

    void repr(Io::Emit &e) const;

    bool match(Media const &m) const;
};

// https://www.w3.org/TR/css-fonts-4/#cssfontfacerule
struct FontFaceRule {
    Vec<FontProp> props;

    void repr(Io::Emit &e) const;
};

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
using _Rule = Union<
    StyleRule,
    FontFaceRule,
    MediaRule,
    ImportRule>;

struct Rule : public _Rule {
    using _Rule::_Rule;

    void repr(Io::Emit &e) const;
};

// https://www.w3.org/TR/cssom-1/#css-style-sheets
struct StyleSheet {
    Mime::Mime mime = "text/css"_mime;
    Mime::Url href = ""_url;
    Str title = "";
    Vec<Rule> rules;

    void repr(Io::Emit &e) const;
};

struct StyleBook {
    Vec<StyleSheet> styleSheets;

    void repr(Io::Emit &e) const;

    void add(StyleSheet &&sheet);
};

} // namespace Vaev::Style
