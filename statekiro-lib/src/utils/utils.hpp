#pragma once

#include "../pointer.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace utils {
    std::string to_utf8(const std::wstring& str);
    Pointer pattern_scan(Pointer begin, std::size_t size, std::string_view pattern);
    std::vector<std::optional<std::uint8_t>> pattern_to_bytes(std::string_view pattern);
}