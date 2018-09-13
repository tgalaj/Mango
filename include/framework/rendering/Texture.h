#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

namespace Vertex
{
    class Texture final
    {
    public:
        Texture();
        ~Texture();

        void bind(GLuint unit = 0) const;
        
        GLuint getWidth()  const { return m_width; }
        GLuint getHeight() const { return m_height; }

    private:
        void genTexture2D     (const std::string & filename,  GLint base_level, bool is_srgb = false);
        void genCubeMapTexture(const std::string * filenames, GLint base_level, bool is_srgb = false);

        GLuint m_to_id;
        GLenum m_to_type;
        GLuint m_width;
        GLuint m_height;
        GLint  m_base_level;
        GLenum m_format;
        GLint m_internal_format;

        friend class CoreAssetManager;
    };
}
