#define GLFW_INCLUDE_NONE 

#include "TestDemo.h"

using namespace mango;

TestDemo::TestDemo()
    : System("Sandbox")
{
}

void TestDemo::onInit()
{
    m_mainScene = Services::sceneManager()->createScene("Test Demo Scene");
    Services::sceneManager()->setActiveScene(m_mainScene);

    Services::application()->getWindow()->setVSync(false);

    auto camera = m_mainScene->createEntity("MainCamera");
    camera.addComponent<CameraComponent>(45.0f, Services::application()->getWindow()->getAspectRatio(), 0.1f, 500.0f);
    camera.setPosition(0, 4, 30);

    m_freeCameraController = std::make_shared<FreeCameraController>(camera);

    auto font = AssetManager::createFont("Droid48", "assets/fonts/Roboto-Regular.ttf", 48.0f);

    /*auto skybox = std::make_shared<Skybox>("assets/skyboxes/stormydays/",
                                                   "stormydays_lf.tga",
                                                   "stormydays_rt.tga",
                                                   "stormydays_up.tga", 
                                                   "stormydays_dn.tga", 
                                                   "stormydays_ft.tga", 
                                                   "stormydays_bk.tga" );*/
    auto skybox = std::make_shared<Skybox>("assets/skyboxes/cold/",
                                                   "right.jpg",
                                                   "left.jpg",
                                                   "top.jpg", 
                                                   "bottom.jpg", 
                                                   "front.jpg", 
                                                   "back.jpg" );
    Services::renderer()->setSkybox(skybox);

    auto sphereModel = AssetManager::createModel();
    sphereModel.genSphere(0.5f, 24);

    auto cyborgModel        = AssetManager::createModel("assets/models/cyborg/cyborg.obj");
    auto zen3cModel         = AssetManager::createModel("assets/models/Zen3C/Zen3C.X");
    auto damagedHelmetModel = AssetManager::createModel("assets/models/damaged_helmet/DamagedHelmet.gltf");
    auto sponzaModel        = AssetManager::createModel("assets/models/sponza/Sponza.gltf");

    auto wallModel = AssetManager::createModel();
    wallModel.genQuad(5, 5);

    auto groundTex           = AssetManager::createTexture2D("assets/textures/trak_tile_g.jpg", true);
    auto brickwallTex        = AssetManager::createTexture2D("assets/textures/brickwall.jpg", true);
    auto brickwallNormalTex  = AssetManager::createTexture2D("assets/textures/brickwall_normal.jpg");
    auto bricks2             = AssetManager::createTexture2D("assets/textures/bricks2.jpg", true);
    auto bricks2Depth        = AssetManager::createTexture2D("assets/textures/bricks2_disp.jpg");
    auto bricks2Normal       = AssetManager::createTexture2D("assets/textures/bricks2_normal.jpg");
    auto windowTex           = AssetManager::createTexture2D("assets/textures/window.png");
    auto grassTex            = AssetManager::createTexture2D("assets/textures/grass.png");
    auto openglLogo          = AssetManager::createTexture2D("assets/textures/opengl.png");

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
    sponza.setOrientation(0.0f, -90.0f, 0.0f);
    sponza.setScale(6.0f);

    auto wall = m_mainScene->createEntity();
    wall.addComponent<ModelRendererComponent>(wallModel);
    wall.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, brickwallTex);
    wall.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::NORMAL, brickwallNormalTex);
    //wall.getComponent<ModelRendererComponent>()->model.getMesh().material.addTexture(Material::TextureType::DEPTH, bricks2Depth);
    wall.setOrientation(90.0f, 0.0f, 0.0f);
    wall.setPosition(0, 2.0, -9);

    auto wall2 = m_mainScene->createEntity();
    wall2.addComponent<ModelRendererComponent>(wallModel);
    wall2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, bricks2);
    wall2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::NORMAL, bricks2Normal);
    wall2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DEPTH, bricks2Depth);
    wall2.setOrientation(90.0f, 0.0f, 0.0f);
    wall2.setPosition(-5, 2.0, -9);

    auto grass = m_mainScene->createEntity();
    grass.addComponent<ModelRendererComponent>(wallModel);
    grass.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, grassTex);
    grass.getComponent<ModelRendererComponent>().model.getMesh().material.addFloat("alpha_cutoff", 0.1f);
    grass.setOrientation(90.0f, 0.0f, 0.0f);
    grass.setPosition(-5, 1.2, 9);
    grass.setScale(0.5);

    auto window1 = m_mainScene->createEntity();
    window1.addComponent<ModelRendererComponent>(wallModel, ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window1.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, windowTex);
    window1.setOrientation(90.0f, 0.0f, 0.0f);
    window1.setPosition(0, 1.2, 9);
    window1.setScale(0.51);

    auto window3 = m_mainScene->createEntity();
    window3.addComponent<ModelRendererComponent>(wallModel, ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window3.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, windowTex);
    window3.setOrientation(90.0f, 0.0f, 0.0f);
    window3.setPosition(3, 1.2, 13);
    window3.setScale(0.5);

    auto window2 = m_mainScene->createEntity();
    window2.addComponent<ModelRendererComponent>(wallModel, ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, windowTex);
    window2.setOrientation(90.0f, 0.0f, 0.0f);
    window2.setPosition(5, 1.2, 9);
    window2.setScale(0.5);

    auto plane1 = m_mainScene->createEntity();
    plane1.addComponent<ModelRendererComponent>(wallModel);
    plane1.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, bricks2);
    plane1.setOrientation(90.0f, 0.0f, 0.0f);
    plane1.setPosition(5, 3.0, -14);

    auto plane2 = m_mainScene->createEntity();
    plane2.addComponent<ModelRendererComponent>(wallModel);
    plane2.getComponent<ModelRendererComponent>().model.getMesh().material.addTexture(Material::TextureType::DIFFUSE, bricks2);
    plane2.setOrientation(0.0f, 0.0f, 0.0f);
    plane2.setPosition(5, 0.5, -11.5);

    auto sphere1 = m_mainScene->createEntity();
    sphere1.addComponent<ModelRendererComponent>(sphereModel, ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_STATIC);
    sphere1.setPosition(5, 2, -11.5);
    sphere1.setScale(0.5f);

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
    dirLight.addComponent<DirectionalLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 4.0f, 200.0f);
    dirLight.setOrientation(-45.0f, 180.0f, 0.0f);

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
        pointLight.addComponent<PointLightComponent>();
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
    spotLight.getComponent<SpotLightComponent>().intensity = 1000;
    spotLight.getComponent<SpotLightComponent>().setCutOffAngle(30.0f);
    spotLight.setPosition(1.5, 5, 1.5);
    spotLight.setOrientation(-135, -45, 0);
    spotLight.getComponent<SpotLightComponent>().setCastsShadows(true);
}

