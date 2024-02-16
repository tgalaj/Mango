#pragma once
#include "Mango/Core/System.h"
#include "Mango/Events/EntityEvents.h"
#include "Mango/Events/SceneEvents.h"
#include "Mango/Rendering/Model.h"
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
    class ModelRendererComponent;
    class TransformComponent;
    class Camera;
    class Window;

    enum class RenderingMode { EDITOR, GAME };

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
        void receive(const ComponentAddedEvent<ModelRendererComponent>   & event);
        void receive(const ComponentReplacedEvent<ModelRendererComponent>& event);
        void receive(const ComponentRemovedEvent<ModelRendererComponent> & event);
        void receive(const ActiveSceneChangedEvent                       & event);

        void setSkybox(const std::shared_ptr<Skybox> & skybox);
        void resize(unsigned width, unsigned height);

        int getSelectedEntityID(int mouseX, int mouseY);

        void setOutputToOffscreenTexture(bool enabled) { m_outputToOffscreenTexture = enabled; }
        uint32_t getOutputOffscreenTextureID() const;

        void setRenderingMode(RenderingMode mode, Camera* editorCamera = nullptr, const glm::vec3& editorCameraPosition = glm::vec3(0.0f));

        Camera& getCamera() const;
        glm::vec3 getCameraPosition() const { return m_cameraPosition; }

        RendererStatistics getStatistics() const { return m_statistics; }
        glm::uvec2 getMainFramebufferSize() const { return m_mainFramebufferSize; }

    public:
        glm::vec3 sceneAmbientColor{};

        static bool         DEBUG_RENDERING;
        static unsigned int DEBUG_WINDOW_WIDTH;

        static std::unordered_map<Material::TextureType, std::shared_ptr<Texture>> s_defaultTextures;

    private:
        static void initRenderingStates();

        static void beginForwardRendering();
        static void endForwardRendering();

        void bindMainRenderTarget();

        void applyPostprocess(std::shared_ptr<PostprocessEffect>& effect, std::shared_ptr<RenderTarget>* src, std::shared_ptr<RenderTarget>* dst);

        void renderForward(Scene* scene);
        void renderDeferred(Scene* scene);
        void renderDebug();
        void renderDebugLightsBoundingBoxes(Scene* scene);

        void renderOpaque(const std::shared_ptr<Shader>& shader);
        void renderAlpha(const std::shared_ptr<Shader>& shader);
        void renderEnviroMappingStatic(const std::shared_ptr<Shader>& shader);
        void renderDynamicEnviroMapping(const std::shared_ptr<Shader>& shader);
        void renderLightsForward(Scene* scene);
        void renderLightsDeferred(Scene* scene);

        void sortAlpha();
        void addEntityToRenderQueue     (Entity entity, ModelRendererComponent::RenderQueue renderQueue);
        void removeEntityFromRenderQueue(Entity entity, ModelRendererComponent::RenderQueue renderQueue);

    private:
        enum TextureMaps { SHADOW_MAP = 5 }; //TODO: move to Material class

        std::vector<Entity> m_opaqueQueue;
        std::vector<Entity> m_alphaQueue;
        std::vector<Entity> m_enviroStaticQueue;
        std::vector<Entity> m_enviroDynamicQueue;

        std::shared_ptr<Shader> m_forwardAmbient;
        std::shared_ptr<Shader> m_forwardDirectional;
        std::shared_ptr<Shader> m_forwardPoint;
        std::shared_ptr<Shader> m_forwardSpot;
        std::shared_ptr<Shader> m_shadowMapGenerator;
        std::shared_ptr<Shader> m_omniShadowMapGenerator;
        std::shared_ptr<Shader> m_blendingShader;
        std::shared_ptr<Shader> m_enviroMappingShader;
        std::shared_ptr<Shader> m_debugRendering;

        std::shared_ptr<Shader> m_gbufferShader;
        std::shared_ptr<Shader> m_deferredDirectional;
        std::shared_ptr<Shader> m_deferredPoint;
        std::shared_ptr<Shader> m_deferredSpot;

        std::shared_ptr<Shader> m_boundingboxShader;
        std::shared_ptr<Shader> m_nullShader;
        Model m_lightBoundingSphere;
        Model m_lightBoundingCone;

        std::shared_ptr<PostprocessEffect> m_hdrFilter;
        std::shared_ptr<PostprocessEffect> m_fxaaFilter;
        std::shared_ptr<DeferredRendering> m_deferredRendering;
        std::shared_ptr<BloomPS> m_bloomFilter;
        std::shared_ptr<SSAO> m_ssao;
        std::shared_ptr<Picking> m_picking;

        std::shared_ptr<RenderTarget> m_mainRenderTarget;
        std::shared_ptr<RenderTarget> m_helperRenderTarget;
        std::shared_ptr<RenderTarget> m_dirShadowMap;
        std::shared_ptr<RenderTarget> m_spotShadowMap;
        std::shared_ptr<RenderTarget> m_omniShadowMap;

        std::shared_ptr<Skybox> m_skybox;

        glm::uvec2 m_mainFramebufferSize = {};

        RendererStatistics m_statistics = {};

        RenderingMode m_mode           = RenderingMode::GAME;
        glm::vec3     m_cameraPosition = {};

        Camera * m_camera      = nullptr;
        Scene  * m_activeScene = nullptr;
        Window * m_mainWindow  = nullptr;

        bool m_outputToOffscreenTexture = false;
    };
}
