#pragma once

#include "constants.hpp"

#include <cstdint>
#include <windows.h>

namespace profuis
{
    static constexpr auto hwnd_offset   = simplytest::arch == simplytest::architecture::x64 ? 0x40 : 0x20;
    static constexpr auto state_padding = simplytest::arch == simplytest::architecture::x64 ? 0x04 : 0x00;

    struct c_object
    {
        char padding_00[hwnd_offset];
        HWND hwnd;
    };

    enum class box_state : std::uint32_t
    {
        BOX_UNCHECKED                 = 0x0,
        BOX_CHECKED                   = 0x1,
        BOX_INDETERMINATE             = 0x2,
        BOX_LDOWN_UNCHECKED           = 0x3,
        BOX_LDOWN_CHECKED             = 0x4,
        BOX_LDOWN_INDETERMINATE       = 0x5,
        BOX_MOUSE_HOVER_UNCHECKED     = 0x6,
        BOX_MOUSE_HOVER_CHECKED       = 0x7,
        BOX_MOUSE_HOVER_INDETERMINATE = 0x8,
        BOX_DISABLED_UNCHECKED        = 0x9,
        BOX_DISABLED_CHECKED          = 0xA,
        BOX_DISABLED_INDETERMINATE    = 0xB,
    };

    struct checkable_data
    {
        box_state state;
        char padding[state_padding];
        c_object *object;
        LPARAM lparam;
        RECT client_rect;
        LPCWSTR text;
    };
} // namespace profuis
