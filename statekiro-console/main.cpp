#include <chrono>
#include <iostream>
#include <statekiro/statekiro.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static DWORD WINAPI on_attach(LPVOID mod)
{
    const auto die_with_msgbox = [&](const char* text) {
        MessageBoxA(nullptr, text, "Error", MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(mod), 1);
    };

    if (!statekiro::initialize()) {
        die_with_msgbox("Failed to initialize mod!");
    }

    if (!AllocConsole()) {
        die_with_msgbox("Failed to allocate console!");
    }

    if (freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout) != 0) {
        die_with_msgbox("Failed to open stdout!");
    }

    using hrc = std::chrono::high_resolution_clock;
    auto last_update = hrc::time_point {};

    while (!(GetAsyncKeyState(VK_DELETE) & 0x8000)) {
        const auto now = hrc::now();
        if ((std::chrono::duration_cast<std::chrono::seconds>(now - last_update)).count() < 1) {
            continue;
        }
        system("cls");
        std::cout << "Health : [" << statekiro::get_current_health() << '/' << statekiro::get_max_health() << "]\n";
        std::cout << "Posture: [" << statekiro::get_current_posture() << '/' << statekiro::get_max_posture() << "]\n";
        last_update = now;
    }

    FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(mod), 0);
}

static BOOL WINAPI on_detach()
{
    statekiro::uninitialize();
    fclose(stdout);
    FreeConsole();
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE mod, DWORD call_reason, [[maybe_unused]] LPVOID reserved)
{
    if (call_reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(mod);
        if (HANDLE thread = CreateThread(nullptr, 0, on_attach, mod, 0, nullptr)) {
            CloseHandle(thread);
            return TRUE;
        }
    } else if (call_reason == DLL_PROCESS_DETACH) {
        return on_detach();
    }
    return FALSE;
}