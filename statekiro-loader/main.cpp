#include "defer.hpp"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

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

int main([[maybe_unused]] int argc, char** argv)
{
    DEFER { Sleep(3000); };

    const bool have_color = enable_cmd_esc_sequence(STD_OUTPUT_HANDLE) && enable_cmd_esc_sequence(STD_ERROR_HANDLE);

    const auto println = [&]<typename... Args>(fmt::color fg, std::string_view format_str, Args&&... args) {
        if (have_color) {
            fmt::print(
                fmt::fg(fg),
                "{}\n",
                fmt::format(
                    fmt::runtime(format_str),
                    std::forward<Args>(args)...
                )
            );
        } else {
            fmt::print(
                "{}\n",
                fmt::format(
                    fmt::runtime(format_str),
                    std::forward<Args>(args)...
                )
            );
        }
    };

    const auto eprintln = [&]<typename... Args>(fmt::color fg, std::string_view format_str, Args&&... args) {
        if (have_color) {
            fmt::print(
                stderr,
                fmt::fg(fg),
                "{}\n",
                fmt::format(
                    fmt::runtime(format_str),
                    std::forward<Args>(args)...
                )
            );
        } else {
            fmt::print(
                stderr,
                "{}\n",
                fmt::format(
                    fmt::runtime(format_str),
                    std::forward<Args>(args)...
                )
            );
        }
    };

    println(fmt::color::alice_blue, "[+] Looking for Sekiro...");

    const auto sekiro = get_process_id("sekiro.exe");
    if (!sekiro.has_value()) {
        eprintln(fmt::color::red, "[!] Sekiro is not running! Aborting..");
        return 1;
    } else {
        println(fmt::color::alice_blue, "[+] Found Sekiro with PID: {}", *sekiro);
    }

    println(fmt::color::alice_blue, "[+] Looking for Discord...");

    if (const auto discord = get_process_id("Discord.exe"); !discord.has_value()) {
        eprintln(fmt::color::yellow, "[!] Discord is not running, continuing anyway..");
    } else {
        println(fmt::color::alice_blue, "[+] Found Discord with PID: {}", *discord);
    }

    println(fmt::color::alice_blue, "[+] Looking for DLLs in the executable directory...");

    const auto DLLS = [&] {
        std::vector<fs::path> ret;
        for (const auto& entry : fs::directory_iterator(fs::path(argv[0]).parent_path())) {
            const auto& path = entry.path();
            if (path.has_extension() && path.extension() == ".dll") {
                ret.emplace_back(path);
            }
        }
        return ret;
    }();

    if (DLLS.empty()) {
        eprintln(fmt::color::red, "[!] Failed to find DLLs, make sure they are in the same directory as the executable!");
        return 1;
    }

    for (std::size_t i = 0; const auto& dll : DLLS) {
        println(fmt::color::blue, "[{}] {}", ++i, dll.filename().string());
    }

    const auto dll = [&] {
        while (true) {
            println(fmt::color::blue, "Choose DLL to inject: ");

            std::size_t choice = 0;
            std::cin >> choice;

            if (std::cin.fail() || choice == 0 || choice - 1 >= DLLS.size()) {
                eprintln(fmt::color::red, "[!] Invalid choice!");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return DLLS[choice - 1];
            }
        }
    }();

    println(fmt::color::alice_blue, "[+] Injecting \"{}\"...", dll.filename().string());

    if (!inject_dll(*sekiro, dll.string())) {
        eprintln(fmt::color::red, "[!] Failed to inject DLL! Aborting..\n");
        return 1;
    }

    println(fmt::color::green, "[✓] DLL injected successfully!\n");
}