#pragma once

#include "profuis.hpp"

#include <windows.h>
#include <memory>

namespace simplytest
{
    using profuis::box_state;
    using profuis::checkable_data;

    enum class checkable_type
    {
        check,
        radio,
    };

    class checkable
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        checkable();

      public:
        ~checkable();

      public:
        void toggle() const;

      public:
        [[nodiscard]] HWND hwnd() const;
        [[nodiscard]] bool checked() const;
        [[nodiscard]] box_state state() const;

      public:
        static void update(checkable_data *, checkable_type);
    };
} // namespace simplytest
