#pragma once
#include <map>
#include "Prefab.h"

namespace Vertex
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
            std::shared_ptr<GameObject> game_object;

            if (m_prefabs.count(object))
            {
                game_object = m_prefabs[object]->create();
            }

            return game_object;
        }

        static std::shared_ptr<GameObject> createEmpty()
        {
            return std::make_shared<GameObject>();
        }

    private:
        static std::map<std::string, std::shared_ptr<Prefab>> m_prefabs;
    };
}

