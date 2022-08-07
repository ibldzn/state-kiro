#include "../hooks.hpp"

/*HRESULT*/ long Hooks::hk_present(IDXGISwapChain* swap_chain, unsigned int sync_interval, unsigned int flags)
{
    return Hooks::instance()
        .m_original_present
        .as<decltype(&Hooks::hk_present)>()(swap_chain, sync_interval, flags);
}