#include <lime/entrypoint.hpp>
#include <lime/module.hpp>

#include <exports.hpp>
#include <windows.h>

#include "logger.hpp"
#include "patch.hpp"

using simplytest::logger;

void lime::load()
{
    char original[MAX_PATH];
    GetSystemDirectory(original, sizeof(original));

    strcat_s(original, "\\MSIMG32.dll");
    lime::proxy::setup(original);

    logger::get()->debug("redirecting to '{}'", original);
    auto module = lime::module::find("ProfUIS");

    if (!module)
    {
        logger::get()->error("could not find 'ProfUIS'");
        return;
    }

    simplytest::patch(module.value());
}

void lime::unload()
{
    logger::get()->info("unloading");
}
