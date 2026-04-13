#pragma once

#include "checkable.hpp"

#include <memory>
#include <uiautomation.h>

namespace simplytest
{
    class checkable;

    struct uia_core
    {
        BOOL (*UiaClientsAreListening)();

      public:
        HRESULT STDMETHODCALLTYPE (*UiaHostProviderFromHwnd)(HWND, IRawElementProviderSimple **);
        HRESULT STDMETHODCALLTYPE (*UiaRaiseAutomationEvent)(IRawElementProviderSimple *, EVENTID);
        LRESULT STDMETHODCALLTYPE (*UiaReturnRawElementProvider)(HWND, WPARAM, LPARAM, IRawElementProviderSimple *);

      public:
        static uia_core &get();
    };

    struct toggle_uia : IRawElementProviderSimple, IUIAutomationTogglePattern, IToggleProvider
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        toggle_uia(std::shared_ptr<checkable>, checkable_type);

      public:
        virtual ~toggle_uia();

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
