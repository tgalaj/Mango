#pragma once
#include <memory>
#include "Shader.h"

namespace Vertex
{
    class PostprocessEffect
    {
    public:
        PostprocessEffect();
        virtual ~PostprocessEffect();

        virtual void init(const std::string & filter_name, const std::string & fragment_shader_path);
        
        void bind() const;
        void render() const;

    private:
        std::shared_ptr<Shader> m_postprocess;
        GLuint m_dummy_vao_id;
    };
}
