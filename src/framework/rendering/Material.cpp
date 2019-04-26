#include "framework/rendering/Material.h"
#include "core_engine/CoreAssetManager.h"

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

        if (texture_type == TextureType::DIFFUSE)
        {
            return CoreAssetManager::createTexture2D("res/textures/diff_default.jpg");
        }

        if (texture_type == TextureType::SPECULAR)
        {
            return CoreAssetManager::createTexture2D("res/textures/spec_default.jpg");
        }

        if (texture_type == TextureType::NORMAL)
        {
            return CoreAssetManager::createTexture2D("res/textures/normal_default.jpg");
        }

        if (texture_type == TextureType::DEPTH)
        {
            return CoreAssetManager::createTexture2D("res/textures/spec_default.jpg");
        }

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
