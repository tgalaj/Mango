#include "mgpch.h"

#include "Material.h"
#include "Mango/Systems/RenderingSystem.h"
#include "Mango/Asset/AssetManager.h"

namespace mango
{
    Material::Material(const std::string& name) 
        : name(name)
    {
        addFloat("specular_power",     20.0f);
        addFloat("specular_intensity", 5.0f);
        addFloat("m_depth_scale",      0.015f);
        addFloat("alpha_cutoff",       0.2f);
        addFloat("alpha",              1.0f);

        m_blendMode = BlendMode::NONE;

        // TODO(tgalaj): create default textures when the engine starts
        m_textureMap[TextureType::DIFFUSE]      = AssetManager::getAssetByName<Texture>("DefaultDiffuse");
        m_textureMap[TextureType::SPECULAR]     = AssetManager::getAssetByName<Texture>("DefaultSpecular");
        m_textureMap[TextureType::NORMAL]       = AssetManager::getAssetByName<Texture>("DefaultNormal");
        m_textureMap[TextureType::EMISSION]     = AssetManager::getAssetByName<Texture>("DefaultEmission");
        m_textureMap[TextureType::DISPLACEMENT] = AssetManager::getAssetByName<Texture>("DefaultDisplacement");
    }


    Material::~Material()
    {
    }

    void Material::addTexture(TextureType textureType, const ref<Texture> & texture)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_textureMap[textureType] = texture;
    }

    void Material::addVector3(const std::string & uniformName, const glm::vec3 & vec)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_vec3Map[uniformName] = vec;
    }

    void Material::addFloat(const std::string & uniformName, float value)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_floatMap[uniformName] = value;
    }

    void Material::addBool(const std::string& uniformName, bool value)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_boolMap[uniformName] = value;
    }

    ref<Texture> Material::getTexture(TextureType textureType)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_textureMap.contains(textureType))
        {
            return m_textureMap[textureType];
        }

        MG_CORE_ASSERT_FAIL("Couldn't find texture with the specified texture type!");
        return nullptr;
    }

    glm::vec3 Material::getVector3(const std::string & uniformName)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_vec3Map.contains(uniformName))
        {
            return m_vec3Map[uniformName];
        }

        return glm::vec3(1.0f);
    }

    float Material::getFloat(const std::string & uniformName)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_floatMap.contains(uniformName))
        {
            return m_floatMap[uniformName];
        }

        return 1.0f;
    }

    bool Material::getBool(const std::string& uniformName)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_boolMap.contains(uniformName))
        {
            return m_boolMap[uniformName];
        }

        return false;
    }

}
