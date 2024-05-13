#include "uia.hpp"

#include <atomic>
#include <lime/module.hpp>

namespace simplytest
{
    uia_core::uia_core() = default;

    uia_core &uia_core::get()
    {
        static std::unique_ptr<uia_core> instance;

        if (instance)
        {
            return *instance;
        }

        instance = std::unique_ptr<uia_core>(new uia_core);

        // We need to load the automation core dynamically because we depend on building with MinGW...
        auto core = lime::module::load("UIAutomationCore.dll");

        instance->UiaClientsAreListening =
            reinterpret_cast<decltype(UiaClientsAreListening)>(core->symbol("UiaClientsAreListening"));

        instance->UiaHostProviderFromHwnd =
            reinterpret_cast<decltype(UiaHostProviderFromHwnd)>(core->symbol("UiaHostProviderFromHwnd"));

        instance->UiaRaiseAutomationEvent =
            reinterpret_cast<decltype(UiaRaiseAutomationEvent)>(core->symbol("UiaRaiseAutomationEvent"));

        instance->UiaReturnRawElementProvider =
            reinterpret_cast<decltype(UiaReturnRawElementProvider)>(core->symbol("UiaReturnRawElementProvider"));

        return *instance;
    }

    struct toggle_uia::impl
    {
        std::shared_ptr<checkable> parent;
        checkable_type type;

      public:
        ToggleState cached;
        std::atomic_size_t ref_count;
    };

    toggle_uia::~toggle_uia() = default;

    toggle_uia::toggle_uia(std::shared_ptr<checkable> parent, checkable_type type) : m_impl(std::make_unique<impl>())
    {
        m_impl->parent = std::move(parent);
        m_impl->type   = type;

        get_CurrentToggleState(&m_impl->cached);
    }

    IFACEMETHODIMP_(ULONG) toggle_uia::AddRef()
    {
        m_impl->ref_count++;
        return S_OK;
    }

    IFACEMETHODIMP_(ULONG) toggle_uia::Release()
    {
        auto count = m_impl->ref_count.fetch_sub(1);

        if (count == 0)
        {
            delete this;
        }

        return count;
    }

    IFACEMETHODIMP toggle_uia::QueryInterface(REFIID id, void **ret)
    {
        if (id == __uuidof(IUnknown) || id == __uuidof(IRawElementProviderSimple))
        {
            *ret = static_cast<IRawElementProviderSimple *>(this);
        }
        else if (id == __uuidof(IToggleProvider))
        {
            *ret = static_cast<IToggleProvider *>(this);
        }
        else if (id == __uuidof(IUIAutomationTogglePattern))
        {
            *ret = static_cast<IUIAutomationTogglePattern *>(this);
        }
        else
        {
            *ret = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();

        return S_OK;
    }

    HRESULT toggle_uia::Toggle()
    {
        m_impl->parent->toggle();
        return S_OK;
    }

    HRESULT toggle_uia::get_ToggleState(ToggleState *ret)
    {
        return get_CurrentToggleState(ret);
    }

    HRESULT toggle_uia::get_CachedToggleState(ToggleState *ret)
    {
        *ret = m_impl->cached;
        return S_OK;
    }

    HRESULT toggle_uia::get_CurrentToggleState(ToggleState *ret)
    {
        m_impl->cached = m_impl->parent->checked() ? ToggleState_On : ToggleState_Off;
        *ret           = m_impl->cached;

        return S_OK;
    }

    IFACEMETHODIMP toggle_uia::GetPropertyValue(PROPERTYID id, VARIANT *ret)
    {
        ret->vt = VT_EMPTY;

        if (id == UIA_FrameworkIdPropertyId)
        {
            ret->bstrVal = SysAllocString(L"SimplyTest: profuis-patch");
            ret->vt      = VT_BSTR;
        }
        else if (id == UIA_HelpTextPropertyId)
        {
            ret->bstrVal =
                SysAllocString(L"https://github.com/simplytest/profuis-patch - Noah Karnel (https://github.com/Curve)");
            ret->vt = VT_BSTR;
        }
        else if (id == UIA_ControlTypePropertyId)
        {
            ret->lVal =
                m_impl->type == checkable_type::check ? UIA_CheckBoxControlTypeId : UIA_RadioButtonControlTypeId;
            ret->vt = VT_I4;
        }

        return S_OK;
    }

    IFACEMETHODIMP toggle_uia::GetPatternProvider(PATTERNID id, IUnknown **ret)
    {
        *ret = nullptr;

        if (id == UIA_TogglePatternId)
        {
            QueryInterface(__uuidof(IUnknown), reinterpret_cast<void **>(ret));
        }

        return S_OK;
    }

    IFACEMETHODIMP toggle_uia::get_ProviderOptions(ProviderOptions *ret)
    {
        *ret = static_cast<ProviderOptions>(ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading);
        return S_OK;
    }

    IFACEMETHODIMP toggle_uia::get_HostRawElementProvider(IRawElementProviderSimple **ret)
    {
        return uia_core::get().UiaHostProviderFromHwnd(m_impl->parent->hwnd(), ret);
    }
} // namespace simplytest
