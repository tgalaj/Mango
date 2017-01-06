#pragma once
#include <chrono>

class Time
{
public:

    /** @brief Get current time 
      * @result time in seconds
     **/
    static double getTime()
    {
        auto now = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count() / static_cast<double>(SECOND);
    }

    static const long long SECOND = 1'000'000'000;
};
