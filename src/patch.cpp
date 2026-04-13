#include "patch.hpp"

#include "constants.hpp"
#include "checkable.hpp"
#include "logger.hpp"

#include <lime/hooks/hook.hpp>

namespace simplytest
{
    void patch(const lime::lib &profuis)
    {
        using architecture::x64;
        using namespace lime::literals;
        using enum lime::calling_convention;

        logger::get()->info("target: {}", profuis.name());

        auto paint_check = profuis["?PaintCheckButton@CExtPaintManager"_re];
        auto paint_radio = profuis["?PaintRadioButton@CExtPaintManager"_re];

        if (!paint_check.has_value() || !paint_radio.has_value())
        {
            logger::get()->error("could not find check or radio renderer");
            return;
        }

        logger::get()->info("check_paint is {:x}", *paint_check);
        logger::get()->info("check_radio is {:x}", *paint_radio);

        using signature                  = void(void *, void *, profuis::checkable_data *);
        static constexpr auto convention = arch == x64 ? cc_generic : cc_thiscall;

        auto radio_hook =
            lime::make_hook<signature, convention>(*paint_radio,
                                                   [](auto &hook, void *thiz, void *cdc, profuis::checkable_data *data)
                                                   {
                                                       checkable::update(data, checkable_type::radio);
                                                       return hook.original()(thiz, cdc, data);
                                                   });

        auto check_hook =
            lime::make_hook<signature, convention>(*paint_radio,
                                                   [](auto &hook, void *thiz, void *cdc, profuis::checkable_data *data)
                                                   {
                                                       checkable::update(data, checkable_type::check);
                                                       return hook.original()(thiz, cdc, data);
                                                   });

        if (!check_hook.has_value())
        {
            logger::get()->error("failed to create check hook: {}", static_cast<int>(check_hook.error()));
            return;
        }

        if (!radio_hook.has_value())
        {
            logger::get()->error("failed to create radio hook: {}", static_cast<int>(radio_hook.error()));
            return;
        }

        logger::get()->info("hooks installed");
    }
} // namespace simplytest
