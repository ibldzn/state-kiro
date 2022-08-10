#include <render/render.hpp>
#include <statekiro/statekiro.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static DWORD WINAPI on_attach(LPVOID mod)
{
    // while (!IsDebuggerPresent()) {
    //     Sleep(100);
    // }

    const auto die_with_msgbox = [&](const char* text) {
        MessageBoxA(nullptr, text, "Error", MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(mod), 1);
    };

    if (!statekiro::initialize()) {
        die_with_msgbox("Failed to initialize mod!");
    }

    statekiro::register_present_callback(Render::on_present);

    while (!(GetAsyncKeyState(VK_DELETE))) {
        Sleep(100);
    }

    FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(mod), 0);
}

static BOOL WINAPI on_detach()
{
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE mod, DWORD call_reason, [[maybe_unused]] LPVOID reserved)
{
    if (call_reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(mod);
        if (HANDLE thread = CreateThread(
                nullptr,
                0,
                on_attach,
                mod,
                0,
                nullptr
            )) {
            CloseHandle(thread);
            return TRUE;
        }
    } else if (call_reason == DLL_PROCESS_DETACH) {
        return on_detach();
    }
    return FALSE;
}