#include <globals.hpp>
#include <hooks/hooks.hpp>
#include <utils/utils.hpp>

#include <intrin.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

unsigned int Hooks::hk_get_current_health(void* rcx)
{
    static const auto get_player_health_ret_addr = []() -> Pointer {
        const auto sekiro = reinterpret_cast<std::uint8_t*>(
            GetModuleHandleA(nullptr)
        );
        if (!sekiro) {
            return {};
        }

        const auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(sekiro);
        const auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(sekiro + dos->e_lfanew);

        return utils::pattern_scan(
            sekiro + nt->OptionalHeader.BaseOfCode,
            nt->OptionalHeader.SizeOfCode,
            "48 8B CB 66 0F 6E F0 0F 5B F6 E8 ? ? ? ? 66 0F 6E C0 0F 5B C0 F3 0F 5E F0 0F 2F FE"
        );
    }();

    if (_ReturnAddress() == get_player_health_ret_addr) {
        globals::local_player = rcx;
    }

    const auto health = Hooks::instance()
                            .m_dtr_get_current_health
                            .get_original<decltype(&Hooks::hk_get_current_health)>()(rcx);
    if (rcx == globals::local_player) {
        globals::current_health = health;
    }

    return health;
}