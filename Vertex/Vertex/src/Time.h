#pragma once

#include <SDL2\SDL.h>

class Time
{
public:
    Time();
    ~Time();

    Time(const Time&)            = delete;
    Time& operator=(const Time&) = delete;

    /**
     * @brief Returns time since SDL initialization.
     * @return Time in seconds.
     */
    static inline float getTime()
    {
        return static_cast<float>(SDL_GetTicks()) / 1000.0f;
    }

    /**
    * @brief Returns time since SDL initialization.
    * @return Time in milliseconds.
    */
    static inline unsigned int getTimeMs()
    {
        return static_cast<unsigned int>(SDL_GetTicks());
    }
};

