#pragma once
#include "Mango/Core/Base.h"

#include "Mango/Rendering/PostprocessEffect.h"
#include "Mango/Rendering/RenderTarget.h"
#include "Mango/Rendering/Shader.h"

namespace mango
{
    class Entity;

    class JFAOutline
    {
    public:
        JFAOutline()  = default;
        ~JFAOutline();

        void init(int width, int height);
        void clear();
        void resize(int width, int height);
        void render(ref<RenderTarget>& dstRT, Entity entity, const glm::vec3& outlineColor, float outlineWidth);

    private:
        void renderEntity(Entity entity, const ref<Shader> shader);

    private:
        ref<RenderTarget> m_objectsMaskRT;
        ref<RenderTarget> m_jumpFloodBufferA;
        ref<RenderTarget> m_jumpFloodBufferB;

        ref<Shader> m_stencilFillShader;
        ref<Shader> m_maskFillShader;
        ref<PostprocessEffect> m_jumpFloodInitPS;
        ref<PostprocessEffect> m_jumpFloodPS;
        // ref<PostprocessEffect> m_jumpFloodSingleAxisPS;
        ref<PostprocessEffect> m_jumpFloodOutlinePS;
    };
}