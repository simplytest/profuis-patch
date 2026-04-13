#include "uia.hpp"

#include <lime/lib.hpp>

#include <atomic>
#include <optional>

namespace simplytest
{
    uia_core &uia_core::get()
    {
        using namespace lime::literals;

        static std::optional<uia_core> instance;

        if (instance)
        {
            return *instance;
        }

        // We need to load the automation core dynamically because we depend on building with MinGW...
        auto core = lime::lib::load("UIAutomationCore.dll").value();

#define UIAFunc(name)                                                                                                  \
    #name, std::type_identity<decltype(name)> {}

        instance = uia_core{
            .UiaClientsAreListening      = core[UIAFunc(UiaClientsAreListening)].value(),
            .UiaHostProviderFromHwnd     = core[UIAFunc(UiaHostProviderFromHwnd)].value(),
            .UiaRaiseAutomationEvent     = core[UIAFunc(UiaRaiseAutomationEvent)].value(),
            .UiaReturnRawElementProvider = core[UIAFunc(UiaReturnRawElementProvider)].value(),
        };

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

    toggle_uia::toggle_uia(std::shared_ptr<checkable> parent, checkable_type type) : m_impl(std::make_unique<impl>())
    {
        m_impl->parent = std::move(parent);
        m_impl->type   = type;

        get_CurrentToggleState(&m_impl->cached);
    }

    toggle_uia::~toggle_uia() = default;

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
        using enum checkable_type;

        static constexpr auto frameworkId = L"SimplyTest: profuis-patch (https://github.com/simplytest/profuis-patch)";
        static constexpr auto frameworkHelp = L"Noah Karnel (https://github.com/Curve)";

        ret->vt = VT_EMPTY;

        if (id == UIA_FrameworkIdPropertyId)
        {
            ret->bstrVal = SysAllocString(frameworkId);
            ret->vt      = VT_BSTR;
        }
        else if (id == UIA_HelpTextPropertyId)
        {
            ret->bstrVal = SysAllocString(frameworkHelp);
            ret->vt      = VT_BSTR;
        }
        else if (id == UIA_ControlTypePropertyId)
        {
            ret->lVal = m_impl->type == check ? UIA_CheckBoxControlTypeId : UIA_RadioButtonControlTypeId;
            ret->vt   = VT_I4;
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
