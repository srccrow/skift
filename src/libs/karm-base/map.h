#pragma once

#include "std.h"
#include "vec.h"

namespace Karm {

template <typename K, typename V>
struct Map {
    Vec<Cons<K, V>> _els{};

    Map() = default;

    Map(std::initializer_list<Cons<K, V>> &&list)
        : _els(std::move(list)) {}

    void put(K const &key, V value) {
        for (auto &i : ::mutIter(_els)) {
            if (i.car == key) {
                i.cdr = std::move(value);
                return;
            }
        }

        _els.pushBack(Cons<K, V>{key, std::move(value)});
    }

    V &get(K const &key) {
        for (auto &i : _els) {
            if (i.car == key) {
                return i.cdr;
            }
        }

        panic("key not found");
    }

    V take(K const &key) {
        for (usize i = 0; i < _els.len(); i++) {
            if (_els[i].car == key) {
                V value = std::move(_els[i].cdr);
                _els.removeAt(i);
                return value;
            }
        }

        panic("key not found");
    }

    Opt<V> tryGet(K const &key) const {
        for (auto &i : _els) {
            if (i.car == key) {
                return i.cdr;
            }
        }

        return NONE;
    }

    bool del(K const &key) {
        for (usize i = 0; i < _els.len(); i++) {
            if (_els[i].car == key) {
                _els.removeAt(i);
                return true;
            }
        }

        return false;
    }

    bool removeAll(V const &value) {
        bool changed = false;

        for (usize i = 1; i < _els.len() + 1; i++) {
            if (_els[i - 1].cdr == value) {
                _els.removeAt(i - 1);
                changed = true;
                i--;
            }
        }

        return changed;
    }

    bool removeFirst(V const &value) {
        for (usize i = 1; i < _els.len() + 1; i++) {
            if (_els[i - 1].cdr == value) {
                _els.removeAt(i - 1);
                return true;
            }
        }

        return false;
    }

    auto iter() {
        return mutIter(_els);
    }

    auto iter() const {
        return ::iter(_els);
    }

    V at(usize index) const {
        return _els[index].cdr;
    }

    usize len() const {
        return _els.len();
    }

    void clear() {
        _els.clear();
    }
};

} // namespace Karm
