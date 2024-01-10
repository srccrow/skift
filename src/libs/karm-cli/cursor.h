#pragma once

#include <karm-io/fmt.h>

namespace Karm::Cli {

struct Cmd {
    enum Type {
        SAVE,
        RESTORE,

        UP,
        DOWN,
        FORWARD,
        BACKWARD,
        NEXTLINE,
        PREVLINE,
        HORIZONTAL,
        POSITION,
        HIDE,
        SHOW,

        CLEAR_DISPLAY,
        CLEAR_DISPLAY_AFTER,
        CLEAR_DISPLAY_BEFORE,
        CLEAR_LINE,
        CLEAR_LINE_AFTER,
        CLEAR_LINE_BEFORE,
    };

    Type _type;
    int _row = 0;
    int _col = 0;

    static Cmd save() {
        return {SAVE};
    }

    static Cmd restore() {
        return {RESTORE};
    }

    static Cmd up(int count) {
        return {UP, count};
    }

    static Cmd down(int count) {
        return {DOWN, count};
    }

    static Cmd forward(int count) {
        return {FORWARD, 0, count};
    }

    static Cmd backward(int count) {
        return {BACKWARD, 0, count};
    }

    static Cmd nextline(int count) {
        return {NEXTLINE, count};
    }

    static Cmd prevline(int count) {
        return {PREVLINE, count};
    }

    static Cmd horizontal(int count) {
        return {HORIZONTAL, 0, count};
    }

    static Cmd position(int row, int col) {
        return {POSITION, row, col};
    }

    static Cmd hide() {
        return {HIDE};
    }

    static Cmd show() {
        return {SHOW};
    }

    static Cmd clearDisplay() {
        return {CLEAR_DISPLAY};
    }

    static Cmd clearDisplayAfter() {
        return {CLEAR_DISPLAY_AFTER};
    }

    static Cmd clearDisplayBefore() {
        return {CLEAR_DISPLAY_BEFORE};
    }

    static Cmd clearLine() {
        return {CLEAR_LINE};
    }

    static Cmd clearLineAfter() {
        return {CLEAR_LINE_AFTER};
    }

    static Cmd clearLineBefore() {
        return {CLEAR_LINE_BEFORE};
    }

    Res<usize> write(Io::TextWriter &writer) const {
#ifdef __ck_karm_cli_backend_ansi__
        switch (_type) {
        case Cli::Cmd::SAVE:
            return Io::format(writer, "\x1b[s");

        case Cli::Cmd::RESTORE:
            return Io::format(writer, "\x1b[u");

        case Cli::Cmd::UP:
            return Io::format(writer, "\x1b[{}A", _row);

        case Cli::Cmd::DOWN:
            return Io::format(writer, "\x1b[{}B", _row);

        case Cli::Cmd::FORWARD:
            return Io::format(writer, "\x1b[{}C", _col);

        case Cli::Cmd::BACKWARD:
            return Io::format(writer, "\x1b[{}D", _col);

        case Cli::Cmd::NEXTLINE:
            return Io::format(writer, "\x1b[{}E", _row);

        case Cli::Cmd::PREVLINE:
            return Io::format(writer, "\x1b[{}F", _row);

        case Cli::Cmd::HORIZONTAL:
            return Io::format(writer, "\x1b[{}G", _col);

        case Cli::Cmd::POSITION:
            return Io::format(writer, "\x1b[{};{}H", _row, _col);

        case Cli::Cmd::HIDE:
            return Io::format(writer, "\x1b[?25l");

        case Cli::Cmd::SHOW:
            return Io::format(writer, "\x1b[?25h");

        case Cli::Cmd::CLEAR_DISPLAY:
            return Io::format(writer, "\x1b[2J");

        case Cli::Cmd::CLEAR_DISPLAY_AFTER:
            return Io::format(writer, "\x1b[J");

        case Cli::Cmd::CLEAR_DISPLAY_BEFORE:
            return Io::format(writer, "\x1b[1J");

        case Cli::Cmd::CLEAR_LINE:
            return Io::format(writer, "\x1b[2K");

        case Cli::Cmd::CLEAR_LINE_AFTER:
            return Io::format(writer, "\x1b[K");

        case Cli::Cmd::CLEAR_LINE_BEFORE:
            return Io::format(writer, "\x1b[1K");

        default:
            panic("Invalid cursor move type");
        }
#endif

        (void)writer;
        return Ok(0uz);
    }
};

} // namespace Karm::Cli

template <>
struct Karm::Io::Formatter<Cli::Cmd> {
    Res<usize> format(Io::TextWriter &writer, Cli::Cmd const &move) {
        return move.write(writer);
    }
};
