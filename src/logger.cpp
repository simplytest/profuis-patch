#include "logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>

namespace simplytest
{
    logger::logger()
    {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("td-patch.log");
        file_sink->set_level(spdlog::level::trace);

        m_logger = std::make_unique<spdlog::logger>("simplytest", file_sink);
        m_logger->set_level(spdlog::level::trace);
        m_logger->flush_on(spdlog::level::trace);
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
