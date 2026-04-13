#include <lime/lib.hpp>
#include <lime/exports.hpp>
#include <lime/entrypoint.hpp>

#include "logger.hpp"
#include "patch.hpp"

#include <windows.h>

namespace fs = std::filesystem;

using simplytest::logger;
using namespace lime::literals;

void lime::load()
{
    char original[MAX_PATH];
    GetSystemDirectory(original, sizeof(original));

    lime::exports::init(fs::path{original} / "MSIMG32.dll");
    logger::get()->debug("redirecting to '{}'", original);

    auto module = lime::lib::find("ProfUIS"_re);

    if (!module.has_value())
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
