#include "framework/rendering/Material.h"
#include "core_engine/CoreAssetManager.h"
#include "helpers/Assertions.h"

namespace Vertex
{
    std::map<Material::TextureType, std::string> Material::m_texture_uniform_map
    {
        { TextureType::DIFFUSE,              "diffuse_map"  },
        { TextureType::SPECULAR,             "specular_map" },
        { TextureType::NORMAL,               "normal_map"   },
        { TextureType::EMISSION,             "emission_map" },
        { TextureType::DEPTH,                "depth_map"    },
    };

    Material::Material()
    {
        addFloat("specular_power", 20.0f);
        addFloat("specular_intensity", 5.0f);
        addFloat("m_depth_scale", 0.015f);
        addFloat("alpha_cutoff", 0.2f);

        m_blend_mode = BlendMode::NONE;

        m_texture_map[TextureType::DIFFUSE]  = CoreAssetManager::getTexture2D("default_white");
        m_texture_map[TextureType::SPECULAR] = CoreAssetManager::getTexture2D("default_black");
        m_texture_map[TextureType::NORMAL]   = CoreAssetManager::getTexture2D("default_normal");
        m_texture_map[TextureType::EMISSION] = CoreAssetManager::getTexture2D("default_black");
        m_texture_map[TextureType::DEPTH]    = CoreAssetManager::getTexture2D("default_black");
    }


    Material::~Material()
    {
    }

    void Material::addTexture(TextureType texture_type, const std::shared_ptr<Texture>& texture)
    {
        m_texture_map[texture_type] = texture;
    }

    void Material::addVector3(const std::string& uniform_name, const glm::vec3& vector3)
    {
        m_vec3_map[uniform_name] = vector3;
    }

    void Material::addFloat(const std::string& uniform_name, float value)
    {
        m_float_map[uniform_name] = value;
    }

    std::shared_ptr<Texture> Material::getTexture(TextureType texture_type)
    {
        if(m_texture_map.count(texture_type))
        {
            return m_texture_map[texture_type];
        }

        VERTEX_ASSERT_MSG(false, "Couldn't find texture with the specified texture type!");

        return nullptr;
    }

    glm::vec3 Material::getVector3(const std::string& uniform_name)
    {
        if (m_vec3_map.count(uniform_name))
        {
            return m_vec3_map[uniform_name];
        }

        return glm::vec3(1.0f);
    }

    float Material::getFloat(const std::string& uniform_name)
    {
        if (m_float_map.count(uniform_name))
        {
            return m_float_map[uniform_name];
        }

        return 1.0f;
    }
}
