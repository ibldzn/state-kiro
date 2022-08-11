#pragma once

#include <cstdint>
#include <functional>
#include <singleton.hpp>
#include <vector>

struct Input : Singleton<Input> {
    using callback_t = std::function<void(void* hwnd, std::uint32_t msg, std::uintptr_t wparam, std::intptr_t lparam)>;

    bool initialize(void* window);
    void register_wndproc_callback(callback_t callback);

private:
    friend struct Singleton<Input>;
    Input() = default;
    ~Input();

    static std::intptr_t wndproc_callback(void* hwnd, std::uint32_t msg, std::uintptr_t wparam, std::intptr_t lparam);

private:
    void* m_window = nullptr;
    void* m_original_wndproc = nullptr;
    std::vector<callback_t> m_wndproc_callbacks = {};
};