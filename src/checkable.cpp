#include "checkable.hpp"

#include "logger.hpp"
#include "uia.hpp"

#include <lockpp/lock.hpp>
#include <unordered_map>
#include <mutex>

namespace simplytest
{
    using profuis::box_state;

    struct checkable::impl
    {
        using map_t = std::unordered_map<HWND, std::shared_ptr<checkable>>;

      public:
        static inline lockpp::lock<map_t, std::recursive_mutex> instances{};

      public:
        lockpp::lock<box_state> state;
        HWND hwnd;

      public:
        toggle_uia *provider;
        WNDPROC wnd_proc;

      public:
        static LRESULT hk_wndproc(HWND, UINT, WPARAM, LPARAM);
    };

    LRESULT checkable::impl::hk_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
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
            logger::get()->trace("[{:x}] received destroy signal", addr);
            locked->erase(hwnd);

            return 0;
        }

        if (message == WM_GETOBJECT)
        {
            logger::get()->trace("[{:x}] received accessibility request", addr);
            return uia_core::get().UiaReturnRawElementProvider(hwnd, wparam, lparam, self->provider);
        }

        return CallWindowProcW(self->wnd_proc, hwnd, message, wparam, lparam);
    }

    checkable::checkable() : m_impl(std::make_unique<impl>()) {}

    checkable::~checkable()
    {
        m_impl->provider->Release();
    }

    void checkable::toggle() const
    {
        logger::get()->debug("[{:x}] received toggle request");
        SendMessageW(m_impl->hwnd, BM_SETCHECK, checked() ? BST_UNCHECKED : BST_CHECKED, 0);
    }

    HWND checkable::hwnd() const
    {
        return m_impl->hwnd;
    }

    bool checkable::checked() const
    {
        auto state = m_impl->state.copy();

        // Why couldn't they just use proper bitflags?!

        return state == box_state::BOX_CHECKED ||          //
               state == box_state::BOX_LDOWN_CHECKED ||    //
               state == box_state::BOX_DISABLED_CHECKED || //
               state == box_state::BOX_MOUSE_HOVER_CHECKED;
    }

    box_state checkable::state() const
    {
        return m_impl->state.copy();
    }

    void checkable::update(checkable_data *data, checkable_type type)
    {
        auto instances = impl::instances.write();
        auto *hwnd     = data->object->hwnd;

        if (!instances->contains(hwnd))
        {
            logger::get()->info(L"creating instance {:x} (content: '{}')", reinterpret_cast<std::uintptr_t>(hwnd),
                                data->text);

            auto state = std::shared_ptr<checkable>(new checkable);

            state->m_impl->hwnd = hwnd;
            state->m_impl->state.assign(data->state);
            state->m_impl->provider = new toggle_uia(state, type);

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
        instance->m_impl->state.assign(data->state);

        if (!uia_core::get().UiaClientsAreListening() || (was_checked && instance->checked()))
        {
            return;
        }

        uia_core::get().UiaRaiseAutomationEvent(instance->m_impl->provider, UIA_AutomationPropertyChangedEventId);
    }
} // namespace simplytest
