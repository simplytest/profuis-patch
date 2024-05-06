#pragma once

#include <cstdint>

namespace simplytest
{
    enum class architecture
    {
        x86,
        x64,
    };

    static constexpr inline architecture arch = sizeof(std::uintptr_t) == 8 ? architecture::x64 : architecture::x86;
} // namespace simplytest
