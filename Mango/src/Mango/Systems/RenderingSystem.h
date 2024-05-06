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
    class JFAOutline;
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

    using DebugView = std::pair<std::string, ref<Texture>>;

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
        
        // Get the ID of the entity that was picked by the mouse
        int getSelectedEntityID(int mouseX, int mouseY);

        void setOutputToOffscreenTexture(bool enabled) { m_outputToOffscreenTexture = enabled; }
        uint32_t getOutputOffscreenTextureID() const;

        void setRenderingMode(RenderingMode mode, Camera* editorCamera = nullptr, const glm::vec3& editorCameraPosition = glm::vec3(0.0f));

        Camera& getCamera() const;
        glm::vec3 getCameraPosition() const { return m_cameraPosition; }

        RendererStatistics getStatistics() const { return m_statistics; }
        glm::uvec2 getMainFramebufferSize() const { return m_mainFramebufferSize; }

        void      addDebugTexture    (const std::string& viewName, const ref<Texture>& texture);
        DebugView getDebugView       (const std::string& viewName) const;
        void      setCurrentDebugView(const std::string& viewName);
        DebugView getCurrentDebugView() const { return m_currentDebugView; }
        auto      getDebugViewsMap   () const { return m_debugViews; }

    public:
        glm::vec3 sceneAmbientColor{};

        RenderingMode renderingMode = RenderingMode::GAME;

        glm::vec3 outlineColor                  = glm::vec3(0.9569, 0.7333, 0.2667);
        float     outlineWidth                  = 1.0f;
        bool      outlineUseSeparableAxisMethod = true;

        inline static bool         s_VisualizeLights           = false;
        inline static bool         s_VisualizePhysicsColliders = true;
        inline static ShadingMode  s_ShadingMode               = ShadingMode::SHADED;
        inline static unsigned int s_DebugWindowWidth          = 0;

        inline static glm::vec3 s_PhysicsCollidersColor = glm::vec3(0.247f, 0.629f, 0.208f);

    private:
        static void initRenderingStates();

        static void beginForwardRendering();
        static void endForwardRendering();

        void bindMainRenderTarget();

        void applyPostprocess(ref<PostprocessEffect>& effect, ref<RenderTarget>* src, ref<RenderTarget>* dst);

        void renderForward (Scene* scene);
        void renderDeferred(Scene* scene);
        void renderDebugView();
        void renderDebugLightsBoundingBoxes(Scene* scene);
        void renderDebugPhysicsColliders   (Scene* scene);
        void renderLightBillboards         (Scene* scene);

        void renderEntitiesInQueue(ref<Shader>& shader, std::vector<Entity>& queue);

        void renderLightsForward(Scene* scene);
        void renderLightsDeferred(Scene* scene);

        void sortAlpha();
        void addEntityToRenderQueue     (Entity entity, Material::RenderQueue renderQueue);
        void removeEntityFromRenderQueue(Entity entity, Material::RenderQueue renderQueue);

    private:
        enum TextureMaps { SHADOW_MAP = 5 }; //TODO: move to Material class

        // map that holds textures that we'd like to visualize
        std::unordered_map<std::string, ref<Texture>> m_debugViews;
        DebugView m_currentDebugView;

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
        ref<Shader> m_lightBillboardEditorShader;

        ref<Shader> m_gbufferShader;
        ref<Shader> m_deferredDirectional;
        ref<Shader> m_deferredPoint;
        ref<Shader> m_deferredSpot;

        ref<Shader> m_debugMeshShader;
        ref<Shader> m_nullShader;
        ref<Mesh>   m_lightBoundingSphere;
        ref<Mesh>   m_lightBoundingCone;

        ref<Mesh> m_physicsColliderBox;
        ref<Mesh> m_physicsColliderSphere;
        ref<Mesh> m_physicsColliderCapsule;
        ref<Mesh> m_debugSpotLightMesh;
        ref<Mesh> m_debugDirLightMesh;

        ref<PostprocessEffect> m_hdrFilter;
        ref<PostprocessEffect> m_fxaaFilter;
        ref<DeferredRendering> m_deferredRendering;
        ref<BloomPS>           m_bloomFilter;
        ref<SSAO>              m_ssao;
        ref<Picking>           m_picking;
        ref<JFAOutline>        m_jfaOutline;

        ref<RenderTarget> m_mainRenderTarget;
        ref<RenderTarget> m_helperRenderTarget;
        ref<RenderTarget> m_dirShadowMap;
        ref<RenderTarget> m_spotShadowMap;
        ref<RenderTarget> m_omniShadowMap;

        ref<Skybox> m_skybox;
        ref<Texture> m_dirLightSpriteTexture;
        ref<Texture> m_pointLightSpriteTexture;
        ref<Texture> m_spotLightSpriteTexture;

        glm::uvec2 m_mainFramebufferSize = {};

        RendererStatistics m_statistics = {};

        glm::vec3     m_cameraPosition = {};

        Camera * m_camera      = nullptr;
        Scene  * m_activeScene = nullptr;
        Window * m_mainWindow  = nullptr;

        bool m_outputToOffscreenTexture = false;
    };
}
