#include "mgpch.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace mango
{
    std::shared_ptr<spdlog::logger> Log::s_coreLogger = nullptr;
    std::shared_ptr<spdlog::logger> Log::s_applicationLogger = nullptr;

    void Log::init()
    {
        MG_PROFILE_ZONE_SCOPED;
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Mango.log", true));

        logSinks[0]->set_pattern("%^[%T] %n: %v%$");
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        s_coreLogger = std::make_shared<spdlog::logger>("MANGO", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_coreLogger);
        s_coreLogger->set_level(spdlog::level::trace);
        s_coreLogger->flush_on(spdlog::level::trace);

        s_applicationLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_applicationLogger);
        s_applicationLogger->set_level(spdlog::level::trace);
        s_applicationLogger->flush_on(spdlog::level::trace);
    }
}