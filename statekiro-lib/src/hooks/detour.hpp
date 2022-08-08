#pragma once

struct Detour {
    Detour() = default;
    Detour(const Detour&) = delete;
    Detour& operator=(const Detour&) = delete;
    ~Detour();
    Detour(void* target, void* detour)
        : m_is_hooked(false)
        , m_target(target)
        , m_detour(detour)
        , m_original(nullptr)
    {
    }

    bool create(void* target, void* detour);
    bool create();
    bool remove();
    bool is_hooked() const;

    template <typename T>
    T get_original() const { return (T)m_original; }

private:
    bool m_is_hooked = false;
    void* m_target = nullptr;
    void* m_detour = nullptr;
    void* m_original = nullptr;
};