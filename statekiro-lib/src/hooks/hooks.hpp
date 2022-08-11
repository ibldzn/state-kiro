#pragma once

#include <hooks/detour.hpp>
#include <pointer.hpp>
#include <utils/singleton.hpp>

struct IDXGISwapChain;

struct Hooks : utils::Singleton<Hooks> {
    bool initialize();
    void shutdown();

private:
    friend struct Singleton<Hooks>;
    Hooks() = default;
    bool hook_present(bool stream_proof);
    void unhook_present();

private:
    static unsigned int hk_get_max_health(void* rcx);
    static unsigned int hk_get_current_health(void* rcx);

    static unsigned int hk_get_max_posture(void* rcx);
    static unsigned int hk_get_current_posture(void* rcx);

    static void* hk_sub_140E302B0(void* rcx, void* rdx);

    static /*HRESULT*/ long hk_present(IDXGISwapChain* swap_chain, unsigned int sync_interval, unsigned int flags);

private:
    Detour m_dtr_get_max_health = {};
    Detour m_dtr_get_current_health = {};

    Detour m_dtr_get_max_posture = {};
    Detour m_dtr_get_current_posture = {};

    Detour m_dtr_sub_140E302B0 = {};

    Detour m_dtr_present = {};
    Pointer m_original_present = nullptr;
    // Discord's pointer to the original IDXGISwapChain::Present
    // Used for pointer swapping
    Pointer m_discord_original_present = nullptr;
};