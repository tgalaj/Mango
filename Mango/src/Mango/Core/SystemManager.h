#pragma once

#include "System.h"

#include <vector>

namespace mango
{
    class SystemManager final
    {
    public:
        SystemManager() = default;
        ~SystemManager();

        void add(System* system);
        void remove(System* system);
        void remove(const std::string& name);

        void configure();
        void updateAll(float dt);

        System* const getSystem(const std::string& name) const;

        std::vector<System*>::iterator         begin()  { return m_systems.begin(); }
        std::vector<System*>::iterator         end()    { return m_systems.end(); }
        std::vector<System*>::reverse_iterator rbegin() { return m_systems.rbegin(); }
        std::vector<System*>::reverse_iterator rend()   { return m_systems.rend(); }

        std::vector<System*>::const_iterator         begin()  const { return m_systems.begin(); }
        std::vector<System*>::const_iterator         end()    const { return m_systems.end(); }
        std::vector<System*>::const_reverse_iterator rbegin() const { return m_systems.rbegin(); }
        std::vector<System*>::const_reverse_iterator rend()   const { return m_systems.rend(); }

    private:
        std::vector<System*> m_systems;
    };
}

