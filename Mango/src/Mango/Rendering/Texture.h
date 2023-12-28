#pragma once
#include <string>

#include "glad/glad.h"
#include "glm/vec4.hpp"

namespace mango
{
    struct TextureDescriptorGL
    {
        TextureDescriptorGL() {}

        GLenum             type           = 0;
        GLenum             format         = 0;
        GLenum             internalFormat = 0;
        GLuint             mipLevels      = 1;
        GLuint             width          = 0;
        GLuint             height         = 0;
        GLuint             depth          = 1;
        glm::ivec4         swizzles       = glm::ivec4(GL_TEXTURE_SWIZZLE_R, GL_TEXTURE_SWIZZLE_G, GL_TEXTURE_SWIZZLE_B, GL_TEXTURE_SWIZZLE_A);
        bool               compressed     = false;
    };

    // Handles loading images with stb_image (jpg, png, tga, bmp, psd, gif, hdr, pic, pnm)
    class Texture
    {
    public:
        Texture();
        virtual ~Texture();

        void bind(GLuint unit = 0) const;
        static void unbindTextureUnit(GLuint unit);
        
        GLuint getWidth()  const { return m_descriptor.width; }
        GLuint getHeight() const { return m_descriptor.height; }
        GLuint getID()     const { return m_id; }
    
    protected:
        GLuint              m_id;
        TextureDescriptorGL m_descriptor;

    private:
        uint8_t* loadTexture(const std::string& filename, TextureDescriptorGL& descriptor);
        
        void genTexture2D     (const std::string & filename,  GLuint numMipmaps, bool isSrgb = false);
        void genTexture2D1x1  (const glm::uvec4 & color);
        void genCubeMapTexture(const std::string * filenames, GLuint numMipmaps, bool isSrgb = false);

    private:
        friend class AssetManager;
        friend class ImGuiSystem;
    };

    class TextureDDS : public Texture
    {
    public:
        TextureDDS();

    private:
        void genTexture(const std::string& filename);

    private:
        friend class AssetManager;
        friend class ImGuiSystem;
    };
}
