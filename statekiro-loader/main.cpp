#include "defer.hpp"

#include <array>
#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <TlHelp32.h>

namespace fs = std::filesystem;

static std::optional<unsigned int> get_process_id(const char* exe_name)
{
    HANDLE snapshot { CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };
    if (snapshot == INVALID_HANDLE_VALUE) {
        return std::nullopt;
    }
    DEFER { CloseHandle(snapshot); };

    PROCESSENTRY32 entry { .dwSize = sizeof(PROCESSENTRY32) };

    if (!Process32First(snapshot, &entry)) {
        return std::nullopt;
    }

    do {
        if (std::string_view { entry.szExeFile } == exe_name) {
            return entry.th32ProcessID;
        }
    } while (Process32Next(snapshot, &entry));

    return std::nullopt;
}

static bool inject_dll(unsigned int process_id, std::string_view dll_path)
{
    if (!fs::exists(dll_path)) {
        return false;
    }

    const std::string path = fs::canonical(dll_path).string();

    HANDLE handle { OpenProcess(PROCESS_ALL_ACCESS, false, process_id) };
    if (!handle) {
        return false;
    }
    DEFER { CloseHandle(handle); };

    LPVOID remote_dll_path {
        VirtualAllocEx(
            handle,
            nullptr,
            path.length() + 1,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE
        ),
    };
    if (!remote_dll_path) {
        return false;
    }

    DEFER { VirtualFreeEx(handle, remote_dll_path, 0, MEM_RELEASE); };

    if (!WriteProcessMemory(
            handle,
            remote_dll_path,
            &path[0],
            path.length(),
            nullptr
        )) {
        return false;
    }

    HANDLE thread {
        CreateRemoteThread(
            handle,
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA),
            remote_dll_path,
            0,
            nullptr
        ),
    };
    if (!thread) {
        return false;
    }

    DEFER { CloseHandle(thread); };

    WaitForSingleObject(thread, INFINITE);

    return true;
}

// just windows things..
static bool enable_cmd_esc_sequence(DWORD handle)
{
    HANDLE std_handle = GetStdHandle(handle);
    if (std_handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD mode = 0;

    if (!GetConsoleMode(std_handle, &mode)) {
        return false;
    }

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(std_handle, mode)) {
        return false;
    }

    return true;
}

int main()
{
    DEFER { Sleep(3000); };

    const bool have_color = enable_cmd_esc_sequence(STD_OUTPUT_HANDLE) && enable_cmd_esc_sequence(STD_ERROR_HANDLE);

    const auto print = [&](fmt::color fg, std::string_view str) {
        if (have_color) {
            fmt::print(fmt::fg(fg), "{}", str);
        } else {
            fmt::print("{}", str);
        }
    };

    const auto eprint = [&](fmt::color fg, std::string_view str) {
        if (have_color) {
            fmt::print(stderr, fmt::fg(fg), "{}", str);
        } else {
            fmt::print(stderr, "{}", str);
        }
    };

    const auto sekiro = get_process_id("sekiro.exe");
    if (!sekiro.has_value()) {
        eprint(fmt::color::red, "[!] Sekiro is not running! Aborting..\n");
        return 1;
    }

    if (!get_process_id("Discord.exe").has_value()) {
        eprint(fmt::color::yellow, "[!] Discord is not running, continuing anyway..\n");
    }

    constexpr std::array DLLS = {
        "statekiro_imgui.dll",
        "statekiro_console.dll",
    };

    for (std::size_t i = 0; const auto dll : DLLS) {
        print(fmt::color::blue, fmt::format("[{}] {}\n", ++i, dll));
    }

    const auto dll = [&] {
        while (true) {
            print(fmt::color::blue, "Choose DLL to inject: ");

            std::size_t choice = 0;
            std::cin >> choice;

            if (std::cin.fail() || choice == 0 || choice - 1 >= DLLS.size()) {
                eprint(fmt::color::red, "[!] Invalid choice!\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return DLLS[choice - 1];
            }
        }
    }();

    if (!fs::exists(dll)) {
        print(fmt::color::red, fmt::format("[!] \"{}\" does not exists. ", dll));
        print(fmt::color::red, "Please make sure the DLL is in the same directory as the executable.\n");
        return 1;
    }

    if (!inject_dll(*sekiro, dll)) {
        print(fmt::color::red, "[!] Failed to inject DLL! Aborting..\n");
        return 1;
    }

    print(fmt::color::green, "[✓] DLL injected successfully!");
}