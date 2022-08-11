#include <globals.hpp>
#include <hooks/hooks.hpp>
#include <statekiro.hpp>

/*HRESULT*/ long Hooks::hk_present(IDXGISwapChain* swap_chain, unsigned int sync_interval, unsigned int flags)
{
    for (const auto& cb : statekiro::get_present_callbacks()) {
        cb(swap_chain);
    }

    return Hooks::instance()
        .m_original_present
        .as<decltype(&Hooks::hk_present)>()(swap_chain, sync_interval, flags);
}