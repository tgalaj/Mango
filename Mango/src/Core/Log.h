#pragma once
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace mango
{
    class Log
    {
    public:
        static void init();

        static std::shared_ptr<spdlog::logger>& getCoreLogger()        { return s_coreLogger; }
        static std::shared_ptr<spdlog::logger>& getApplicationLogger() { return s_applicationLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_coreLogger;
        static std::shared_ptr<spdlog::logger> s_applicationLogger;
    };
}

template<typename ostream, glm::length_t L, typename T, glm::qualifier Q>
inline ostream& operator<<(ostream& os, const glm::vec<L, T, Q>& vector)
{
    return os << glm::to_string(vector);
}

template<typename ostream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline ostream& operator<<(ostream& os, const glm::mat<C, R, T, Q>& matrix)
{
    return os << glm::to_string(matrix);
}

template<typename ostream, typename T, glm::qualifier Q>
inline ostream& operator<<(ostream& os, glm::qua<T, Q> quaternion)
{
    return os << glm::to_string(quaternion);
}

#define MG_CORE_TRACE(...)    mango::Log::getCoreLogger()->trace(__VA_ARGS__);
#define MG_CORE_INFO(...)     mango::Log::getCoreLogger()->info(__VA_ARGS__);
#define MG_CORE_WARN(...)     mango::Log::getCoreLogger()->warn(__VA_ARGS__);
#define MG_CORE_ERROR(...)    mango::Log::getCoreLogger()->error(__VA_ARGS__);
#define MG_CORE_CRITICAL(...) mango::Log::getCoreLogger()->critical(__VA_ARGS__);

#define MG_TRACE(...)    mango::Log::getApplicationLogger()->trace(__VA_ARGS__);
#define MG_INFO(...)     mango::Log::getApplicationLogger()->info(__VA_ARGS__);
#define MG_WARN(...)     mango::Log::getApplicationLogger()->warn(__VA_ARGS__);
#define MG_ERROR(...)    mango::Log::getApplicationLogger()->error(__VA_ARGS__);
#define MG_CRITICAL(...) mango::Log::getApplicationLogger()->critical(__VA_ARGS__);