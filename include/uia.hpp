#pragma once

#include "checkable.hpp"

#include <memory>
#include <uiautomation.h>

namespace simplytest
{
    class checkable;

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

    class toggle_uia : public IRawElementProviderSimple, public IUIAutomationTogglePattern, public IToggleProvider
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        virtual ~toggle_uia();

      public:
        toggle_uia(std::shared_ptr<checkable>, checkable_type);

      public:
        IFACEMETHODIMP_(ULONG) AddRef() override;
        IFACEMETHODIMP_(ULONG) Release() override;
        IFACEMETHODIMP QueryInterface(REFIID, void **) override;

      public:
        HRESULT STDMETHODCALLTYPE Toggle() override;

      public:
        HRESULT STDMETHODCALLTYPE get_ToggleState(ToggleState *) override;
        HRESULT STDMETHODCALLTYPE get_CachedToggleState(ToggleState *) override;
        HRESULT STDMETHODCALLTYPE get_CurrentToggleState(ToggleState *) override;

      public:
        IFACEMETHODIMP GetPropertyValue(PROPERTYID, VARIANT *) override;
        IFACEMETHODIMP GetPatternProvider(PATTERNID, IUnknown **) override;

      public:
        IFACEMETHODIMP get_ProviderOptions(ProviderOptions *) override;
        IFACEMETHODIMP get_HostRawElementProvider(IRawElementProviderSimple **) override;
    };
} // namespace simplytest
