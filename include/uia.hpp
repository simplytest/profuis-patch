#pragma once

#include <memory>
#include <uiautomation.h>

namespace simplytest
{
    class checkbox;

    class uia_core
    {
        uia_core();

      public:
        BOOL (*UiaClientsAreListening)();

      public:
        HRESULT (*UiaHostProviderFromHwnd)(HWND, IRawElementProviderSimple **);
        HRESULT (*UiaRaiseAutomationEvent)(IRawElementProviderSimple *, EVENTID);
        LRESULT (*UiaReturnRawElementProvider)(HWND, WPARAM, LPARAM, IRawElementProviderSimple *);

      public:
        static uia_core &get();
    };

    class checkbox_uia : public IRawElementProviderSimple, public IUIAutomationTogglePattern, public IToggleProvider
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        virtual ~checkbox_uia();

      public:
        checkbox_uia(std::shared_ptr<checkbox>);

      public:
        IFACEMETHODIMP_(ULONG) AddRef() override;
        IFACEMETHODIMP_(ULONG) Release() override;
        IFACEMETHODIMP QueryInterface(REFIID, void **) override;

      public:
        HRESULT Toggle() override;

      public:
        HRESULT get_ToggleState(ToggleState *) override;
        HRESULT get_CachedToggleState(ToggleState *) override;
        HRESULT get_CurrentToggleState(ToggleState *) override;

      public:
        IFACEMETHODIMP GetPropertyValue(PROPERTYID, VARIANT *) override;
        IFACEMETHODIMP GetPatternProvider(PATTERNID, IUnknown **) override;

      public:
        IFACEMETHODIMP get_ProviderOptions(ProviderOptions *) override;
        IFACEMETHODIMP get_HostRawElementProvider(IRawElementProviderSimple **) override;
    };
} // namespace simplytest
