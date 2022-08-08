#include "utils.hpp"
#include <cstdlib>
#include <optional>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

std::string utils::to_utf8(const std::wstring& str)
{
    int required_size = WideCharToMultiByte(
        CP_UTF8,
        0,
        str.c_str(),
        static_cast<int>(str.length()),
        nullptr,
        0,
        nullptr,
        nullptr
    );

    std::string ret(static_cast<std::size_t>(required_size), 0);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        str.c_str(),
        static_cast<int>(str.length()),
        &ret[0],
        required_size,
        nullptr,
        nullptr
    );

    return ret;
}

Pointer utils::pattern_scan(Pointer begin, std::size_t size, std::string_view pattern)
{
    if (!begin) {
        return {};
    }

    const auto bytes = pattern_to_bytes(pattern);
    if (bytes.empty()) {
        return {};
    }

    const auto end = begin.as<const std::uint8_t*>() + size;

    for (; begin != end; begin.self_offset(1)) {
        bool found = true;

        for (std::size_t i = 0; i < bytes.size(); ++i) {
            if (begin.offset(i) == end) {
                return {};
            }

            const auto& byte = bytes[i];
            if (byte.has_value() && *byte != begin.offset(i).as<const std::uint8_t&>()) {
                found = false;
                break;
            }
        }

        if (found) {
            return begin;
        }
    }

    return {};
}

std::vector<std::optional<std::uint8_t>> utils::pattern_to_bytes(std::string_view pattern)
{
    std::vector<std::optional<std::uint8_t>> ret;

    for (std::size_t i = 0, len = pattern.length(); i < len; ++i) {
        if (pattern[i] == '?') {
            i += 1;

            if (i < len && pattern[i] == '?') {
                i += 1;
            }

            ret.emplace_back(std::nullopt);
        } else if (i + 1 != len) {
            ret.emplace_back(static_cast<std::uint8_t>(std::strtoul(&pattern[i], nullptr, 16)));
            i += 2;
        } else {
            return {};
        }
    }

    return ret;
}
