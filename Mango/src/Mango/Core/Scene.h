#pragma once
#include "Prefab.h"
#include <map>

namespace mango
{
    class Scene final
    {
    public:
        Scene() {}
        ~Scene() {}

        template <typename P, typename ... Args>
        static void registerPrefab(const std::string & object, Args && ... args)
        {
            m_prefabs[object] = std::make_shared<P>(std::forward<Args>(args) ...);
        }

        static std::shared_ptr<GameObject> createPrefab(const std::string & object)
        {
            std::shared_ptr<GameObject> gameObject;

            if (m_prefabs.count(object))
            {
                gameObject = m_prefabs[object]->create();
            }

            return gameObject;
        }

        static std::shared_ptr<GameObject> createEmpty()
        {
            return std::make_shared<GameObject>();
        }

    private:
        static std::map<std::string, std::shared_ptr<Prefab>> m_prefabs;
    };
}

