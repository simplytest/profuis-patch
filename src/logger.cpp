#include "logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>

namespace simplytest
{
    struct logger::impl
    {
        std::unique_ptr<spdlog::logger> logger;
    };

    logger::logger() : m_impl(std::make_unique<impl>())
    {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("td-patch.log");
        file_sink->set_level(spdlog::level::trace);

        m_impl->logger = std::make_unique<spdlog::logger>("simplytest", file_sink);
        m_impl->logger->set_level(spdlog::level::trace);
        m_impl->logger->flush_on(spdlog::level::trace);
    }

    spdlog::logger *logger::operator->() const
    {
        return m_impl->logger.get();
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
