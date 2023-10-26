#define GLFW_INCLUDE_NONE 

#include "TestDemo.h"
#include "imgui.h"
#include "Systems/MoveSystem.h"

#include <Mango/Components/CameraComponent.h>
#include <Mango/Components/DirectionalLightComponent.h>
#include <Mango/Components/FreeLookComponent.h>
#include <Mango/Components/FreeMoveComponent.h>
#include <Mango/Components/ModelRendererComponent.h>
#include <Mango/Components/PointLightComponent.h>
#include <Mango/Components/SpotLightComponent.h>
#include <Mango/Components/TransformComponent.h>
#include <Mango/Core/CoreAssetManager.h>
#include <Mango/Core/CoreServices.h>
#include <Mango/Core/GameObject.h>
#include <Mango/GUI/GUI.h>

TestDemo::TestDemo()
{
}

TestDemo::~TestDemo()
{

}

void TestDemo::init()
{
    auto camera = mango::CoreAssetManager::createGameObject();
    camera.addComponent<mango::CameraComponent>(45.0f, mango::Window::getAspectRatio(), 0.1f, 500.0f);
    camera.addComponent<mango::FreeLookComponent>();
    camera.addComponent<mango::FreeMoveComponent>();
    camera.setPosition(0, 4, 30);

    auto font = mango::CoreAssetManager::createFont("Droid48", "assets/fonts/Roboto-Regular.ttf", 48.0f);

    /*auto skybox = std::make_shared<mango::Skybox>("assets/skyboxes/stormydays/",
                                                   "stormydays_lf.tga",
                                                   "stormydays_rt.tga",
                                                   "stormydays_up.tga", 
                                                   "stormydays_dn.tga", 
                                                   "stormydays_ft.tga", 
                                                   "stormydays_bk.tga" );*/
    auto skybox = std::make_shared<mango::Skybox>("assets/skyboxes/cold/",
                                                   "right.jpg",
                                                   "left.jpg",
                                                   "top.jpg", 
                                                   "bottom.jpg", 
                                                   "front.jpg", 
                                                   "back.jpg" );
    mango::CoreServices::getRenderer()->setSkybox(skybox);

    auto sphereModel = mango::CoreAssetManager::createModel();
    sphereModel.genSphere(0.5f, 24);

    auto cyborgModel        = mango::CoreAssetManager::createModel("assets/models/cyborg/cyborg.obj");
    auto zen3cModel         = mango::CoreAssetManager::createModel("assets/models/Zen3C/Zen3C.X");
    auto damagedHelmetModel = mango::CoreAssetManager::createModel("assets/models/damaged_helmet/DamagedHelmet.gltf");
    auto sponzaModel        = mango::CoreAssetManager::createModel("assets/models/sponza/Sponza.gltf");

    auto wallModel = mango::CoreAssetManager::createModel();
    wallModel.genQuad(5, 5);

    auto groundTex           = mango::CoreAssetManager::createTexture2D("assets/textures/trak_tile_g.jpg", true);
    auto brickwallTex        = mango::CoreAssetManager::createTexture2D("assets/textures/brickwall.jpg", true);
    auto brickwallNormalTex  = mango::CoreAssetManager::createTexture2D("assets/textures/brickwall_normal.jpg");
    auto bricks2             = mango::CoreAssetManager::createTexture2D("assets/textures/bricks2.jpg", true);
    auto bricks2Depth        = mango::CoreAssetManager::createTexture2D("assets/textures/bricks2_disp.jpg");
    auto bricks2Normal       = mango::CoreAssetManager::createTexture2D("assets/textures/bricks2_normal.jpg");
    auto windowTex           = mango::CoreAssetManager::createTexture2D("assets/textures/window.png");
    auto grassTex            = mango::CoreAssetManager::createTexture2D("assets/textures/grass.png");
    auto openglLogo          = mango::CoreAssetManager::createTexture2D("assets/textures/opengl.png");

    auto cyborg = mango::CoreAssetManager::createGameObject();
    cyborg.addComponent<mango::ModelRendererComponent>(cyborgModel);
    cyborg.setPosition(0.0f, 0.0f, 0.0f);
    cyborg.setScale(1.0f);

    auto zen3c = mango::CoreAssetManager::createGameObject();
    zen3c.addComponent<mango::ModelRendererComponent>(zen3cModel);
    zen3c.setPosition(-3.0f, -2.3f, 0.0f);
    zen3c.setScale(0.018f);

    auto damagedHelmet = mango::CoreAssetManager::createGameObject();
    damagedHelmet.addComponent<mango::ModelRendererComponent>(damagedHelmetModel);
    damagedHelmet.setPosition(3.0f, 2.5f, 0.0f);
    damagedHelmet.setScale(1.0f);

    auto sponza = mango::CoreAssetManager::createGameObject();
    sponza.addComponent<mango::ModelRendererComponent>(sponzaModel);
    sponza.setPosition(-1.5f, 0.0f, 10.0f);
    sponza.setOrientation(0.0f, -90.0f, 0.0f);
    sponza.setScale(6.0f);

    auto wall = mango::CoreAssetManager::createGameObject();
    wall.addComponent<mango::ModelRendererComponent>(wallModel);
    wall.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DIFFUSE, brickwallTex);
    wall.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::NORMAL, brickwallNormalTex);
    //wall.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DEPTH, bricks2Depth);
    wall.setOrientation(90.0f, 0.0f, 0.0f);
    wall.setPosition(0, 2.0, -9);

    auto wall2 = mango::CoreAssetManager::createGameObject();
    wall2.addComponent<mango::ModelRendererComponent>(wallModel);
    wall2.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DIFFUSE, bricks2);
    wall2.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::NORMAL, bricks2Normal);
    wall2.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DEPTH, bricks2Depth);
    wall2.setOrientation(90.0f, 0.0f, 0.0f);
    wall2.setPosition(-5, 2.0, -9);

    auto grass = mango::CoreAssetManager::createGameObject();
    grass.addComponent<mango::ModelRendererComponent>(wallModel);
    grass.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DIFFUSE, grassTex);
    grass.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addFloat("alpha_cutoff", 0.1f);
    grass.setOrientation(90.0f, 0.0f, 0.0f);
    grass.setPosition(-5, 1.2, 9);
    grass.setScale(0.5);

    auto window1 = mango::CoreAssetManager::createGameObject();
    window1.addComponent<mango::ModelRendererComponent>(wallModel, mango::ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window1.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DIFFUSE, windowTex);
    window1.setOrientation(90.0f, 0.0f, 0.0f);
    window1.setPosition(0, 1.2, 9);
    window1.setScale(0.51);

    auto window3 = mango::CoreAssetManager::createGameObject();
    window3.addComponent<mango::ModelRendererComponent>(wallModel, mango::ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window3.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DIFFUSE, windowTex);
    window3.setOrientation(90.0f, 0.0f, 0.0f);
    window3.setPosition(3, 1.2, 13);
    window3.setScale(0.5);

    auto window2 = mango::CoreAssetManager::createGameObject();
    window2.addComponent<mango::ModelRendererComponent>(wallModel, mango::ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window2.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DIFFUSE, windowTex);
    window2.setOrientation(90.0f, 0.0f, 0.0f);
    window2.setPosition(5, 1.2, 9);
    window2.setScale(0.5);

    auto plane1 = mango::CoreAssetManager::createGameObject();
    plane1.addComponent<mango::ModelRendererComponent>(wallModel);
    plane1.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DIFFUSE, bricks2);
    plane1.setOrientation(90.0f, 0.0f, 0.0f);
    plane1.setPosition(5, 3.0, -14);

    auto plane2 = mango::CoreAssetManager::createGameObject();
    plane2.addComponent<mango::ModelRendererComponent>(wallModel);
    plane2.getComponent<mango::ModelRendererComponent>()->model.getMesh().material.addTexture(mango::Material::TextureType::DIFFUSE, bricks2);
    plane2.setOrientation(0.0f, 0.0f, 0.0f);
    plane2.setPosition(5, 0.5, -11.5);

    auto sphere1 = mango::CoreAssetManager::createGameObject();
    sphere1.addComponent<mango::ModelRendererComponent>(sphereModel, mango::ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_STATIC);
    sphere1.setPosition(5, 2, -11.5);
    sphere1.setScale(0.5f);

    auto sphere2 = mango::CoreAssetManager::createGameObject();
    sphere2.addComponent<mango::ModelRendererComponent>(sphereModel);
    sphere2.setPosition(5, 3, -12.5);
    sphere2.setScale(0.5f);

    float offset = 15.0f / 10.0f;
    float numObjects = 10;

    for(int i = 0; i < numObjects; ++i)
    {
        float xPos = 15.0f / -2.0f + (i * offset);

        for(int j = 0; j < numObjects; ++j)
        {
            auto object = mango::CoreAssetManager::createGameObject();
            object.addComponent<mango::ModelRendererComponent>(sphereModel);
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
    auto dirLight = mango::CoreAssetManager::createGameObject();
    dirLight.addComponent<mango::DirectionalLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 4.0f, 200.0f);
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
        auto pointLight = mango::CoreAssetManager::createGameObject();
        pointLight.addComponent<mango::PointLightComponent>();
        pointLight.getComponent<mango::PointLightComponent>()->setAttenuation(3.0f, 4.0f, 10.0f);
        pointLight.getComponent<mango::PointLightComponent>()->color = colors[i];
        pointLight.getComponent<mango::PointLightComponent>()->intensity = 200.0f;
        pointLight.setPosition(positions[i].x, positions[i].y, positions[i].z);
        pointLight.addComponent<MoveSystemComponent>();

        //if (i == 0)
        //    pointLight.getComponent<mango::TransformComponent>()->addChild(nanobot.getComponent<mango::TransformComponent>());
    }

    auto spotLight = mango::CoreAssetManager::createGameObject();
    spotLight.addComponent<mango::SpotLightComponent>();
    spotLight.getComponent<mango::SpotLightComponent>()->color = glm::vec3(255, 206, 250) / 255.0f;
    spotLight.getComponent<mango::SpotLightComponent>()->intensity = 1000;
    spotLight.getComponent<mango::SpotLightComponent>()->setCutOffAngle(30.0f);
    spotLight.setPosition(1.5, 5, 1.5);
    spotLight.setOrientation(-135, -45, 0);
    spotLight.getComponent<mango::SpotLightComponent>()->setCastsShadows(true);
    spotLight.addComponent<MoveSystemComponent>();
}

