#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include <unordered_map>

#include "Texture.h"

namespace mango
{
    class Material
    {
    public:
        enum class TextureType { DIFFUSE, SPECULAR, NORMAL, EMISSION, DEPTH };
        enum class BlendMode   { NONE, ALPHA };

        Material();
        ~Material();

        void addTexture(TextureType textureType, const ref<Texture> & texture);
        void addVector3(const std::string & uniformName, const glm::vec3 & vec);
        void addFloat  (const std::string & uniformName, float value);

        ref<Texture> getTexture(TextureType textureType);
        glm::vec3    getVector3(const std::string & uniformName);
        float        getFloat  (const std::string & uniformName);

        void setBlendMode(BlendMode mode) { m_blendMode = mode; }

        std::unordered_map<TextureType, ref<Texture>>& getTexturesMap() { return m_textureMap; }
        std::unordered_map<std::string, glm::vec3>   & getVec3Map()     { return m_vec3Map; }
        std::unordered_map<std::string, float>       & getFloatMap()    { return m_floatMap; }

    private:
        std::unordered_map<TextureType, ref<Texture>> m_textureMap;
        std::unordered_map<std::string, glm::vec3>    m_vec3Map;
        std::unordered_map<std::string, float>        m_floatMap;

        BlendMode m_blendMode;

        static std::unordered_map<TextureType, std::string> m_textureUniformsMap;

    private:
        friend class SceneHierarchyPanel;
    };
}
