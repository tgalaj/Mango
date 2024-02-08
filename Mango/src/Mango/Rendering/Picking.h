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
        std::shared_ptr<Shader> getShader() const;

        int getPickedID();

        void bindFramebuffer();

    private:
        struct PickingData
        {
            int   id;
            float depth;
        };

        std::shared_ptr<RenderTarget> m_pickingBuffer;
        std::shared_ptr<Shader> m_pickingShader;
        GLuint m_ssbo;
    };
}