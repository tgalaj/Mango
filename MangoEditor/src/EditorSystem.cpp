#define GLFW_INCLUDE_NONE 

#include "EditorSystem.h"
#include "Mango/Math/Math.h"
#include "Mango/Scene/SceneSerializer.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <ImGuizmo.h>
#include <portable-file-dialogs.h>

namespace
{
    ImGuizmo::OPERATION getImGuizmoOp(mango::GizmoType type)
    {
        switch (type)
        {
            case mango::GizmoType::TRANSLATE: return ImGuizmo::OPERATION::TRANSLATE;
            case mango::GizmoType::ROTATE:    return ImGuizmo::OPERATION::ROTATE;
            case mango::GizmoType::SCALE:     return ImGuizmo::OPERATION::SCALE;
        }

        MG_ASSERT_FAIL("Unknown GizmoType!");
        return ImGuizmo::OPERATION::TRANSLATE;
    }

    ImGuizmo::MODE getImGuizmoMode(mango::GizmoMode mode)
    {
        switch (mode)
        {
            case mango::GizmoMode::LOCAL: return ImGuizmo::MODE::LOCAL;
            case mango::GizmoMode::WORLD: return ImGuizmo::MODE::WORLD;
        }

        MG_ASSERT_FAIL("Unknown GizmoMode!");
        return ImGuizmo::MODE::LOCAL;
    }
}

namespace mango
{
    EditorSystem::EditorSystem()
        : System("EditorSystem")
    {
        // Set CVars
        CVarFloat CVarCameraRotationSpeed("editor.camera.rotationSpeed", "rotation speed of the editor camera", 0.2f);
        CVarFloat CVarCameraMoveSpeed    ("editor.camera.moveSpeed",     "movement speed of the editor camera", 10.0f);
    }

