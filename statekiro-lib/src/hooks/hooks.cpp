#include <hooks/hooks.hpp>
#include <utils/utils.hpp>

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <windows.h>

static Pointer get_dxgi_present();

Hooks::~Hooks()
{
    unhook_present();

    m_dtr_get_current_health.remove();
    m_dtr_get_current_posture.remove();

    m_dtr_get_max_health.remove();
    m_dtr_get_max_posture.remove();

    m_dtr_sub_140E302B0.remove();
}

bool Hooks::initialize()
{
    const auto sekiro = reinterpret_cast<std::uint8_t*>(GetModuleHandleA(nullptr));
    if (!sekiro) {
        return false;
    }

    const auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(sekiro);
    const auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(sekiro + dos->e_lfanew);

    const auto scan_begin = sekiro + nt->OptionalHeader.BaseOfCode;
    const auto scan_size = nt->OptionalHeader.SizeOfCode;

#define MAKE_HOOK(name, pattern, after) (m_dtr_##name.create(utils::pattern_scan(scan_begin, scan_size, pattern) after, reinterpret_cast<void*>(hk_##name)))

    if (!MAKE_HOOK(get_current_health, "E8 ? ? ? ? 8B 0E 3B C8", .self_jmp())) {
        return false;
    }

    if (!MAKE_HOOK(get_current_posture, "E8 ? ? ? ? 8B 4F 38 3B C1", .self_jmp())) {
        return false;
    }

    if (!MAKE_HOOK(get_max_health, "E8 ? ? ? ? 89 46 1C 8B D0", .self_jmp())) {
        return false;
    }

    if (!MAKE_HOOK(get_max_posture, "E8 ? ? ? ? 89 46 38", .self_jmp())) {
        return false;
    }

    if (!MAKE_HOOK(sub_140E302B0, "41 0F 94 C1 48 8D 45 A7", .self_offset(0x34).self_jmp())) {
        return false;
    }

#undef MAKE_HOOK

    if (!hook_present(true) && !hook_present(false)) {
        return false;
    }

    return true;
}

bool Hooks::hook_present(bool stream_proof)
{
    unhook_present();

    if (stream_proof) {
        const auto discord_original_present = []() -> Pointer {
            const auto discord_hook = reinterpret_cast<
                std::uint8_t*>(
                GetModuleHandleA("DiscordHook64.dll")
            );
            if (!discord_hook) {
                return {};
            }

            const auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(discord_hook);
            const auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(discord_hook + dos->e_lfanew);

            return utils::pattern_scan(
                       discord_hook + nt->OptionalHeader.BaseOfCode,
                       nt->OptionalHeader.SizeOfCode,
                       "41 89 F0 FF 15 ? ? ? ? 89 C6 E8 ? ? ? ?"
            )
                .self_offset(0x5)
                .self_rip();
        }();

        if (!discord_original_present) {
            return false;
        }

        m_discord_original_present = discord_original_present;
        m_original_present = discord_original_present.as<const void*&>();
        discord_original_present.as<void*&>() = reinterpret_cast<void*>(hk_present);
    } else {
        if (!m_dtr_present.create(
                get_dxgi_present(),
                reinterpret_cast<void*>(hk_present)
            )) {
            return false;
        }

        m_original_present = m_dtr_present.get_original<void*>();
    }

    return true;
}

void Hooks::unhook_present()
{
    if (m_discord_original_present) {
        m_discord_original_present.as<void*&>() = m_original_present;
        m_discord_original_present = nullptr;
    } else if (m_dtr_present.is_hooked()) {
        m_dtr_present.remove();
    }
}

static Pointer get_dxgi_present()
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = GetForegroundWindow();
    scd.SampleDesc.Count = 1;
    scd.Windowed = true;
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    const auto d3d11 = GetModuleHandleA("d3d11.dll");
    if (!d3d11) {
        return {};
    }

    const auto create_dev_and_sc = reinterpret_cast<
        decltype(&D3D11CreateDeviceAndSwapChain)>(
        GetProcAddress(
            d3d11,
            "D3D11CreateDeviceAndSwapChain"
        )
    );

    D3D_FEATURE_LEVEL lvl = D3D_FEATURE_LEVEL_11_0;
    ID3D11Device* device = nullptr;
    IDXGISwapChain* swap_chain = nullptr;
    ID3D11DeviceContext* device_context = nullptr;

    if (FAILED(
            create_dev_and_sc(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                0,
                &lvl,
                1,
                D3D11_SDK_VERSION,
                &scd,
                &swap_chain,
                &device,
                nullptr,
                &device_context
            )
        )) {
        return {};
    }

    const auto swap_chain_vtable = *reinterpret_cast<void***>(swap_chain);
    const auto present = swap_chain_vtable[8];

    if (device) {
        device->Release();
    }

    if (swap_chain) {
        swap_chain->Release();
    }

    if (device_context) {
        device_context->Release();
    }

    return present;
}