#include <hideo-base/alert.h>
#include <hideo-base/scafold.h>
#include <karm-kira/badge.h>
#include <karm-sys/entry.h>
#include <karm-text/book.h>
#include <karm-ui/app.h>
#include <karm-ui/scroll.h>

namespace Hideo::Fonts {

struct State {
    Text::FontBook fontBook;
    Opt<String> fontFamily = NONE;
    Opt<Strong<Text::Fontface>> fontFace = NONE;

    State(Text::FontBook fontBook) : fontBook(fontBook) {}

    bool canGoBack() const {
        return fontFace || fontFamily;
    }
};

struct GoBack {};

struct SelectFamily {
    String family;
};

struct SelectFace {
    Strong<Text::Fontface> id;
};

using Action = Union<GoBack, SelectFamily, SelectFace>;

void reduce(State &s, Action a) {
    a.visit(Visitor{
        [&](GoBack) {
            if (s.fontFace) {
                s.fontFace = NONE;
            } else if (s.fontFamily) {
                s.fontFamily = NONE;
            }
        },
        [&](SelectFamily a) {
            s.fontFamily = a.family;
        },
        [&](SelectFace a) {
            s.fontFace = a.id;
        },
    });
}

using Model = Ui::Model<State, Action, reduce>;

static constexpr Str PANGRAM = "All beings born free, equal in dignity, rights—justice demanded, voice exhorted, zealously championed worldwide";

// MARK: All Families ----------------------------------------------------------

Ui::Child allFamiliesItem(State const &s, Str family) {
    auto &fontBook = s.fontBook;
    auto nStyle = s.fontBook.queryFamily(family).len();
    auto fontface = fontBook.queryClosest({.family = family}).unwrap();

    Text::Font font{
        .fontface = fontface,
        .fontsize = 36,
    };

    return Ui::vflow(
               8,
               Ui::labelMedium(Ui::GRAY500, "{} · {} {}", family, nStyle, nStyle == 1 ? "Style" : "Styles"),
               Ui::text(Text::ProseStyle{font}, PANGRAM)
           ) |
           Ui::spacing({12, 8, 0, 8}) |
           Ui::hclip() |
           Ui::button(Model::bind<SelectFamily>(family), Ui::ButtonStyle::outline());
}

Ui::Child allFamiliesContent(State const &s) {
    Ui::Children children;
    auto &fontBook = s.fontBook;
    auto families = fontBook.families();
    for (auto const &family : families) {
        children.pushBack(allFamiliesItem(s, family));
    }

    return Ui::vflow(8, children) |
           Ui::spacing(16) |
           Ui::vscroll();
}

// MARK: Family ----------------------------------------------------------------

Ui::Child fontfaceTag(Str str) {
    return Kr::badge(Ui::GRAY400, Io::toParamCase(str).unwrap());
}

Ui::Child fontfaceTags(Text::FontAttrs const &attrs) {
    Ui::Children children;
    if (attrs.monospace == Text::Monospace::YES) {
        children.pushBack(fontfaceTag("monospace"s));
    }

    if (attrs.style != Text::FontStyle::NORMAL) {
        children.pushBack(fontfaceTag(Io::toStr(attrs.style).unwrap()));
    }

    if (attrs.stretch != Text::FontStretch::NORMAL) {
        children.pushBack(fontfaceTag(Io::toStr(attrs.stretch).unwrap()));
    }

    if (attrs.weight != Text::FontWeight::REGULAR) {
        children.pushBack(fontfaceTag(Io::toStr(attrs.weight).unwrap()));
    }

    return Ui::hflow(4, children);
}

Ui::Child familyItem(State const &, Strong<Text::Fontface> fontface) {
    auto attrs = fontface->attrs();

    Text::Font font{
        .fontface = fontface,
        .fontsize = 36,
    };

    return Ui::vflow(
               8,
               Ui::labelMedium(Ui::GRAY500, "{}", attrs.family),
               Ui::text(Text::ProseStyle{font}, PANGRAM),
               fontfaceTags(attrs)
           ) |
           Ui::spacing({12, 8, 0, 8}) |
           Ui::hclip() |
           Ui::button(Model::bind<SelectFace>(fontface), Ui::ButtonStyle::outline());
}

Ui::Child familyContent(State const &s) {
    Ui::Children children;
    auto &fontBook = s.fontBook;
    auto fontfaces = fontBook.queryFamily(s.fontFamily.unwrap());

    auto header = Ui::labelSmall(s.fontFamily.unwrap()) | Ui::spacing({16, 6});

    for (auto const &fontface : fontfaces) {
        children.pushBack(familyItem(s, fontface));
    }

    return Ui::vflow(
        header,
        Ui::separator(),
        Ui::vflow(8, children) | Ui::spacing(16) | Ui::vscroll() | Ui::grow()
    );
}

// MARK: Fontface --------------------------------------------------------------

Ui::Child pangrams(Strong<Text::Fontface> fontface) {
    f64 size = 12;
    Ui::Children children;

    for (isize i = 0; i < 12; i++) {
        Text::Font font{
            .fontface = fontface,
            .fontsize = size,
        };
        children.pushBack(Ui::text(Text::ProseStyle{font}, PANGRAM));
        size *= 1.2;
    }

    return Ui::vflow(8, children) |
           Ui::spacing(16) |
           Ui::vhscroll();
}

Ui::Child fontfaceContent(State const &s) {
    auto fontface = s.fontFace.unwrap();
    auto attrs = fontface->attrs();

    return Ui::vflow(
        Ui::hflow(
            0,
            Math::Align::CENTER,
            Ui::labelSmall(attrs.normal() ? "{}" : "{}  · ", attrs.family),
            fontfaceTags(attrs)
        ) | Ui::spacing({16, 6}),
        Ui::separator(),
        pangrams(fontface)
    );
}

Ui::Child appContent(State const &s) {
    if (s.fontFace) {
        return fontfaceContent(s);
    } else if (s.fontFamily) {
        return familyContent(s);
    } else {
        return allFamiliesContent(s);
    }
}

Ui::Child app(Text::FontBook book) {
    return Ui::reducer<Model>(book, [](State const &s) {
        return scafold({
            .icon = Mdi::FORMAT_FONT,
            .title = "Fonts"s,
            .startTools = slots$(Ui::button(Model::bindIf<GoBack>(s.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT)),
            .body = slot$(appContent(s)),
        });
    });
}

} // namespace Hideo::Fonts

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    Text::FontBook book;
    co_try$(book.loadAll());
    co_return Ui::runApp(ctx, Hideo::Fonts::app(book));
}