    void EditorSystem::onInit()
    {
        m_playIcon = std::make_shared<Texture>();
        m_playIcon->createTexture2d("icons/play.png", false, 8);

        m_stopIcon = std::make_shared<Texture>();
        m_stopIcon->createTexture2d("icons/stop.png", false, 8);

        m_pauseIcon = std::make_shared<Texture>();
        m_pauseIcon->createTexture2d("icons/pause.png", false, 8);

        m_stepIcon = std::make_shared<Texture>();
        m_stepIcon->createTexture2d("icons/step.png", false, 8);

        m_mainScene = Services::sceneManager()->createScene("Editor Scene");
        Services::sceneManager()->setActiveScene(m_mainScene);

        m_sceneHierarchyPanel.setScene(m_mainScene);

        m_editorCamera.setPerspective(glm::radians(45.0f), Services::application()->getWindow()->getAspectRatio(), 0.1f, 1000.0f);
        m_editorCamera.setPosition(glm::vec3(0.0f, 4.0f, 30.0f));

        Services::renderer()->setOutputToOffscreenTexture(true);
        Services::application()->getWindow()->setVSync(false);
        Services::application()->getImGuiSystem()->setDefaultIniSettingsFile("imgui.ini");

        Services::eventBus()->subscribe<GamepadConnectedEvent>([](const GamepadConnectedEvent& event)
        {
            if (event.isConnected)
            {
                MG_CORE_INFO("Conencted gamepad:\n  ID: {}\n  Name: {}", int(event.gid), Input::getGamepadName(event.gid));
            }
            else
            {
                MG_CORE_ERROR("Disconencted gamepad:\n  ID: {}", int(event.gid));
            }
        });

        auto camera1 = m_mainScene->createEntity("MainCamera");
        camera1.addComponent<CameraComponent>().camera.setPerspective(glm::radians(45.0f), Services::application()->getWindow()->getAspectRatio(), 0.1f, 1000.0f);
        camera1.setPosition(0, 4, 30);

        auto camera2 = m_mainScene->createEntity("MainCamera2");
        auto& cc2 = camera2.addComponent<CameraComponent>();
        cc2.camera.setPerspective(glm::radians(45.0f), Services::application()->getWindow()->getAspectRatio(), 0.1f, 1000.0f);
        cc2.isPrimary = false;
        camera2.setPosition(0, 4, -30);
        camera2.setOrientation({0, 1, 0}, glm::radians(180.0f));

        auto font = AssetManager::createFont("Droid48", "fonts/Roboto-Regular.ttf", 48.0f);

        /*auto skybox = std::make_shared<Skybox>("skyboxes/stormydays/",
                                                       "stormydays_lf.tga",
                                                       "stormydays_rt.tga",
                                                       "stormydays_up.tga", 
                                                       "stormydays_dn.tga", 
                                                       "stormydays_ft.tga", 
                                                       "stormydays_bk.tga" );*/
        auto skybox = std::make_shared<Skybox>("skyboxes/cold/",
                                                       "right.jpg",
                                                       "left.jpg",
                                                       "top.jpg", 
                                                       "bottom.jpg", 
                                                       "front.jpg", 
                                                       "back.jpg" );
        Services::renderer()->setSkybox(skybox);

        auto sphereModel = AssetManager::createModel();
        sphereModel.genSphere(0.5f, 24);

        auto cyborgModel        = AssetManager::createModel("models/cyborg/cyborg.obj");
        auto zen3cModel         = AssetManager::createModel("models/Zen3C/Zen3C.X");
        auto damagedHelmetModel = AssetManager::createModel("models/damaged_helmet/DamagedHelmet.gltf");
        auto sponzaModel        = AssetManager::createModel("models/sponza/Sponza.gltf");

        auto wallModel = AssetManager::createModel();
        wallModel.genQuad(5, 5);

        auto groundTex           = AssetManager::createTexture2D("textures/trak_tile_g.jpg", true);
        auto brickwallTex        = AssetManager::createTexture2D("textures/brickwall.dds", true);
        auto brickwallNormalTex  = AssetManager::createTexture2D("textures/brickwall_normal.jpg");
        auto bricks2             = AssetManager::createTexture2D("textures/bricks2.jpg", true);
        auto bricks2Depth        = AssetManager::createTexture2D("textures/bricks2_disp.jpg");
        auto bricks2Normal       = AssetManager::createTexture2D("textures/bricks2_normal.jpg");
        auto windowTex           = AssetManager::createTexture2D("textures/window.png");
        auto grassTex            = AssetManager::createTexture2D("textures/grass.png");
        auto openglLogo          = AssetManager::createTexture2D("textures/opengl.png");

        auto cyborg = m_mainScene->createEntity("Cyborg");
        cyborg.addComponent<ModelRendererComponent>(cyborgModel);
        cyborg.setPosition(0.0f, 0.0f, 0.0f);
        cyborg.setScale(1.0f);

        auto zen3c = m_mainScene->createEntity("Zen3c");
        zen3c.addComponent<ModelRendererComponent>(zen3cModel);
        zen3c.setPosition(-3.0f, -2.3f, 0.0f);
        zen3c.setScale(0.018f);

        auto damagedHelmet = m_mainScene->createEntity("Damaged Helmet");
        damagedHelmet.addComponent<ModelRendererComponent>(damagedHelmetModel);
        damagedHelmet.setPosition(3.0f, 2.5f, 0.0f);
        damagedHelmet.setScale(1.0f);

        auto sponza = m_mainScene->createEntity("Sponza");
        sponza.addComponent<ModelRendererComponent>(sponzaModel);
        sponza.setPosition(-1.5f, 0.0f, 10.0f);
        sponza.setRotation(0.0f, -90.0f, 0.0f);
        sponza.setScale(6.0f);

        auto wall = m_mainScene->createEntity("Brickwall");
        wall.addComponent<ModelRendererComponent>(wallModel);
        wall.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, brickwallTex);
        wall.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::NORMAL, brickwallNormalTex);
        //wall.getComponent<ModelRendererComponent>()->model.getMesh().material.addTexture(Material::TextureType::DEPTH, bricks2Depth);
        wall.setRotation(90.0f, 0.0f, 0.0f);
        wall.setPosition(0, 2.0, -9);

        auto wall2 = m_mainScene->createEntity("Parallax brickwall");
        wall2.addComponent<ModelRendererComponent>(wallModel);
        wall2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, bricks2);
        wall2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::NORMAL, bricks2Normal);
        wall2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DEPTH, bricks2Depth);
        wall2.setRotation(90.0f, 0.0f, 0.0f);
        wall2.setPosition(-5, 2.0, -9);

        auto grass = m_mainScene->createEntity("Grass");
        grass.addComponent<ModelRendererComponent>(wallModel);
        grass.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, grassTex);
        grass.getComponent<ModelRendererComponent>().model.getMesh().material.addFloat("alpha_cutoff", 0.1f);
        grass.setRotation(90.0f, 0.0f, 0.0f);
        grass.setPosition(-5, 1.2, 9);
        grass.setScale(0.5);

        auto window1 = m_mainScene->createEntity("Window1");
        window1.addComponent<ModelRendererComponent>(wallModel, ModelRendererComponent::RenderQueue::RQ_ALPHA);
        window1.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, windowTex);
        window1.setRotation(90.0f, 0.0f, 0.0f);
        window1.setPosition(0, 1.2, 9);
        window1.setScale(0.51);

        auto window3 = m_mainScene->createEntity("Window3");
        window3.addComponent<ModelRendererComponent>(wallModel, ModelRendererComponent::RenderQueue::RQ_ALPHA);
        window3.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, windowTex);
        window3.setRotation(90.0f, 0.0f, 0.0f);
        window3.setPosition(3, 1.2, 13);
        window3.setScale(0.5);

        auto window2 = m_mainScene->createEntity("Window2");
        window2.addComponent<ModelRendererComponent>(wallModel, ModelRendererComponent::RenderQueue::RQ_ALPHA);
        window2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, windowTex);
        window2.setRotation(90.0f, 0.0f, 0.0f);
        window2.setPosition(5, 1.2, 9);
        window2.setScale(0.5);

        auto plane1 = m_mainScene->createEntity("Plane1");
        plane1.addComponent<ModelRendererComponent>(wallModel);
        plane1.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, bricks2);
        plane1.setRotation(90.0f, 0.0f, 0.0f);
        plane1.setPosition(5, 3.0, -14);

        auto plane2 = m_mainScene->createEntity("Plane2");
        plane2.addComponent<ModelRendererComponent>(wallModel);
        plane2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, bricks2);
        plane2.setRotation(0.5f, 0.0f, 0.0f);
        plane2.setPosition(5, 0.5, -11.5);
        plane2.addComponent<BoxCollider3DComponent>(glm::vec3(2.5f, 0.1f, 2.5f), glm::vec3(0.0f, -0.1f, 0.0f));
        plane2.addComponent<RigidBody3DComponent>();

        auto sphere1 = m_mainScene->createEntity("Bouncing Sphere");
        sphere1.addComponent<ModelRendererComponent>(sphereModel, ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_STATIC);
        sphere1.setPosition(5, 20, -11.5);
        sphere1.setScale(0.5f);
        sphere1.addComponent<SphereColliderComponent>(0.5f);

        auto& rb3dSphere1                      = sphere1.addComponent<RigidBody3DComponent>();
              rb3dSphere1.motionType           = RigidBody3DComponent::MotionType::Dynamic;
              rb3dSphere1.isInitiallyActivated = true;
              rb3dSphere1.friction             = 0.1f;
              rb3dSphere1.linearDamping        = 0.05f;
              rb3dSphere1.restitution          = 0.7f;
    
        auto sphere2 = m_mainScene->createEntity("Static sphere");
        sphere2.addComponent<ModelRendererComponent>(sphereModel);
        sphere2.setPosition(5, 3, -12.5);
        sphere2.setScale(0.5f);

        float offset = 15.0f / 10.0f;
        float numObjects = 10;

        for(int i = 0; i < numObjects; ++i)
        {
            float xPos = 15.0f / -2.0f + (i * offset);

            for(int j = 0; j < numObjects; ++j)
            {
                auto object = m_mainScene->createEntity("Sphere" + std::to_string(int(i * numObjects + j + 1)));
                object.addComponent<ModelRendererComponent>(sphereModel);
                object.setScale(0.75f);
            
                float zPos = 15.0f / -2.0f + (j * offset);
                object.setPosition(xPos + 0.5f * offset, 0.5f, zPos + 0.5f * offset);

                if (i == 2 && j == 2)
                {
                    cyborg.addChild(object);
                    //object.setScale(4.0f);
                }
            }
        }

        /* Lights */
        auto dirLight = m_mainScene->createEntity("DirectionalLight");
        dirLight.addComponent<DirectionalLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 4.0f, 200.0f, true);
        dirLight.setRotation(-45.0f, 180.0f, 0.0f);

        float d = 8;
        glm::vec3 positions[4]
        {
            glm::vec3(-d, 1.2,  0),
            glm::vec3( d, 1.2,  0),
            glm::vec3( 0, 1.2,  d),
            glm::vec3( 0, 1.2, -d),
        };

        glm::vec3 colors[4]
        {
            glm::vec3(1,0,0),
            glm::vec3(0,1,0),
            glm::vec3(0,0,1),
            glm::vec3(1,1,0)
        };

        for (int i = 0; i < 4; ++i)
        {
            auto pointLight = m_mainScene->createEntity("PointLight" + std::to_string(i));
            pointLight.addComponent<PointLightComponent>(true);
            pointLight.getComponent<PointLightComponent>().setAttenuation(3.0f, 4.0f, 10.0f);
            pointLight.getComponent<PointLightComponent>().color = colors[i];
            pointLight.getComponent<PointLightComponent>().intensity = 200.0f;
            pointLight.setPosition(positions[i].x, positions[i].y, positions[i].z);

            //if (i == 0)
            //    pointLight.getComponent<TransformComponent>()->addChild(nanobot.getComponent<TransformComponent>());
        }

        auto spotLight = m_mainScene->createEntity("SpotLight");
        spotLight.addComponent<SpotLightComponent>();
        spotLight.getComponent<SpotLightComponent>().color = glm::vec3(255, 206, 250) / 255.0f;
        spotLight.getComponent<SpotLightComponent>().intensity = 1000.0f;
        spotLight.getComponent<SpotLightComponent>().setCutOffAngle(30.0f);
        spotLight.setPosition(1.5, 5, 1.5);
        spotLight.setRotation(-45, 45, 45);
        spotLight.getComponent<SpotLightComponent>().setCastsShadows(true);
    }

    void EditorSystem::onDestroy()
    {
    }

    void EditorSystem::onUpdate(float dt)
    {
        if (SceneState::Edit == m_sceneState)
        {
            bool ctrl  = Input::getKey(KeyCode::LeftControl) || Input::getKey(KeyCode::RightControl);
            bool shift = Input::getKey(KeyCode::LeftShift)   || Input::getKey(KeyCode::RightShift);

            if (Input::getKeyDown(KeyCode::N))
            {
                if (ctrl)
                {
                    newScene();
                    return;
                }
            }

            if (Input::getKeyDown(KeyCode::O))
            {
                if (ctrl)
                {
                    openScene();
                    return;
                }
            }

            if (Input::getKeyDown(KeyCode::S))
            {
                if (ctrl)
                {
                    if (shift)
                    {
                        saveSceneAs();
                        return;
                    }
                
                    saveScene();
                    return;
                }
            }

            if (Input::getKeyDown(KeyCode::Alpha1))
            {
                m_gizmoType = GizmoType::TRANSLATE;
            }

            if (Input::getKeyDown(KeyCode::Alpha2))
            {
                m_gizmoType = GizmoType::ROTATE;
            }

            if (Input::getKeyDown(KeyCode::Alpha3))
            {
                m_gizmoType = GizmoType::SCALE;
            }

            if (Input::getKeyDown(KeyCode::Alpha0))
            {
                m_gizmoType = GizmoType::NONE;
            }

            if (Input::getKeyDown(KeyCode::Alpha4))
            {
                m_gizmoMode = m_gizmoMode == GizmoMode::LOCAL ? GizmoMode::WORLD : GizmoMode::LOCAL;
            }

            /** TODO: BELOW TO BE REMOVED */
            static bool isDebugRender = false;
    
            if (Input::getKeyDown(KeyCode::Escape) || Input::getGamepadButtonDown(GamepadID::PAD_1, GamepadButton::BACK))
            {
                Services::application()->stop();
            }

            if (Input::getKeyDown(KeyCode::H) || Input::getGamepadButtonDown(GamepadID::PAD_1, GamepadButton::Y))
            {
                isDebugRender = !isDebugRender;
                RenderingSystem::DEBUG_RENDERING = isDebugRender;
            }

            static bool fullscreen = Services::application()->getWindow()->isFullscreen();
            if (Input::getKeyDown(KeyCode::F11))
            {
                fullscreen = !fullscreen;
                Services::application()->getWindow()->setFullscreen(fullscreen);
            }

            m_editorCamera.onUpdate(dt);
        }
        else if (SceneState::Play == m_sceneState)
        {
            static bool shouldMoveLights = false;
            if (Input::getKeyDown(KeyCode::Space) || Input::getGamepadButtonDown(GamepadID::PAD_1, GamepadButton::X))
            {
                shouldMoveLights = !shouldMoveLights;
            }

            if (shouldMoveLights)
            {
                moveLights(dt);
            }
        }
    }
    
    void EditorSystem::onGui()
    {    
        // Resize the main FBO based not on the window, but based on the Viewport panel !!

        // Resize
        if (glm::uvec2 mainFramebufferSize = Services::renderer()->getMainFramebufferSize();
            m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (mainFramebufferSize.x != m_viewportSize.x || mainFramebufferSize.y != m_viewportSize.y))
        {
            Services::renderer()->resize(m_viewportSize.x, m_viewportSize.y);
        }

        static bool dockspaceOpen = true;
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
            windowFlags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Mango Editor", &dockspaceOpen, windowFlags);
        ImGui::PopStyleVar(3);

        // Submit the DockSpace
        ImGuiIO&    io                    = ImGui::GetIO();
        ImGuiStyle& style                 = ImGui::GetStyle();
        float       minImGuiWindowSizeX   = style.WindowMinSize.x;
                    style.WindowMinSize.x = 370.0f;

        ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }
        style.WindowMinSize.x = minImGuiWindowSizeX;

        /** Menu Bar */
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                {
                    newScene();
                }

                if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
                {
                    openScene();
                }

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                {
                    saveScene();
                }

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                {
                    saveSceneAs();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit")) Services::application()->stop();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Begin("CVars Editor");
        {
            CVarSystem::get()->drawImguiEditor();
        }
        ImGui::End(); // CVars Editor

        
        onGuiViewport();
        m_sceneHierarchyPanel.onGui();
        m_assetsBrowserPanel.onGui();
        onGuiToolbar();
        onGuiStats();

        ImGui::End(); // Mango Editor
    }

    void EditorSystem::newScene()
    {
        m_mainScene = Services::sceneManager()->createScene("New Scene");
        Services::sceneManager()->setActiveScene(m_mainScene);
        m_sceneHierarchyPanel.setScene(m_mainScene);

        m_editorScenePath = std::filesystem::path();
    }

    void EditorSystem::openScene()
    {
        auto file = pfd::open_file("", "." /*TODO: project path here*/, { "Mango Scene (*" MG_SCENE_EXTENSION ")", "*" MG_SCENE_EXTENSION }, false);
        
        if (!file.result().empty())
        {
            openScene(file.result()[0]);
        }
    }

    void EditorSystem::openScene(const std::filesystem::path& path)
    {
        if (path.extension().string() != MG_SCENE_EXTENSION)
        {
            MG_WARN("Could not load {0} - not a scene file", path.filename().string());
            return;
        }

        m_editorScenePath = path;
        m_mainScene       = SceneSerializer::deserialize(m_editorScenePath);
        Services::sceneManager()->setActiveScene(m_mainScene);
        m_sceneHierarchyPanel.setScene(m_mainScene);
    }

    void EditorSystem::saveScene()
    {
        if (!m_editorScenePath.empty())
        {
            SceneSerializer::serialize(m_mainScene, m_editorScenePath);
        }
        else
        {
            saveSceneAs();
        }
    }

    void EditorSystem::saveSceneAs()
    {
        auto file = pfd::save_file("", "." /*TODO: project path here*/, { "Mango Scene (*" MG_SCENE_EXTENSION ")", "*" MG_SCENE_EXTENSION }, true);
        if (!file.result().empty())
        {
            m_editorScenePath = file.result();

            if (!m_editorScenePath.has_extension())
            {
                m_editorScenePath += ".mango";
            }

            SceneSerializer::serialize(m_mainScene, m_editorScenePath);
        }
    }

    void EditorSystem::onScenePlay()
    {
        m_sceneState = SceneState::Play;
    }

    void EditorSystem::onSceneStep()
    {
        m_sceneState = SceneState::Step;
    }

    void EditorSystem::onSceneStop()
    {
        m_sceneState = SceneState::Edit;
    }

    void EditorSystem::onScenePause()
    {
        
    }

    void EditorSystem::onGuiViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");
        {
            if (SceneState::Edit == m_sceneState)
            {
                Services::renderer()->setRenderingMode(RenderingMode::EDITOR, &m_editorCamera, m_editorCamera.getPosition());
            }
            else if (SceneState::Play == m_sceneState)
            {
                Services::renderer()->setRenderingMode(RenderingMode::GAME);
            }

            auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
            auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
            auto viewportOffset    = ImGui::GetWindowPos();

            m_viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
            m_viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

            m_viewportFocused = ImGui::IsWindowFocused();
            m_viewportHovered = ImGui::IsWindowHovered();

            if (m_viewportHovered)
            {
                ImGui::SetNextFrameWantCaptureKeyboard(false);
                ImGui::SetNextFrameWantCaptureMouse(false);
            }

            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
                   m_viewportSize    = { viewportPanelSize.x, viewportPanelSize.y };

            uint32_t outputTextureID = Services::renderer()->getOutputOffscreenTextureID();

            ImGui::Image((ImTextureID)outputTextureID, viewportPanelSize, { 0, 1 }, { 1, 0 });

            /** Drag drop target */
            if (ImGui::BeginDragDropTarget())
            {
                // TODO: consider moving payload type to a shared file (make a define or static const)
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MG_ASSETS_BROWSER_ITEM"))
                {
                    const auto* path = (const wchar_t*)payload->Data;
                    openScene(path);
                }
                ImGui::EndDragDropTarget();
            }

            if (SceneState::Edit == m_sceneState)
            {
                /** ImGuizmo */
                Entity selectedEntity = m_sceneHierarchyPanel.getSelectedEntity();
                if (selectedEntity && m_gizmoType != GizmoType::NONE)
                {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();

                    ImGuizmo::SetRect(m_viewportBounds[0].x, m_viewportBounds[0].y, m_viewportBounds[1].x - m_viewportBounds[0].x, m_viewportBounds[1].y - m_viewportBounds[0].y);

                    // Camera
                    const glm::mat4& cameraView       = m_editorCamera.getView();
                    const glm::mat4& cameraProjection = m_editorCamera.getProjection();

                    // Entity transform
                    auto& tc = selectedEntity.getComponent<TransformComponent>();
                    glm::mat4 transform = tc.getWorldMatrix();

                    // Snapping
                    bool snap = Input::getKey(KeyCode::LeftControl);
                    float snapValue = 0.5f; // Snap to 0.5m for translation and scale

                    // Snap to 45 degrees for rotation
                    if (m_gizmoType == GizmoType::ROTATE)
                    {
                        snapValue = 45.0f;
                    }
                    glm::vec3 snapValues = glm::vec3(snapValue);

                    ImGuizmo::Manipulate(glm::value_ptr(cameraView), 
                                         glm::value_ptr(cameraProjection), 
                                         getImGuizmoOp(m_gizmoType), 
                                         getImGuizmoMode(m_gizmoMode), 
                                         glm::value_ptr(transform),
                                         nullptr /*deltaMatrix*/,
                                         snap ? glm::value_ptr(snapValues) : nullptr);

                    if (ImGuizmo::IsUsing())
                    {
                        glm::vec3 translation, rotation, scale;
                        math::decompose(transform,
                                        translation,
                                        rotation,
                                        scale);

                        // Calculate delta rotation, to always add 
                        // small portion of the rotation to avoid the gimbal lock
                        glm::vec3 currentRotation = tc.getRotation();
                        glm::vec3 deltaRotation   = rotation - currentRotation;

                        tc.setPosition(translation);
                        tc.setRotation(currentRotation + deltaRotation);
                        tc.setScale(scale);
                    }
                }

                // Mouse Entity picking
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver() && m_viewportHovered)
                {
                    auto mousePos = Input::getMousePosition();
                    mousePos -= m_viewportBounds[0];

                    auto viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
                    mousePos.y = viewportSize.y - mousePos.y;

                    if (mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < (int)viewportSize.x && mousePos.y < (int)viewportSize.y)
                    {
                        int    selectedID = Services::renderer()->getSelectedEntityID(mousePos.x, mousePos.y);
                        Entity selectedEntity = selectedID == -1 ? Entity() : Entity((entt::entity)selectedID, m_mainScene.get());

                        m_sceneHierarchyPanel.setSelectedEntity(selectedEntity);
                    }
                }
            }
        }
        ImGui::End(); // Viewport
        ImGui::PopStyleVar();
    }

    void EditorSystem::onGuiToolbar()
    {        
        const ImVec2 iconSize   (32.0f, 32.0f);
        const ImVec2 itemSpacing(2.0f, 5.0f);

        beginDockingToolbar("SceneState", ImGuiAxis_X, iconSize);
        {
            auto toolbarWidth = ImGui::GetContentRegionAvail().x;

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,  itemSpacing);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

            auto&       colors        = ImGui::GetStyle().Colors;
            const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
            const auto& buttonActive  = colors[ImGuiCol_ButtonActive];
            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(buttonActive.x,  buttonActive.y,  buttonActive.z,  0.5f));

            ImGui::SetCursorPosX((toolbarWidth - 3.0f * (iconSize.x + itemSpacing.x)) * 0.5f); // 3 - no. buttons to center
            
            auto playStopIcon = (SceneState::Edit == m_sceneState) ? m_playIcon : m_stopIcon;
            if (ImGui::ImageButton("playStopButton", (ImTextureID)playStopIcon->getRendererID(), iconSize))
            {
                if (SceneState::Edit == m_sceneState)
                {
                    onScenePlay();
                }
                else if (SceneState::Play == m_sceneState)
                {
                    onSceneStop();
                }
            }
            
            ImGui::SameLine();
            ImGui::ImageButton("pauseButton", (ImTextureID)m_pauseIcon->getRendererID(), iconSize);

            ImGui::SameLine();
            ImGui::ImageButton("stepButton", (ImTextureID)m_stepIcon->getRendererID(), iconSize);

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);
        }
        endDockingToolbar();
    }

    void EditorSystem::onGuiStats()
    {
        ImGui::Begin("Stats");
        {
            auto stats = Services::renderer()->getStatistics();
            ImGui::Text("%s\nDriver: %s\nGLSL Version: %s\n\n", stats.rendererName.c_str(),
                        stats.driverVersion.c_str(),
                        stats.glslVersion.c_str());
            ImGui::Text("Frame Rate: %.3f ms/frame (%.1f FPS)", Services::application()->getFramerate(), 1000.0f / Services::application()->getFramerate());
        }
        ImGui::End(); // Stats
    }

        void EditorSystem::beginDockingToolbar(const char* name, ImGuiAxis toolbarAxis, const ImVec2& iconSize)
    {
        // 1. We request auto-sizing on one axis
        // Note however this will only affect the toolbar when NOT docked.
        ImVec2 requestedSize = (toolbarAxis == ImGuiAxis_X) ? ImVec2(-1.0f, 0.0f) : ImVec2(0.0f, -1.0f);
        ImGui::SetNextWindowSize(requestedSize);

        // 2. Specific docking options for toolbars.
        // Currently they add some constraint we ideally wouldn't want, but this is simplifying our first implementation
        ImGuiWindowClass window_class;
        window_class.DockingAllowUnclassed     = true;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoCloseButton;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_HiddenTabBar; // ImGuiDockNodeFlags_NoTabBar // FIXME: Will need a working Undock widget for _NoTabBar to work
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingSplit;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingOverMe;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingOverOther;
        if (toolbarAxis == ImGuiAxis_X)
            window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoResizeY;
        else
            window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoResizeX;

        ImGui::SetNextWindowClass(&window_class);

        // 3. Begin into the window
        ImGui::Begin(name, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

        // 4. Overwrite node size
        ImGuiDockNode* node = ImGui::GetWindowDockNode();
        if (node != NULL)
        {
            // Overwrite size of the node
                  ImGuiStyle& style                    = ImGui::GetStyle();
            const ImGuiAxis   toolbarAxisPerp          = (ImGuiAxis)(toolbarAxis ^ 1);
            const float       TOOLBAR_SIZE_WHEN_DOCKED = style.WindowPadding[toolbarAxisPerp] * 2.0f + iconSize[toolbarAxisPerp];

            node->WantLockSizeOnce      = true;
            node->Size[toolbarAxisPerp] = node->SizeRef[toolbarAxisPerp] = TOOLBAR_SIZE_WHEN_DOCKED;

            if (node->ParentNode && node->ParentNode->SplitAxis != ImGuiAxis_None)
                toolbarAxis = (ImGuiAxis)(node->ParentNode->SplitAxis ^ 1);
        }
    }

    void EditorSystem::endDockingToolbar()
    {
        ImGui::End();
    }

    void EditorSystem::moveLights(float dt)
    {    
        static float acc = 0.0f;
        acc += dt / 6.0f;
    
        {
            auto view = m_mainScene->getEntitiesWithComponent<TransformComponent, PointLightComponent>();
            for (auto entity : view)
            {
                auto [transform, pointLight] = view.get(entity);

                glm::vec3 delta = transform.getPosition() - glm::vec3(0.0f);

                float r = 8.0f * glm::abs(glm::sin(acc));
                float currentAngle = atan2(delta.z, delta.x);

                auto position = transform.getPosition();
                position.x = r * glm::cos(currentAngle + dt);
                position.y = r * (glm::cos(2.0f * (currentAngle + dt)) * 0.5f + 0.5f) * 0.5f;
                position.z = r * glm::sin(currentAngle + dt);

                transform.setPosition(position);
            }
        }
    
        {
            auto view = m_mainScene->getEntitiesWithComponent<TransformComponent, SpotLightComponent>();
            for (auto entity : view)
            {
                auto [transform, spotLight] = view.get(entity);

                glm::quat previousOrientation = transform.getOrientation();

                transform.setRotation(0.0f, 16.667f * dt, 0.0f);
                transform.setRotation(previousOrientation * transform.getOrientation());
            }
        }
    }
}