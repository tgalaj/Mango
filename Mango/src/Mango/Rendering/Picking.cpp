#include "mgpch.h"
#include "Picking.h"
#include "Mango/Core/AssetManager.h"

namespace mango
{
    Picking::~Picking()
    {
        clear();
    }

    void Picking::init(int width, int height)
    {
        MG_PROFILE_ZONE_SCOPED;

        resize(width, height);

        glCreateBuffers(1, &m_ssbo);
        PickingData initialValues = { -1, 1.0f };
        glNamedBufferData(m_ssbo, sizeof(initialValues), &initialValues, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);

        m_pickingShader = AssetManagerOld::createShader("PickingShader", "Picking.vert", "Picking.frag");
        m_pickingShader->link();

        m_pickingBillboardShader = AssetManagerOld::createShader("PickingBillboardSprite", "PickingBillboardSprite.vert", "PickingBillboardSprite.frag", "PickingBillboardSprite.geom");
        m_pickingBillboardShader->link();
    }

    void Picking::clear()
    {
        glDeleteBuffers(1, &m_ssbo);
    }

    void Picking::resize(int width, int height)
    {
        m_pickingBuffer = createRef<RenderTarget>();
        m_pickingBuffer->create(width, height, RenderTarget::DepthInternalFormat::DEPTH24, RenderTarget::RenderTargetType::Tex2D, false);
    }

    void Picking::setPickingRegion(int x, int y, uint32_t width, uint32_t height)
    {
        MG_PROFILE_ZONE_SCOPED;
        glScissor(x, y, width, height);
    }

    ref<Shader> Picking::getShader() const
    {
        return m_pickingShader;
    }

    ref<Shader> Picking::getBillboardShader() const
    {
        return m_pickingBillboardShader;
    }

    int Picking::getPickedID()
    {
        MG_PROFILE_ZONE_SCOPED;
        int selectedID = -1;
        PickingData initialValues = { -1, 1.0f };

        void* data = glMapNamedBuffer(m_ssbo, GL_READ_WRITE);
        memcpy(&selectedID, data, sizeof(selectedID));
        memcpy(data, &initialValues, sizeof(initialValues));
        glUnmapNamedBuffer(m_ssbo);

        return selectedID;
    }

    void Picking::bindFramebuffer()
    {
        m_pickingBuffer->bind();
    }

}