#pragma once

#include "System.h"

#include <map>

namespace mango
{
    class SystemManager final
    {
    public:
        SystemManager() = default;
        ~SystemManager();

        void add(System* system, SystemPriority priority = SystemPriority::Medium);
        void remove(System* system);
        void remove(const std::string& name);

        void configure();
        void updateAll(float dt);

        System* const getSystem(const std::string& name) const;

        std::multimap<SystemPriority, System*>::iterator         begin()  { return m_systems.begin(); }
        std::multimap<SystemPriority, System*>::iterator         end()    { return m_systems.end(); }
        std::multimap<SystemPriority, System*>::reverse_iterator rbegin() { return m_systems.rbegin(); }
        std::multimap<SystemPriority, System*>::reverse_iterator rend()   { return m_systems.rend(); }

        std::multimap<SystemPriority, System*>::const_iterator         begin()  const { return m_systems.begin(); }
        std::multimap<SystemPriority, System*>::const_iterator         end()    const { return m_systems.end(); }
        std::multimap<SystemPriority, System*>::const_reverse_iterator rbegin() const { return m_systems.rbegin(); }
        std::multimap<SystemPriority, System*>::const_reverse_iterator rend()   const { return m_systems.rend(); }

    private:
        std::multimap<SystemPriority, System*> m_systems;
    };
}

