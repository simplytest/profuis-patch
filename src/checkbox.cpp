#include "checkbox.hpp"

#include "logger.hpp"
#include "uia.hpp"

#include <lockpp/lock.hpp>
#include <unordered_map>

namespace simplytest
{
    using profuis::box_state;

    struct checkbox::impl
    {
        using map_t = std::unordered_map<HWND, std::shared_ptr<checkbox>>;

      public:
        static inline lockpp::lock<map_t> instances;

      public:
        lockpp::lock<box_state> state;
        HWND hwnd;

      public:
        WNDPROC wnd_proc;
        checkbox_uia *provider;

      public:
        static LRESULT hk_wndproc(HWND, UINT, WPARAM, LPARAM);
    };

    LRESULT checkbox::impl::hk_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
        auto locked = instances.write();

        if (!locked->contains(hwnd))
        {
            return 0;
        }

        auto &self = locked->at(hwnd)->m_impl;
        auto addr  = reinterpret_cast<std::uintptr_t>(hwnd);

        if (message == WM_DESTROY)
        {
            logger::get()->trace("[{:x}] received WM_DESTROY", addr);
            locked->erase(hwnd);

            return 0;
        }

        if (message == WM_GETOBJECT)
        {
            logger::get()->trace("[{:x}] received WM_GETOBJECT", addr);
            return uia_core::get().UiaReturnRawElementProvider(hwnd, wparam, lparam, self->provider);
        }

        return CallWindowProcW(self->wnd_proc, hwnd, message, wparam, lparam);
    }

    checkbox::checkbox() : m_impl(std::make_unique<impl>()) {}

    checkbox::~checkbox()
    {
        m_impl->provider->Release();
    }

    void checkbox::toggle() const
    {
        // TODO: Implement
    }

    HWND checkbox::hwnd() const
    {
        return m_impl->hwnd;
    }

    bool checkbox::checked() const
    {
        auto state = m_impl->state.copy();

        // Why couldn't they just use proper bitflags?!

        return state == box_state::BOX_CHECKED ||          //
               state == box_state::BOX_LDOWN_CHECKED ||    //
               state == box_state::BOX_DISABLED_CHECKED || //
               state == box_state::BOX_MOUSE_HOVER_CHECKED;
    }

    box_state checkbox::state() const
    {
        return m_impl->state.copy();
    }

    void checkbox::update(checkable_data *data)
    {
        auto instances = impl::instances.write();
        auto *hwnd     = data->object->hwnd;

        if (!instances->contains(hwnd))
        {
            logger::get()->info(L"creating instance {:x} (content: '{}')", reinterpret_cast<std::uintptr_t>(hwnd),
                                data->text);

            auto state = std::shared_ptr<checkbox>(new checkbox);

            state->m_impl->hwnd = hwnd;
            state->m_impl->state.assign(data->state);
            state->m_impl->provider = new checkbox_uia(state);

            auto wnd_proc = GetWindowLongPtrW(hwnd, GWLP_WNDPROC);

            state->m_impl->wnd_proc = reinterpret_cast<WNDPROC>(wnd_proc);
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(impl::hk_wndproc));

            instances->emplace(hwnd, std::move(state));
        }

        auto &instance = instances->at(hwnd);

        if (instance->state() == data->state)
        {
            return;
        }

        const auto was_checked = instance->checked();
        instances->at(hwnd)->m_impl->state.assign(data->state);

        if (!uia_core::get().UiaClientsAreListening() || (was_checked && instance->checked()))
        {
            return;
        }

        uia_core::get().UiaRaiseAutomationEvent(instance->m_impl->provider, UIA_AutomationPropertyChangedEventId);
    }
} // namespace simplytest
