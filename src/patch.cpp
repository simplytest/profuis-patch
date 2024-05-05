#include "patch.hpp"

#include "checkable.hpp"
#include "logger.hpp"

#include <lime/hooks/hook.hpp>

namespace simplytest
{
    using check_render_hk_t = lime::hook<void(void *, void *, profuis::checkable_data *)>;
    using radio_render_hk_t = lime::hook<void(void *, void *, profuis::checkable_data *)>;

    std::unique_ptr<check_render_hk_t> check_render_hook;
    std::unique_ptr<radio_render_hk_t> radio_render_hook;

    void __attribute__((fastcall)) hk_check_render(void *thiz, void *cdc, profuis::checkable_data *data)
    {
        checkable::update(data, checkable_type::check);
        check_render_hook->original()(thiz, cdc, data);
    }

    void __attribute__((fastcall)) hk_radio_render(void *thiz, void *cdc, profuis::checkable_data *data)
    {
        checkable::update(data, checkable_type::radio);
        radio_render_hook->original()(thiz, cdc, data);
    }

    void patch(const lime::module &profuis)
    {
        auto paint_check = profuis.find_symbol("?PaintCheckButton@CExtPaintManager");
        auto paint_radio = profuis.find_symbol("?PaintRadioButton@CExtPaintManager");

        if (!paint_check || !paint_radio)
        {
            logger::get()->error("could not find check or radio renderer");
            return;
        }

        logger::get()->info("check_paint is {:x}", paint_check.value());
        logger::get()->info("check_radio is {:x}", paint_radio.value());

        auto check_hook = check_render_hk_t::create(paint_check.value(), hk_check_render);

        if (!check_hook)
        {
            logger::get()->error("failed to create check hook: {}", static_cast<int>(check_hook.error()));
            return;
        }

        check_render_hook = std::move(check_hook.value());

        auto radio_hook = radio_render_hk_t::create(paint_radio.value(), hk_radio_render);

        if (!radio_hook)
        {
            logger::get()->error("failed to create radio hook: {}", static_cast<int>(radio_hook.error()));
            return;
        }

        radio_render_hook = std::move(radio_hook.value());
    }
} // namespace simplytest
