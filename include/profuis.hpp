#pragma once

#include <windows.h>

namespace profuis
{
    struct CObject
    {
        char padding_00[64];
        HWND hwnd;
    };

    enum class box_state
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
        int padding_08;
        CObject *object;
    };
} // namespace profuis
