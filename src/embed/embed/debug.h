#pragma once

#include <karm-io/traits.h>

#include "sys-defs.h"

namespace Embed {

void loggerLock();

void loggerUnlock();

Io::TextWriter<Encoding> &loggerOut();

} // namespace Embed
