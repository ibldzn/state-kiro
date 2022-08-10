#include "input.hpp"
#include <mutex>
#include <utility>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

Input::~Input()
{
    if (m_original_wndproc) {
        SetWindowLongPtr(reinterpret_cast<HWND>(m_window), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_original_wndproc));
        m_original_wndproc = nullptr;
    }
}

bool Input::initialize(void* window)
{
    if (!m_original_wndproc) {
        m_window = window;
        m_original_wndproc = reinterpret_cast<void*>(
            SetWindowLongPtr(
                reinterpret_cast<HWND>(m_window),
                GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(wndproc_callback)
            )
        );
    }

    return m_original_wndproc != nullptr;
}

void Input::register_wndproc_callback(callback_t callback)
{
    static std::mutex mtx;
    std::lock_guard lock(mtx);
    m_wndproc_callbacks.emplace_back(std::move(callback));
}

std::intptr_t Input::wndproc_callback(void* hwnd, std::uint32_t msg, std::uintptr_t wparam, std::intptr_t lparam)
{
    const auto& input = Input::instance();

    for (const auto& cb : input.m_wndproc_callbacks) {
        cb(hwnd, msg, wparam, lparam);
    }

    return CallWindowProc(
        reinterpret_cast<WNDPROC>(input.m_original_wndproc),
        reinterpret_cast<HWND>(hwnd),
        msg,
        wparam,
        lparam
    );
}
