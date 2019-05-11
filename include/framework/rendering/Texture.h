#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

namespace Vertex
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
        
        GLuint getWidth()  const { return m_tex_data.width; }
        GLuint getHeight() const { return m_tex_data.height; }
        GLuint getID()     const {  return m_to_id; }

    private:
        void genTexture2D     (const std::string & filename,  GLint base_level, bool is_srgb = false);
        void genCubeMapTexture(const std::string * filenames, GLint base_level, bool is_srgb = false);

        ImageData m_tex_data;
        GLuint m_to_id;
        GLenum m_to_type;
        GLint  m_base_level;
        GLenum m_format;
        GLint m_internal_format;

        friend class CoreAssetManager;
        friend class GUI;
    };
}
