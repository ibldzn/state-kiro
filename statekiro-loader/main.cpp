#include <array>
#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <TlHelp32.h>

namespace fs = std::filesystem;
using SmartHandle = std::unique_ptr<void, decltype(&CloseHandle)>;

static std::optional<unsigned int> get_process_id(const char* exe_name)
{
    SmartHandle snapshot { CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0), &CloseHandle };
    if (!snapshot) {
        return std::nullopt;
    }

    PROCESSENTRY32 entry { .dwSize = sizeof(PROCESSENTRY32) };

    if (!Process32First(snapshot.get(), &entry)) {
        return std::nullopt;
    }

    do {
        if (std::string_view { entry.szExeFile } == exe_name) {
            return entry.th32ProcessID;
        }
    } while (Process32Next(snapshot.get(), &entry));

    return std::nullopt;
}

static bool inject_dll(unsigned int process_id, std::string_view dll_path)
{
    if (!fs::exists(dll_path)) {
        return false;
    }

    const std::string path = fs::canonical(dll_path).string();

    SmartHandle handle { OpenProcess(PROCESS_ALL_ACCESS, false, process_id), &CloseHandle };
    if (!handle) {
        return false;
    }

    const auto virtual_free_wrapper = [&](LPVOID ptr) {
        if (ptr) {
            VirtualFreeEx(handle.get(), ptr, 0, MEM_RELEASE);
        }
    };
    std::unique_ptr<void, decltype(virtual_free_wrapper)> remote_dll_path {
        VirtualAllocEx(
            handle.get(),
            nullptr,
            path.length() + 1,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE
        ),
        virtual_free_wrapper
    };
    if (!remote_dll_path) {
        return false;
    }

    if (!WriteProcessMemory(
            handle.get(),
            remote_dll_path.get(),
            &path[0],
            path.length(),
            nullptr
        )) {
        return false;
    }

    SmartHandle thread {
        CreateRemoteThread(
            handle.get(),
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA),
            remote_dll_path.get(),
            0,
            nullptr
        ),
        &CloseHandle
    };
    if (!thread) {
        return false;
    }

    WaitForSingleObject(thread.get(), INFINITE);

    return true;
}

int main()
{
    if (!get_process_id("Discord.exe").has_value()) {
        fmt::print(fmt::fg(fmt::color::yellow), "[!] Discord is not running, continuing anyway..\n");
    }

    const auto sekiro = get_process_id("sekiro.exe");
    if (!sekiro.has_value()) {
        fmt::print(fmt::fg(fmt::color::red), "[!] Sekiro is not running! Aborting..\n");
        return 1;
    }

    constexpr std::array DLLS = {
        "statekiro_imgui.dll",
        "statekiro_console.dll",
    };

    for (std::size_t i = 0; const auto dll : DLLS) {
        fmt::print(fmt::fg(fmt::color::blue), "[{}] {}\n", ++i, dll);
    }

    const auto dll = [&] {
        while (true) {
            fmt::print(fmt::fg(fmt::color::blue), "Choose DLL to inject: ");

            std::size_t choice = 0;
            std::cin >> choice;

            if (std::cin.fail() || choice == 0 || choice - 1 >= DLLS.size()) {
                fmt::print(fmt::fg(fmt::color::red), "[!] Invalid choice!\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return DLLS[choice - 1];
            }
        }
    }();

    if (!fs::exists(dll)) {
        fmt::print(fmt::fg(fmt::color::red), "[!] \"{}\" does not exists. ", dll);
        fmt::print(fmt::fg(fmt::color::red), "Please make sure the DLL is in the same directory as the executable.\n");
        return 1;
    }

    if (!inject_dll(*sekiro, dll)) {
        fmt::print(fmt::fg(fmt::color::red), "[!] Failed to inject DLL! Aborting..\n");
        return 1;
    }

    fmt::print("{}\n", fmt::format(fmt::fg(fmt::color::green), "[✓] DLL injected successfully!"));

    Sleep(3000);
}