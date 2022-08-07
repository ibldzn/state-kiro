#include "../../globals.hpp"
#include "../../pointer.hpp"
#include "../../utils/utils.hpp"
#include "../hooks.hpp"

#include <intrin.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if !defined(__clang__)
extern "C" wchar_t* read_r10();
#endif

void* Hooks::hk_sub_140E302B0(void* rcx, void* rdx)
{
    static const auto get_prosthetic_ret_addr = []() -> Pointer {
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
                   "41 0F 94 C1 48 8D 45 A7"
        )
            .self_offset(0x39);
    }();

    const auto ret = Hooks::instance()
                         .m_dtr_sub_140E302B0
                         .get_original<decltype(&Hooks::hk_sub_140E302B0)>()(rcx, rdx);

    wchar_t* str = nullptr;

#if defined(__clang__)
    __asm mov str, r10;
#else
    str = read_r10();
#endif

    if (str && !IS_INTRESOURCE(str)) {
        if (_ReturnAddress() == get_prosthetic_ret_addr) {
            globals::current_prosthetic = utils::to_utf8(str);
        } else {
            globals::current_quick_item = utils::to_utf8(str);
        }
    }

    return ret;
}