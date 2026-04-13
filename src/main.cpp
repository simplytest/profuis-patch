#include <lime/lib.hpp>
#include <lime/exports.hpp>
#include <lime/entrypoint.hpp>

#include "logger.hpp"
#include "patch.hpp"

#include <windows.h>
#include <shlobj.h>

namespace fs = std::filesystem;

using simplytest::logger;
using namespace lime::literals;

void lime::load()
{
    auto root = PWSTR{};

    if (const auto status = SHGetKnownFolderPath(FOLDERID_System, 0, NULL, &root); status != S_OK)
    {
        logger::get()->error("failed to retrieve system path");
        return;
    }

    auto target = fs::path{root} / "MSIMG32.dll";
    CoTaskMemFree(root);

    lime::exports::init(target);
    logger::get()->debug("redirecting to '{}'", target.string());

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
