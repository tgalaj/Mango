#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include <unordered_map>

#include "Texture.h"

namespace mango
{
    class Material;
    using MaterialTable = std::vector<ref<Material>>;

    class Material
    {
    public:
        enum class TextureType { DIFFUSE, SPECULAR, NORMAL, EMISSION, DISPLACEMENT };
        enum class BlendMode   { NONE, ALPHA };
        enum class RenderQueue { RQ_OPAQUE, RQ_TRANSPARENT, RQ_ENVIRO_MAPPING_STATIC, RQ_ENVIRO_MAPPING_DYNAMIC };

        Material(const std::string& name = "Unnamed");
        ~Material();

        void addTexture(      TextureType  textureType,  const ref<Texture>& texture);
        void addVector3(const std::string& uniformName,  const glm::vec3&    vec);
        void addFloat  (const std::string& uniformName,  float               value);
        void addBool   (const std::string& uniform_name, bool                value);

        ref<Texture> getTexture(TextureType textureType);
        glm::vec3    getVector3(const std::string & uniformName);
        float        getFloat  (const std::string & uniformName);
        bool         getBool   (const std::string & uniformName);

        void      setBlendMode(BlendMode mode) { m_blendMode = mode; }
        BlendMode getBlendMode() const         { return m_blendMode; }

        void        setRenderQueue(RenderQueue queue) { m_renderQueue = queue; }
        RenderQueue getRenderQueue() const { return m_renderQueue; }

        std::unordered_map<TextureType, ref<Texture>>& getTextureMap() { return m_textureMap; }
        std::unordered_map<std::string, glm::vec3>   & getVec3Map()    { return m_vec3Map; }
        std::unordered_map<std::string, float>       & getFloatMap()   { return m_floatMap; }
        std::unordered_map<std::string, bool>        & getBoolMap()    { return m_boolMap; }

        std::string name;

    private:
        std::unordered_map<TextureType, ref<Texture>> m_textureMap;
        std::unordered_map<std::string, glm::vec3>    m_vec3Map;
        std::unordered_map<std::string, float>        m_floatMap;
        std::unordered_map<std::string, bool>         m_boolMap;

        BlendMode   m_blendMode   = BlendMode::NONE;
        RenderQueue m_renderQueue = RenderQueue::RQ_OPAQUE;

    private:
        friend class SceneHierarchyPanel;
    };
}
