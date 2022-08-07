#include "hooks/hooks.hpp"
#include <cstdio>
#include <windows.h>

DWORD WINAPI on_attach(LPVOID mod)
{
#if defined(_DEBUG) && 0
    while (!IsDebuggerPresent()) {
        Sleep(100);
    }
#endif

    if (!Hooks::instance().initialize()) {
        MessageBoxA(nullptr, "Failed to initialize hooks!", "Error", MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(mod), 1);
    }

    while (!(GetAsyncKeyState(VK_END) & 0x8000)) {
        Sleep(100);
    }

    FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(mod), 0);
}

BOOL WINAPI on_detach()
{
    Hooks::instance().shutdown();
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
