#include <karm-logger/logger.h>
#include <karm-sys/async.h>
#include <karm-sys/proc.h>
#include <karm-test/macros.h>

namespace Karm::Sys::Tests {

Async::Task<> sleepyBoy() {
    auto duration = TimeSpan::fromSecs(3);
    auto start = Sys::now();
    co_try_await$(globalSched().sleepAsync(start + duration));
    auto end = Sys::now();

    logDebug("slept for {}", end - start);

    if (end - start < duration - TimeSpan::fromMSecs(100))
        co_return Error::other("sleepAsync woke up too early");

    if (end - start > duration + TimeSpan::fromMSecs(100))
        co_return Error::other("sleepAsync woke up too late");

    co_return Ok();
}

test$(sleepAsync) {
    return Sys::run(sleepyBoy());
}

} // namespace Karm::Sys::Tests