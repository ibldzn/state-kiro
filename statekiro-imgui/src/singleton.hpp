#pragma once

template <typename T>
struct Singleton {
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static T& instance()
    {
        static T inst = {};
        return inst;
    }

protected:
    Singleton() = default;
};