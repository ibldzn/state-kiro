#include <globals.hpp>
#include <hooks/hooks.hpp>

unsigned int Hooks::hk_get_max_health(void* rcx)
{
    const auto max_health = Hooks::instance()
                                .m_dtr_get_max_health
                                .get_original<decltype(&Hooks::hk_get_max_health)>()(rcx);

    if (rcx == globals::local_player) {
        globals::max_health = max_health;
    }

    return max_health;
}