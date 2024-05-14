#include "logger.hpp"

#include <filesystem>
#include <fstream>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace simplytest
{
    logger::logger()
    {
        namespace sinks = spdlog::sinks;
        namespace fs    = std::filesystem;

        m_logger = std::make_unique<spdlog::logger>("simplytest");
        m_logger->set_level(spdlog::level::trace);
        m_logger->flush_on(spdlog::level::trace);

        auto stdout_sink = std::make_shared<sinks::wincolor_stdout_sink_mt>();
        stdout_sink->set_level(spdlog::level::trace);

        m_logger->sinks().emplace_back(stdout_sink);

        const auto log_path = fs::current_path() / "profui-patch.log";
        auto err            = std::error_code{};

        if (!fs::remove(log_path, err) && err != std::errc::no_such_file_or_directory)
        {
            m_logger->error("could not delete log ('{}'): {}", log_path.string(), err.message());
            return;
        }

        if (auto file = std::ofstream{log_path}; file.close(), file.fail())
        {
            m_logger->error("could not create log ('{}'): {}", log_path.string(), file.bad());
            return;
        }

        auto file_sink = std::make_shared<sinks::basic_file_sink_mt>("profui-patch.log");
        file_sink->set_level(spdlog::level::trace);

        m_logger->sinks().emplace_back(file_sink);
    }

    spdlog::logger *logger::operator->() const
    {
        return m_logger.get();
    }

    logger &logger::get()
    {
        static std::unique_ptr<logger> instance;

        if (!instance)
        {
            instance = std::unique_ptr<logger>(new logger);
        }

        return *instance;
    }
} // namespace simplytest
