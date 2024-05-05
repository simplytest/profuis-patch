#include "patch.hpp"

#include "checkbox.hpp"
#include "logger.hpp"

#include <lime/hooks/hook.hpp>

namespace simplytest
{
    using render_hk_t = lime::hook<void(void *, void *, profuis::checkable_data *)>;
    std::unique_ptr<render_hk_t> render_hook;

    void __attribute__((fastcall)) hk_render(void *thiz, void *cdc, profuis::checkable_data *data)
    {
        checkbox::update(data);
        render_hook->original()(thiz, cdc, data);
    }

    void patch(const lime::module &profuis)
    {
        auto paint_check = profuis.find_symbol("?PaintCheckButton@CExtPaintManager");

        if (!paint_check)
        {
            logger::get()->error("could not find `?PaintCheckButton@CExtPaintManager`");
            return;
        }

        logger::get()->info("paint_check is {:x}", paint_check.value());
        auto hook = render_hk_t::create(paint_check.value(), hk_render);

        if (!hook)
        {
            logger::get()->error("failed to create hook: {}", static_cast<int>(hook.error()));
            return;
        }

        render_hook = std::move(hook.value());
    }
} // namespace simplytest
