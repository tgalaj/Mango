#pragma once
#include "Mango/Core/System.h"
#include "Mango/Events/EntityEvents.h"
#include "Mango/Events/SceneEvents.h"
#include "Mango/Rendering/AnimatedMesh.h"
#include "Mango/Rendering/Skybox.h"
#include "Mango/Scene/Entity.h"

namespace mango
{
    class Skybox;
    class PostprocessEffect;
    class RenderTarget;
    class Shader;
    class BloomPS;
    class Picking;
    class SSAO;
    class DeferredRendering;
    class StaticMeshComponent;
    class TransformComponent;
    class Camera;
    class Window;

    enum class RenderingMode { EDITOR, GAME };
    enum class ShadingMode   { SHADED, WIREFRAME, SHADED_WIREFRAME };

    struct RendererStatistics
    {
        std::string vendorName    = "";
        std::string rendererName  = "";
        std::string driverVersion = "";
        std::string glslVersion   = "";
    };

    class RenderingSystem : public System
    {
    public:
        RenderingSystem();
        ~RenderingSystem() = default;

        void onInit();
        void onUpdate(float dt);
        void onDestroy();

        void receive(const EntityRemovedEvent                            & event);
 /*       void receive(const ComponentAddedEvent<StaticMeshComponent>   & event);
        void receive(const ComponentReplacedEvent<StaticMeshComponent>& event);
        void receive(const ComponentRemovedEvent<StaticMeshComponent> & event);*/
        void receive(const ActiveSceneChangedEvent                       & event);

        void setSkybox(const ref<Skybox> & skybox);
        void resize(unsigned width, unsigned height);

        int getSelectedEntityID(int mouseX, int mouseY);

        void setOutputToOffscreenTexture(bool enabled) { m_outputToOffscreenTexture = enabled; }
        uint32_t getOutputOffscreenTextureID() const;

        void setRenderingMode(RenderingMode mode, Camera* editorCamera = nullptr, const glm::vec3& editorCameraPosition = glm::vec3(0.0f));
        void setShadingMode(ShadingMode mode) { shadingMode = mode; }

        Camera& getCamera() const;
        glm::vec3 getCameraPosition() const { return m_cameraPosition; }

        RendererStatistics getStatistics() const { return m_statistics; }
        glm::uvec2 getMainFramebufferSize() const { return m_mainFramebufferSize; }

    public:
        glm::vec3 sceneAmbientColor{};

        RenderingMode renderingMode = RenderingMode::GAME;
        ShadingMode   shadingMode   = ShadingMode::SHADED;

        static bool         DEBUG_RENDERING;
        static unsigned int DEBUG_WINDOW_WIDTH;

    private:
        static void initRenderingStates();

        static void beginForwardRendering();
        static void endForwardRendering();

        void bindMainRenderTarget();

        void applyPostprocess(ref<PostprocessEffect>& effect, ref<RenderTarget>* src, ref<RenderTarget>* dst);

        void renderForward(Scene* scene);
        void renderDeferred(Scene* scene);
        void renderDebug();
        void renderDebugLightsBoundingBoxes(Scene* scene);

        void renderEntitiesInQueue(ref<Shader>& shader, std::vector<Entity>& queue);

        void renderLightsForward(Scene* scene);
        void renderLightsDeferred(Scene* scene);

        void sortAlpha();
        void addEntityToRenderQueue     (Entity entity, Material::RenderQueue renderQueue);
        void removeEntityFromRenderQueue(Entity entity, Material::RenderQueue renderQueue);

    private:
        enum TextureMaps { SHADOW_MAP = 5 }; //TODO: move to Material class

        // TODO(TG): these should be te vectors of Submeshes* instead of entities
        // NOTE(TG): leaving as is for now, as the renderer will be reworked from the ground
        std::vector<Entity> m_opaqueQueue;
        std::vector<Entity> m_alphaQueue;
        std::vector<Entity> m_enviroStaticQueue;
        std::vector<Entity> m_enviroDynamicQueue;

        ref<Shader> m_forwardAmbient;
        ref<Shader> m_forwardDirectional;
        ref<Shader> m_forwardPoint;
        ref<Shader> m_forwardSpot;
        ref<Shader> m_shadowMapGenerator;
        ref<Shader> m_omniShadowMapGenerator;
        ref<Shader> m_blendingShader;
        ref<Shader> m_enviroMappingShader;
        ref<Shader> m_debugRendering;
        ref<Shader> m_wireframeShader;

        ref<Shader> m_gbufferShader;
        ref<Shader> m_deferredDirectional;
        ref<Shader> m_deferredPoint;
        ref<Shader> m_deferredSpot;

        ref<Shader> m_boundingboxShader;
        ref<Shader> m_nullShader;
        ref<Mesh>   m_lightBoundingSphere;
        ref<Mesh>   m_lightBoundingCone;

        ref<PostprocessEffect> m_hdrFilter;
        ref<PostprocessEffect> m_fxaaFilter;
        ref<DeferredRendering> m_deferredRendering;
        ref<BloomPS>           m_bloomFilter;
        ref<SSAO>              m_ssao;
        ref<Picking>           m_picking;

        ref<RenderTarget> m_mainRenderTarget;
        ref<RenderTarget> m_helperRenderTarget;
        ref<RenderTarget> m_dirShadowMap;
        ref<RenderTarget> m_spotShadowMap;
        ref<RenderTarget> m_omniShadowMap;

        ref<Skybox> m_skybox;

        glm::uvec2 m_mainFramebufferSize = {};

        RendererStatistics m_statistics = {};

        glm::vec3     m_cameraPosition = {};

        Camera * m_camera      = nullptr;
        Scene  * m_activeScene = nullptr;
        Window * m_mainWindow  = nullptr;

        bool m_outputToOffscreenTexture = false;
    };
}
