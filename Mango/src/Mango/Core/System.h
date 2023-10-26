#pragma once

#include "Assertions.h"
#include "Log.h"

namespace mango
{
    enum class SystemPriority { High, Medium, Low };

    class System
    {
    public:

        System(const std::string& name = "System");
        virtual ~System() = default;

        virtual void onInit() {}
        virtual void onDestroy() {}
        virtual void onUpdate(float dt) {}
        virtual void onGui() {}

        const std::string& getName() const { return m_name; }

    protected:
        std::string m_name;
        SystemPriority m_priority = SystemPriority::Medium;

    private:
        friend class SystemManager;
    };
}

