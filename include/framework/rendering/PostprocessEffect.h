#pragma once
#include <memory>
#include "Shader.h"

namespace mango
{
    class PostprocessEffect
    {
    public:
        PostprocessEffect();
        virtual ~PostprocessEffect();

        virtual void init(const std::string & filter_name, const std::string & fragment_shader_path);
        
        virtual void bind() const;
        virtual void render() const;

    protected:
        std::shared_ptr<Shader> m_postprocess;
        GLuint m_dummy_vao_id;
    };
}
