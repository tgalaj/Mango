#include "mgpch.h"

#include "Material.h"
#include "Mango/Core/CoreAssetManager.h"

namespace mango
{
    std::unordered_map<Material::TextureType, std::string> Material::m_textureUniformsMap
    {
        { TextureType::DIFFUSE,  "diffuse_map"  },
        { TextureType::SPECULAR, "specular_map" },
        { TextureType::NORMAL,   "normal_map"   },
        { TextureType::EMISSION, "emission_map" },
        { TextureType::DEPTH,    "depth_map"    },
    };

    Material::Material()
    {
        addFloat("specular_power",     20.0f);
        addFloat("specular_intensity", 5.0f);
        addFloat("m_depth_scale",      0.015f);
        addFloat("alpha_cutoff",       0.2f);

        m_blendMode = BlendMode::NONE;

        m_textureMap[TextureType::DIFFUSE]  = CoreAssetManager::getTexture2D("default_white");
        m_textureMap[TextureType::SPECULAR] = CoreAssetManager::getTexture2D("default_black");
        m_textureMap[TextureType::NORMAL]   = CoreAssetManager::getTexture2D("default_normal");
        m_textureMap[TextureType::EMISSION] = CoreAssetManager::getTexture2D("default_black");
        m_textureMap[TextureType::DEPTH]    = CoreAssetManager::getTexture2D("default_black");
    }


    Material::~Material()
    {
    }

    void Material::addTexture(TextureType textureType, const std::shared_ptr<Texture> & texture)
    {
        m_textureMap[textureType] = texture;
    }

    void Material::addVector3(const std::string & uniformName, const glm::vec3 & vec)
    {
        m_vec3Map[uniformName] = vec;
    }

    void Material::addFloat(const std::string & uniformName, float value)
    {
        m_floatMap[uniformName] = value;
    }

    std::shared_ptr<Texture> Material::getTexture(TextureType textureType)
    {
        if (m_textureMap.count(textureType))
        {
            return m_textureMap[textureType];
        }

        MG_CORE_ASSERT_FAIL("Couldn't find texture with the specified texture type!");
        return nullptr;
    }

    glm::vec3 Material::getVector3(const std::string & uniformName)
    {
        if (m_vec3Map.count(uniformName))
        {
            return m_vec3Map[uniformName];
        }

        return glm::vec3(1.0f);
    }

    float Material::getFloat(const std::string & uniformName)
    {
        if (m_floatMap.count(uniformName))
        {
            return m_floatMap[uniformName];
        }

        return 1.0f;
    }
}
