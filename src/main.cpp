#include <lime/entrypoint.hpp>
#include <lime/hooks/hook.hpp>
#include <lime/module.hpp>

#include <exports.hpp>
#include <windows.h>

#include "logger.hpp"
#include "patch.hpp"

using simplytest::logger;

void lime::load()
{
    char original[MAX_PATH];
    GetSystemWow64Directory(original, sizeof(original));

    strcat_s(original, "\\winmm.dll");
    lime::proxy::setup(original);

    logger::get()->debug("Redirecting to: {}", original);
    logger::get()->info("Initialized proxy");

    auto module = lime::module::find("ProfUIS");

    if (!module)
    {
        logger::get()->error("ProfUIS is not loaded");
        return;
    }

    simplytest::patch(module.value());
}

void lime::unload()
{
    logger::get()->info("Unloading...");
}
