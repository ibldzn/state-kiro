#pragma once

#include <cstdint>
#include <type_traits>

struct Pointer {
    Pointer() = default;

    Pointer(std::uintptr_t ptr)
        : m_ptr(ptr)
    {
    }

    Pointer(const void* ptr)
        : m_ptr(reinterpret_cast<std::uintptr_t>(ptr))
    {
    }

    explicit operator bool() const { return m_ptr != 0; }
    operator void*() const { return as<void*>(); }

    Pointer& self_offset(std::ptrdiff_t val)
    {
        *this = offset(val);
        return *this;
    }

    Pointer& self_jmp()
    {
        *this = jmp();
        return *this;
    }

    Pointer& self_rip()
    {
        *this = rip();
        return *this;
    }

    Pointer offset(std::ptrdiff_t val) const
    {
        return m_ptr + val;
    }

    Pointer jmp() const
    {
        // E9 ?? ?? ?? ??
        constexpr auto instruction_size = 5;
        const auto displacement = offset(1).as<const std::int32_t&>();

        return offset(instruction_size + displacement);
    }

    Pointer rip() const
    {
        constexpr auto offset_size = 4;
        const auto displacement = as<const std::int32_t&>();

        return offset(offset_size + displacement);
    }

    template <typename T>
    T as() const
    {
        using type = std::remove_cv_t<T>;
        if constexpr (std::is_lvalue_reference_v<type>) {
            return *reinterpret_cast<std::add_pointer_t<T>>(m_ptr);
        } else if constexpr (std::is_pointer_v<type>) {
            return reinterpret_cast<T>(m_ptr);
        } else {
            return static_cast<T>(m_ptr);
        }
    }

private:
    std::uintptr_t m_ptr = 0;
};
