#include <MinHook.h>
#include <atomic>
#include <hooks/detour.hpp>

static std::atomic_size_t ref_count = 0;

Detour::~Detour()
{
    remove();
}

bool Detour::create(void* target, void* detour)
{
    m_target = target;
    m_detour = detour;
    return create();
}

bool Detour::create()
{
    if (m_is_hooked) {
        return true;
    }

    if (!m_target || !m_detour) {
        return false;
    }

    if (ref_count++ == 0 && MH_Initialize() != MH_OK) {
        return false;
    }

    if (MH_CreateHook(m_target, m_detour, &m_original) != MH_OK) {
        return false;
    }

    if (MH_EnableHook(m_target) != MH_OK) {
        return false;
    }

    m_is_hooked = true;

    return true;
}

bool Detour::remove()
{
    if (!m_is_hooked) {
        return true;
    }

    if (MH_RemoveHook(m_target) != MH_OK) {
        return false;
    }

    m_is_hooked = false;

    if (--ref_count == 0 && MH_Uninitialize() != MH_OK) {
        return false;
    }

    return true;
}

bool Detour::is_hooked() const
{
    return m_is_hooked;
}
