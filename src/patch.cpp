#include "patch.hpp"

#include "constants.hpp"
#include "checkable.hpp"
#include "logger.hpp"

#include <lime/hooks/hook.hpp>

namespace simplytest
{
    void patch(const lime::module &profuis)
    {
        logger::get()->info("patch target: {}", profuis.name());

        auto paint_check = profuis.find_symbol("?PaintCheckButton@CExtPaintManager");
        auto paint_radio = profuis.find_symbol("?PaintRadioButton@CExtPaintManager");

        if (!paint_check || !paint_radio)
        {
            logger::get()->error("could not find check or radio renderer");
            return;
        }

        logger::get()->info("check_paint is {:x}", paint_check.value());
        logger::get()->info("check_radio is {:x}", paint_radio.value());

        using architecture::x64;

        using signature                  = void(void *, void *, profuis::checkable_data *);
        static constexpr auto convention = arch == x64 ? lime::convention::automatic : lime::convention::c_thiscall;

        auto radio_hook = lime::hook<signature, convention>::create(
            paint_radio.value(),
            [](auto *hook, void *thiz, void *cdc, profuis::checkable_data *data)
            {
                checkable::update(data, checkable_type::radio);
                return hook->original()(thiz, cdc, data);
            });

        auto check_hook = lime::hook<signature, convention>::create(
            paint_check.value(),
            [](auto *hook, void *thiz, void *cdc, profuis::checkable_data *data)
            {
                checkable::update(data, checkable_type::check);
                return hook->original()(thiz, cdc, data);
            });

        if (!check_hook)
        {
            logger::get()->error("failed to create check hook: {}", static_cast<int>(check_hook.error()));
            return;
        }

        if (!radio_hook)
        {
            logger::get()->error("failed to create radio hook: {}", static_cast<int>(radio_hook.error()));
            return;
        }
    }
} // namespace simplytest
