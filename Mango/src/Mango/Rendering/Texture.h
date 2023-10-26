#pragma once

#include <filesystem>
#include <glad/glad.h>
#include <glm/vec4.hpp>
#include <string>

namespace mango
{
    struct ImageData
    {
        ImageData()
            : width(0),
              height(0),
              channels(0)
        {}

        GLuint width;
        GLuint height;
        GLuint channels;
    };

    class Texture final
    {
    public:
        Texture();
        ~Texture();

        void bind(GLuint unit = 0) const;
        static void unbindTextureUnit(GLuint unit);
        
        GLuint getWidth()  const { return m_texData.width; }
        GLuint getHeight() const { return m_texData.height; }
        GLuint getID()     const { return m_id; }

    private:
        void genTexture2D     (const std::filesystem::path & filepath,  GLuint numMipmaps, bool isSrgb = false);
        void genTexture2D1x1  (const glm::uvec4 & color);

        void genCubeMapTexture(const std::filesystem::path * filepaths, GLuint numMipmaps, bool isSrgb = false);

        ImageData m_texData;
        GLuint    m_id;
        GLenum    m_type;
        GLuint    m_numMipmaps;
        GLenum    m_format;
        GLint     m_internalFormat;

        friend class CoreAssetManager;
        friend class GUI;
    };
}