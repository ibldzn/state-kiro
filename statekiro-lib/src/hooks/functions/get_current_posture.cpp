#include <globals.hpp>
#include <hooks/hooks.hpp>

unsigned int Hooks::hk_get_current_posture(void* rcx)
{
    const auto posture = Hooks::instance()
                             .m_dtr_get_current_posture
                             .get_original<decltype(&Hooks::hk_get_current_posture)>()(rcx);

    if (rcx == globals::local_player) {
        globals::current_posture = posture;
    }

    return posture;
}