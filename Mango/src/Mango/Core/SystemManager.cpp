#include "mgpch.h"
#include "SystemManager.h"

namespace mango
{
    SystemManager::~SystemManager()
    {
        for (auto& system : m_systems)
        {
            system.second->onDestroy();
            delete system.second;
        }
        m_systems.clear();
    }

    void SystemManager::add(System* system, SystemPriority priority /*= SystemPriority::Medium*/)
    {
        MG_CORE_ASSERT(system != nullptr);
        system->m_priority = priority;

        m_systems.insert({ priority, system });
    }

    void SystemManager::remove(System* system)
    {
        MG_CORE_ASSERT(system != nullptr);

        auto systemPriority = system->m_priority; 

        auto it = std::find_if(m_systems.begin(), m_systems.end(), 
                               [system, systemPriority](const std::pair<SystemPriority, System*>& p)
                               {
                                   return p.second == system && p.first == systemPriority;
                               });

        if (it != m_systems.end())
        {
            system->onDestroy();

            delete system;
            m_systems.erase(it);
        }
    }

    void SystemManager::remove(const std::string& name)
    {
        auto it = std::find_if(m_systems.begin(), m_systems.end(),
                               [&name](const std::pair<SystemPriority, System*>& p)
                               {
                                   return name == p.second->m_name;
                               });

        MG_CORE_ASSERT(it != m_systems.end());

        if (it != m_systems.end())
        {
            it->second->onDestroy();

            delete it->second;
            m_systems.erase(it);
        }
    }

    void SystemManager::configure()
    {
        for (auto& system : m_systems)
        {
            system.second->onInit();
        }
    }

    void SystemManager::updateAll(float dt)
    {
        for (auto& system : m_systems)
        {
            system.second->onUpdate(dt);
        }
    }

    mango::System * const SystemManager::getSystem(const std::string& name) const
    {
        auto it = std::find_if(m_systems.begin(), m_systems.end(),
                               [&name](const std::pair<SystemPriority, System*>& p)
                               {
                                   return name == p.second->m_name;
                               });

        MG_CORE_ASSERT(it != m_systems.end());

        return it == m_systems.end() ? nullptr : it->second;
    }
}
