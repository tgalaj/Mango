#pragma once

namespace mango
{
    class Camera
    {
    public:
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };
    public:
        Camera();

        /** verticalFov in radians. */
        void setPerspective(float verticalFov, float aspectRatio, float nearClip, float farClip);
        void setOrtographic(float size, float aspectRatio, float nearClip, float farClip);

        /** Returns vertical field of view angle in radians. */
        float getPerspectiveVerticalFieldOfView() const { return m_perspectiveFov; }

        /** verticalFov in radians. */
        void  setPerspectiveVerticalFieldOfView(float verticalFov) { m_perspectiveFov = verticalFov; recalculateProjection(); }

        float getPerspectiveNearClip() const { return m_perspectiveNear; }
        void  setPerspectiveNearClip(float nearClip) { m_perspectiveNear = nearClip; recalculateProjection(); }

        float getPerspectiveFarClip() const { return m_perspectiveFar; }
        void  setPerspectiveFarClip(float farClip) { m_perspectiveFar = farClip; recalculateProjection(); }

        float getOrthographicSize() const { return m_orthographicSize; }
        void  setOrthographicSize(float size) { m_orthographicSize = size; recalculateProjection(); }

        float getOrthographicNearClip() const { return m_orthographicNear; }
        void  setOrthographicNearClip(float nearClip) { m_orthographicNear = nearClip; recalculateProjection(); }

        float getOrthographicFarClip() const { return m_orthographicFar; }
        void  setOrthographicFarClip(float farClip) { m_orthographicFar = farClip; recalculateProjection(); }

        ProjectionType getProjectionType() const { return m_projectionType; }
        void setProjectionType(ProjectionType type) { m_projectionType = type; recalculateProjection(); }

        const glm::mat4& getView()       const { return m_view; }
        const glm::mat4& getProjection() const { return m_projection; }

        void setView(const glm::mat4& view) { m_view = view; }

    private:
        void recalculateProjection();

    private:
        glm::mat4 m_view{};
        glm::mat4 m_projection{};

        float m_perspectiveFov = glm::radians(60.0f);
        float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.0f;

        float m_orthographicSize = 10.0f;
        float m_orthographicNear = 0.01f, m_orthographicFar = 1000.0f;

        float m_aspectRatio = 1.0f;

        ProjectionType m_projectionType = ProjectionType::Perspective;
    };
}