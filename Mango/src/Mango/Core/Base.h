#pragma once
#include "Assertions.h"
#include "Log.h"

#include <memory>

namespace mango
{
    template<typename T>
    using scope = std::unique_ptr<T>;

    template<typename T, typename... Args>
    constexpr scope<T> createScope(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    constexpr ref<T> createRef(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}