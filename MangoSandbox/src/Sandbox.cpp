#define GLFW_INCLUDE_NONE 

#include "Sandbox.h"

using namespace mango;

Sandbox::Sandbox()
    : System("Sandbox")
{
}

void Sandbox::onInit()
{
    m_mainScene = Services::sceneManager()->createScene("Sandbox Scene");
    Services::sceneManager()->setActiveScene(m_mainScene);

    Services::renderer()->setOutputToOffscreenTexture(false);
    Services::application()->getWindow()->setVSync(false);

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

    m_camera1 = m_mainScene->createEntity("MainCamera");
    m_camera1.addComponent<CameraComponent>().camera.setPerspective(glm::radians(45.0f), Services::application()->getWindow()->getAspectRatio(), 0.1f, 1000.0f);
    m_camera1.setPosition(0, 4, 30);

    m_camera2 = m_mainScene->createEntity("MainCamera2");
    m_camera2.addComponent<CameraComponent>().camera.setPerspective(glm::radians(45.0f), Services::application()->getWindow()->getAspectRatio(), 0.1f, 1000.0f);
    m_camera2.setPosition(0, 4, -30);
    m_camera2.setOrientation({ 0, 1, 0 }, glm::radians(180.0f));

    m_freeCameraController = createRef<FreeCameraController>();

    auto font = AssetManager::createFont("Droid48", "fonts/Roboto-Regular.ttf", 48.0f);

    /*auto skybox = createRef<Skybox>("skyboxes/stormydays/",
                                      "stormydays_lf.tga",
                                      "stormydays_rt.tga",
                                      "stormydays_up.tga", 
                                      "stormydays_dn.tga", 
                                      "stormydays_ft.tga", 
                                      "stormydays_bk.tga" );*/
    auto skybox = createRef<Skybox>("skyboxes/cold/",
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

    auto cyborg = m_mainScene->createEntity();
    cyborg.addComponent<ModelRendererComponent>(cyborgModel);
    cyborg.setPosition(0.0f, 0.0f, 0.0f);
    cyborg.setScale(1.0f);

    auto zen3c = m_mainScene->createEntity();
    zen3c.addComponent<ModelRendererComponent>(zen3cModel);
    zen3c.setPosition(-3.0f, -2.3f, 0.0f);
    zen3c.setScale(0.018f);

    auto damagedHelmet = m_mainScene->createEntity();
    damagedHelmet.addComponent<ModelRendererComponent>(damagedHelmetModel);
    damagedHelmet.setPosition(3.0f, 2.5f, 0.0f);
    damagedHelmet.setScale(1.0f);

    auto sponza = m_mainScene->createEntity();
    sponza.addComponent<ModelRendererComponent>(sponzaModel);
    sponza.setPosition(-1.5f, 0.0f, 10.0f);
    sponza.setRotation(0.0f, -90.0f, 0.0f);
    sponza.setScale(6.0f);

    auto wall = m_mainScene->createEntity();
    wall.addComponent<ModelRendererComponent>(wallModel);
    wall.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, brickwallTex);
    wall.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::NORMAL, brickwallNormalTex);
    //wall.getComponent<ModelRendererComponent>()->model.getMesh().material.addTexture(Material::TextureType::DEPTH, bricks2Depth);
    wall.setRotation(90.0f, 0.0f, 0.0f);
    wall.setPosition(0, 2.0, -9);

    auto wall2 = m_mainScene->createEntity();
    wall2.addComponent<ModelRendererComponent>(wallModel);
    wall2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, bricks2);
    wall2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::NORMAL, bricks2Normal);
    wall2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DEPTH, bricks2Depth);
    wall2.setRotation(90.0f, 0.0f, 0.0f);
    wall2.setPosition(-5, 2.0, -9);

    auto grass = m_mainScene->createEntity();
    grass.addComponent<ModelRendererComponent>(wallModel);
    grass.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, grassTex);
    grass.getComponent<ModelRendererComponent>().model.getMesh().material.addFloat("alpha_cutoff", 0.1f);
    grass.setRotation(90.0f, 0.0f, 0.0f);
    grass.setPosition(-5, 1.2, 9);
    grass.setScale(0.5);

    auto window1 = m_mainScene->createEntity();
    window1.addComponent<ModelRendererComponent>(wallModel, ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window1.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, windowTex);
    window1.setRotation(90.0f, 0.0f, 0.0f);
    window1.setPosition(0, 1.2, 9);
    window1.setScale(0.51);

    auto window3 = m_mainScene->createEntity();
    window3.addComponent<ModelRendererComponent>(wallModel, ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window3.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, windowTex);
    window3.setRotation(90.0f, 0.0f, 0.0f);
    window3.setPosition(3, 1.2, 13);
    window3.setScale(0.5);

    auto window2 = m_mainScene->createEntity();
    window2.addComponent<ModelRendererComponent>(wallModel, ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, windowTex);
    window2.setRotation(90.0f, 0.0f, 0.0f);
    window2.setPosition(5, 1.2, 9);
    window2.setScale(0.5);

    auto plane1 = m_mainScene->createEntity();
    plane1.addComponent<ModelRendererComponent>(wallModel);
    plane1.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, bricks2);
    plane1.setRotation(90.0f, 0.0f, 0.0f);
    plane1.setPosition(5, 3.0, -14);

    auto plane2 = m_mainScene->createEntity();
    plane2.addComponent<ModelRendererComponent>(wallModel);
    plane2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, bricks2);
    plane2.setRotation(0.5f, 0.0f, 0.0f);
    plane2.setPosition(5, 0.5, -11.5);
    plane2.addComponent<BoxCollider3DComponent>(glm::vec3(2.5f, 0.1f, 2.5f), glm::vec3(0.0f, -0.1f, 0.0f));
    plane2.addComponent<RigidBody3DComponent>();

    auto sphere1 = m_mainScene->createEntity();
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
    
    auto sphere2 = m_mainScene->createEntity();
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
            auto object = m_mainScene->createEntity();
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
    auto dirLight = m_mainScene->createEntity();
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
        auto pointLight = m_mainScene->createEntity();
        pointLight.addComponent<PointLightComponent>(true);
        pointLight.getComponent<PointLightComponent>().setAttenuation(3.0f, 4.0f, 10.0f);
        pointLight.getComponent<PointLightComponent>().color = colors[i];
        pointLight.getComponent<PointLightComponent>().intensity = 200.0f;
        pointLight.setPosition(positions[i].x, positions[i].y, positions[i].z);

        //if (i == 0)
        //    pointLight.getComponent<TransformComponent>()->addChild(nanobot.getComponent<TransformComponent>());
    }

    auto spotLight = m_mainScene->createEntity();
    spotLight.addComponent<SpotLightComponent>();
    spotLight.getComponent<SpotLightComponent>().color = glm::vec3(255, 206, 250) / 255.0f;
    spotLight.getComponent<SpotLightComponent>().intensity = 1000.0f;
    spotLight.getComponent<SpotLightComponent>().setCutOffAngle(30.0f);
    spotLight.setPosition(1.5, 5, 1.5);
    spotLight.setRotation(-45, 45, 45);
    spotLight.getComponent<SpotLightComponent>().setCastsShadows(true);
}

