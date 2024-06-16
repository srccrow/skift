#include <hideo-base/scafold.h>
#include <hideo-base/sidenav.h>

#include "app.h"
#include "model.h"

// Pages
#include "page-avatar.h"
#include "page-badge.h"

namespace Hideo::Zoo {

static Array PAGES = {
    &PAGE_AVATAR,
    &PAGE_BADGE
};

Ui::Child app() {
    return Ui::reducer<Model>([](State const &s) {
        return scafold({
            .icon = Mdi::DUCK,
            .title = "Zoo"s,
            .sidebar = [&] {
                return Hideo::sidenav(
                    iter(PAGES)
                        .mapi([&](Page const *page, usize index) {
                            return Hideo::sidenavItem(
                                index == s.page,
                                Model::bind<Switch>(index),
                                page->icon,
                                page->name
                            );
                        })
                        .collect<Ui::Children>()
                );
            },
            .body = [&] {
                auto &page = PAGES[s.page];
                return Ui::vflow(
                           Ui::titleLarge(page->name),
                           Ui::empty(4),
                           Ui::bodyMedium(page->description),
                           Ui::empty(16),
                           page->build() |
                               Ui::center() |
                               Ui::minSize({Ui::UNCONSTRAINED, 320}) |
                               Ui::box({
                                   .borderRadius = 8,
                                   .borderWidth = 1,
                                   .borderPaint = Ui::GRAY800,
                               })
                       ) |
                       Ui::spacing({16, 24});
            },
        });
    });
}

} // namespace Hideo::Zoo