#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace simplytest
{
    class logger
    {
        std::unique_ptr<spdlog::logger> m_logger;

      private:
        logger();

      public:
        spdlog::logger *operator->() const;

      public:
        [[nodiscard]] static logger &get();
    };
} // namespace simplytest
