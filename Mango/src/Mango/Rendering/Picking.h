#pragma once
#include "RenderTarget.h"
#include "Shader.h"

namespace mango
{
    class Picking
    {
    public:
        Picking() = default;
        ~Picking();

        void init(int width, int height);
        void clear();
        void resize(int width, int height);
        void setPickingRegion(int x, int y, uint32_t width = 1, uint32_t height = 1);
        ref<Shader> getShader() const;
        ref<Shader> getBillboardShader() const;

        int getPickedID();

        void bindFramebuffer();

    private:
        struct PickingData
        {
            int   id;
            float depth;
        };

        ref<RenderTarget> m_pickingBuffer;
        ref<Shader>       m_pickingShader;
        ref<Shader>       m_pickingBillboardShader;
        GLuint            m_ssbo;
    };
}