void TestDemo::input(float delta)
{
    static bool isDebugRender = false;

    if (mango::Input::getKeyUp(mango::KeyCode::Escape))
    {
        mango::CoreServices::getApplication()->stop();
    }

    if (mango::Input::getKeyUp(mango::KeyCode::H))
    {
        isDebugRender = !isDebugRender;
        mango::RenderingSystem::DEBUG_RENDERING = isDebugRender;
    }

    static bool fullscreen = mango::Window::isFullscreen();
    if (mango::Input::getKeyUp(mango::KeyCode::F11))
    {
        fullscreen = !fullscreen;
        mango::Window::setFullscreen(fullscreen);
    }
}

void TestDemo::onGUI(float delta)
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

    mango::GUI::beginHUD();

//    mango::GUI::circleFilled({ mango::Window::getWidth() / 2.0f, mango::Window::getHeight() / 2.0f}, 2.0f, glm::vec4(0.0, 1.0, 0.0, 1.0));
//    auto pos = mango::GUI::text(mango::CoreAssetManager::getFont("Droid48"), "Hello ImGUI Text Demo!", { mango::Window::getWidth() / 2.0f, mango::Window::getHeight() / 2.0f + 100.0f}, 48.0f, glm::vec4(1.0, 0.0, 0.0, 1.0), true, true);
//    mango::GUI::text(mango::CoreAssetManager::getFont("Droid48"), "Hello ImGUI Text Demo2!", { mango::Window::getWidth() / 2.0f, pos}, 48.0f, glm::vec4(1.0, 0.0, 0.0, 1.0), true, false);
    mango::GUI::image(mango::CoreAssetManager::getTexture2D("assets/textures/opengl.png"), { mango::Window::getWidth() - 200, 0 }, { mango::Window::getWidth(), 100 }, { 1.0f, 1.0f, 1.0f, 0.5f });

    mango::GUI::endHUD();
}

//TODO: update function!!
