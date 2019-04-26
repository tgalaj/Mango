#pragma once

#include <glm/vec3.hpp>
#include <map>
#include <memory>
#include "Texture.h"

namespace Vertex
{
    class Material
    {
    public:
        enum class TextureType { DIFFUSE, SPECULAR, NORMAL, EMISSION, DEPTH };
        enum class BlendMode { NONE, ALPHA };

        Material();
        ~Material();

        void addTexture(TextureType texture_type, const std::shared_ptr<Texture> & texture);
        void addVector3(const std::string & uniform_name, const glm::vec3 & vector3);
        void addFloat  (const std::string & uniform_name, float value);

        std::shared_ptr<Texture> getTexture(TextureType texture_type);
        glm::vec3                getVector3(const std::string & uniform_name);
        float                    getFloat  (const std::string & uniform_name);

        void setBlendMode(BlendMode mode) { m_blend_mode = mode; }

    private:
        std::map<TextureType, std::shared_ptr<Texture>> m_texture_map;
        std::map<std::string, glm::vec3> m_vec3_map;
        std::map<std::string, float> m_float_map;

        BlendMode m_blend_mode;

        static std::map<TextureType, std::string> m_texture_uniform_map;
    };
}
