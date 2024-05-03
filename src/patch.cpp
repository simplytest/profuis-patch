#include "patch.hpp"

#include <lime/hooks/hook.hpp>

#include "profuis.hpp"
#include "logger.hpp"

namespace simplytest
{
    using render_hk_t = lime::hook<void(void *, void *, profuis::checkable_data *)>;

    std::unique_ptr<render_hk_t> render_hook;

    void __attribute__((fastcall)) hk_render(void *thiz, void *cdc, profuis::checkable_data *data)
    {
        logger::get()->info("Rendered {:x} which is {}", reinterpret_cast<std::uintptr_t>(data->object->hwnd),
                            static_cast<int>(data->state));

        render_hook->original()(thiz, cdc, data);
    }

    void patch(const lime::module &profuis)
    {
        auto symbol = profuis.find_symbol("?PaintCheckButton@CExtPaintManager");

        if (!symbol)
        {
            logger::get()->error("Failed to find symbol!");
            return;
        }

        logger::get()->info("Found render: {:x}", symbol.value());
        auto hook = render_hk_t::create(symbol.value(), hk_render);

        if (!hook)
        {
            logger::get()->error("Failed to create hook: {}", static_cast<int>(hook.error()));
            return;
        }

        render_hook = std::move(hook.value());
    }
} // namespace simplytest
