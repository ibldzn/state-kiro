#include "../hooks.hpp"
#include <functional>
#include <mutex>
#include <statekiro.hpp>
#include <utility>
#include <vector>

static std::vector<std::function<void(IDXGISwapChain*)>> callbacks;

void statekiro::register_present_callback(std::function<void(IDXGISwapChain*)> callback)
{
    static std::mutex mtx;
    std::lock_guard lock(mtx);
    callbacks.emplace_back(std::move(callback));
}

/*HRESULT*/ long Hooks::hk_present(IDXGISwapChain* swap_chain, unsigned int sync_interval, unsigned int flags)
{
    for (const auto& cb : callbacks) {
        cb(swap_chain);
    }

    return Hooks::instance()
        .m_original_present
        .as<decltype(&Hooks::hk_present)>()(swap_chain, sync_interval, flags);
}