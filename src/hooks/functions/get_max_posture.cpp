#include "../../globals.hpp"
#include "../hooks.hpp"

unsigned int Hooks::hk_get_max_posture(void* rcx)
{
    const auto max_posture = Hooks::instance()
                                 .m_dtr_get_current_posture
                                 .get_original<decltype(&Hooks::hk_get_max_posture)>()(rcx);

    if (rcx == globals::local_player) {
        globals::max_posture = max_posture;
    }

    return max_posture;
}