void Sandbox::onDestroy()
{
}

void Sandbox::onUpdate(float dt)
{
    static bool isDebugRender = false;
    
    if (Input::getKeyUp(KeyCode::Escape) || Input::getGamepadButtonDown(GamepadID::PAD_1, GamepadButton::BACK))
    {
        Services::application()->close();
    }

    if (Input::getKeyUp(KeyCode::H) || Input::getGamepadButtonDown(GamepadID::PAD_1, GamepadButton::Y))
    {
        isDebugRender = !isDebugRender;
        RenderingSystem::DEBUG_RENDERING = isDebugRender;
    }

    static bool fullscreen = Services::application()->getWindow()->isFullscreen();
    if (Input::getKeyUp(KeyCode::F11))
    {
        fullscreen = !fullscreen;
        Services::application()->getWindow()->setFullscreen(fullscreen);
    }

    static bool shouldMoveLights = false;
    if (Input::getKeyUp(KeyCode::Space) || Input::getGamepadButtonDown(GamepadID::PAD_1, GamepadButton::X))
    {
        shouldMoveLights = !shouldMoveLights;
    }

    if (shouldMoveLights)
    {
        moveLights(dt);
    }

    static bool isCamera1Primary = true;
    if (Input::getKeyUp(KeyCode::P) || Input::getGamepadButtonDown(GamepadID::PAD_1, GamepadButton::RIGHT_BUMPER))
    {
        isCamera1Primary = !isCamera1Primary;
        
        if (isCamera1Primary)
        {
            m_camera1.getComponent<CameraComponent>().isPrimary = true;
            m_camera2.getComponent<CameraComponent>().isPrimary = false;
        }
        else
        {
            m_camera1.getComponent<CameraComponent>().isPrimary = false;
            m_camera2.getComponent<CameraComponent>().isPrimary = true;
        }
    }

    m_freeCameraController->onUpdate(dt);
}

void Sandbox::onGui()
{
    ImGui::Begin("CVars Editor");
        CVarSystem::get()->drawImguiEditor();
    ImGui::End();

    /* Overlay start */
    const  float DISTANCE = 10.0f;
    static int   corner   = 0;

    ImVec2 windowPos      = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
    ImVec2 windowPosPivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);

    if (corner != -1)
    {
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
        ImGui::SetNextWindowSize({ 250, 0 });
    }

    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
    if (ImGui::Begin("Perf info", 0, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::Text("Performance info\n");
        ImGui::Separator();
        ImGui::Text("%.1f FPS (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    }
    ImGui::End();
    /* Overlay end */

    ImGuiSystem::beginHUD();

    //ImGuiSystem::circleFilled({ ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f}, 2.0f, glm::vec4(0.0, 1.0, 0.0, 1.0));
    //auto pos = ImGuiSystem::text(AssetManager::getFont("Droid48"), "Hello ImGUI Text Demo!", { ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f + 100.0f}, 48.0f, glm::vec4(1.0, 0.0, 0.0, 1.0), true, true);
    //ImGuiSystem::text(AssetManager::getFont("Droid48"), "Hello ImGUI Text Demo2!", { ImGui::GetIO().DisplaySize.x / 2.0f, pos}, 48.0f, glm::vec4(1.0, 0.0, 0.0, 1.0), true, false);
    auto window = Services::application()->getWindow();
    ImGuiSystem::image(AssetManager::getTexture2D("textures/opengl.png"), { window->getWidth() - 200, 0 }, { window->getWidth(), 100 }, { 1.0f, 1.0f, 1.0f, 0.5f });

    ImGuiSystem::endHUD();
}

void Sandbox::moveLights(float dt)
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
