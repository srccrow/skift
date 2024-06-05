#include <grund-echo/api.h>
#include <karm-ipc/ipc.h>
#include <karm-sys/entry.h>

namespace Grund::Echo {

struct Service : public Ipc::Object<Grund::IEcho> {
    using Ipc::Object<Grund::IEcho>::Object;

    Async::Task<String> echoAsync(String msg) override {
        co_return msg;
    }
};

} // namespace Grund::Echo

Async::Task<> entryPointAsync(Sys::Ctx &ctx) {
    auto server = co_try$(Ipc::Server::create(ctx));
    Grund::Echo::Service service{server};
    co_return co_trya$(server.runAsync());
}