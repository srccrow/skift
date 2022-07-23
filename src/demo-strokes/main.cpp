#include <karm-app/host.h>
#include <karm-main/main.h>
#include <karm-math/rand.h>

struct CircleClient : public Karm::App::Client {
    bool _trace{};
    Math::Vec2i _mousePos{200, 300};

    void paint(Gfx::Context &g) override {

        g.clear(Gfx::ZINC900);

        g.begin();
        g.moveTo({200, 200});
        g.lineTo(_mousePos.cast<double>());
        g.lineTo({200, 600});
        g.close();

        g.strokeStyle(Gfx::stroke(Gfx::BLUE500)
                          .with(Gfx::StrokeStyle::Align::CENTER)
                          .with(Gfx::StrokeStyle::Join::MITER)
                          .with(72));
        g.stroke();
        if (_trace)
            g._trace();
    }

    void handle(Events::Event &e) override {
        e.handle<Events::MouseEvent>([&](auto const &e) {
            if (e.type == Events::MouseEvent::RELEASE) {
                _trace = !_trace;
            }
            _mousePos = e.pos;
            return true;
        });
    }
};

ExitCode entryPoint(CliArgs const &) {
    return try$(App::makeHost(makeBox<CircleClient>()))->run();
}