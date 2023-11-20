#pragma once
#include <chrono>

namespace mango
{
    class Timer final
    {
    public:
        Timer()
        {
            reset();
        }

        void reset()
        {
            m_start = std::chrono::high_resolution_clock::now();
        }

        float elapsed()
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_start).count() * 0.001f * 0.001f * 0.001f;
        }

        float elapsedMs()
        {
            return elapsed() * 1000.0f;
        }

        static double getTime()
        {
            auto now = std::chrono::high_resolution_clock::now();

            return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count() * 0.001f * 0.001f * 0.001f;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };
}