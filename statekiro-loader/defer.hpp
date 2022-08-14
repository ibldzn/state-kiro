#pragma once

#include <utility>

template <typename Fn>
struct Defer {
    Defer(Fn&& fn)
        : m_fn(std::forward<Fn>(fn))
    {
    }
    ~Defer() { m_fn(); }
    Defer(const Defer&) = delete;
    Fn m_fn = {};
};

static struct {
    template <typename Fn>
    Defer<Fn> operator<<(Fn&& fn) { return Defer(std::forward<Fn>(fn)); }
} deferrer;

#define DEFER_PASTE_(x, y) x##y
#define DEFER_PASTE(x, y) DEFER_PASTE_(x, y)
#define DEFER auto DEFER_PASTE(deferrer_lambda__, __COUNTER__) = deferrer << [&]