void TestDemo::onDestroy()
{
}

void TestDemo::onUpdate(float dt)
{
    static bool isDebugRender = false;

    if (Input::getKeyUp(KeyCode::Escape))
    {
        Services::application()->stop();
    }

    if (Input::getKeyUp(KeyCode::H))
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

    m_freeCameraController->onUpdate(dt);
}

void TestDemo::onGui()
{
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

//    GUI::circleFilled({ Window::getWidth() / 2.0f, Window::getHeight() / 2.0f}, 2.0f, glm::vec4(0.0, 1.0, 0.0, 1.0));
//    auto pos = GUI::text(AssetManager::getFont("Droid48"), "Hello ImGUI Text Demo!", { Window::getWidth() / 2.0f, Window::getHeight() / 2.0f + 100.0f}, 48.0f, glm::vec4(1.0, 0.0, 0.0, 1.0), true, true);
//    GUI::text(AssetManager::getFont("Droid48"), "Hello ImGUI Text Demo2!", { Window::getWidth() / 2.0f, pos}, 48.0f, glm::vec4(1.0, 0.0, 0.0, 1.0), true, false);
    auto window = Services::application()->getWindow();
    ImGuiSystem::image(AssetManager::getTexture2D("assets/textures/opengl.png"), { window->getWidth() - 200, 0 }, { window->getWidth(), 100 }, { 1.0f, 1.0f, 1.0f, 0.5f });

    ImGuiSystem::endHUD();
}
