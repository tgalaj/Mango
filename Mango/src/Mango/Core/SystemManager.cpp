#include "mgpch.h"
#include "SystemManager.h"

namespace mango
{
    SystemManager::~SystemManager()
    {
        for (auto& system : m_systems)
        {
            system->onDestroy();
            delete system;
        }
        m_systems.clear();
    }

    void SystemManager::add(System* system)
    {
        MG_CORE_ASSERT(system != nullptr);

        m_systems.emplace_back(system);
    }

    void SystemManager::remove(System* system)
    {
        MG_CORE_ASSERT(system != nullptr);

        auto it = std::find(m_systems.begin(), m_systems.end(), system);

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
                               [&name](const System* s)
                               {
                                   return name == s->m_name;
                               });

        MG_CORE_ASSERT(it != m_systems.end());

        if (it != m_systems.end())
        {
            (*it)->onDestroy();

            delete (*it);
            m_systems.erase(it);
        }
    }

    void SystemManager::configure()
    {
        for (auto& system : m_systems)
        {
            system->onInit();
        }
    }

    void SystemManager::updateAll(float dt)
    {
        for (auto& system : m_systems)
        {
            system->onUpdate(dt);
        }
    }

    mango::System * const SystemManager::getSystem(const std::string& name) const
    {
        auto it = std::find_if(m_systems.begin(), m_systems.end(),
                               [&name](const System* s)
                               {
                                   return name == s->m_name;
                               });

        MG_CORE_ASSERT(it != m_systems.end());

        return it == m_systems.end() ? nullptr : (*it);
    }
}
