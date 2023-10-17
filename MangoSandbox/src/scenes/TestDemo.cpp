#include "TestDemo.h"
#include "core_engine/CoreAssetManager.h"
#include "core_components/CameraComponent.h"
#include "window/Window.h"
#include "core_components/FreeLookComponent.h"
#include "core_components/FreeMoveComponent.h"
#include "core_components/ModelRendererComponent.h"
#include "core_components/PointLightComponent.h"
#include "core_components/DirectionalLightComponent.h"
#include "core_components/TransformComponent.h"
#include "core_components/SpotLightComponent.h"
#include "window/Input.h"
#include "core_engine/CoreServices.h"
#include "imgui.h"
#include "core_engine/GameObject.h"
#include "systems/MoveSystem.h"
#include "gui/GUI.h"

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

    auto sphere_model = mango::CoreAssetManager::createModel();
    sphere_model.genSphere(0.5f, 24);

    auto cyborg_model = mango::CoreAssetManager::createModel("assets/models/cyborg/cyborg.obj");
    auto zen3c_model = mango::CoreAssetManager::createModel("assets/models/Zen3C/Zen3C.X");
    auto damaged_helmet_model = mango::CoreAssetManager::createModel("assets/models/damaged_helmet/DamagedHelmet.gltf");
    auto sponza_model = mango::CoreAssetManager::createModel("assets/models/sponza/Sponza.gltf");

    auto wall_model = mango::CoreAssetManager::createModel();
    wall_model.genQuad(5, 5);

    auto ground_tex           = mango::CoreAssetManager::createTexture2D("assets/textures/trak_tile_g.jpg", true);
    auto brickwall_tex        = mango::CoreAssetManager::createTexture2D("assets/textures/brickwall.jpg", true);
    auto brickwall_normal_tex = mango::CoreAssetManager::createTexture2D("assets/textures/brickwall_normal.jpg");
    auto bricks2              = mango::CoreAssetManager::createTexture2D("assets/textures/bricks2.jpg", true);
    auto bricks2_depth        = mango::CoreAssetManager::createTexture2D("assets/textures/bricks2_disp.jpg");
    auto bricks2_normal       = mango::CoreAssetManager::createTexture2D("assets/textures/bricks2_normal.jpg");
    auto window_tex           = mango::CoreAssetManager::createTexture2D("assets/textures/window.png");
    auto grass_tex            = mango::CoreAssetManager::createTexture2D("assets/textures/grass.png");
    auto opengl_logo          = mango::CoreAssetManager::createTexture2D("assets/textures/opengl.png");

    auto cyborg = mango::CoreAssetManager::createGameObject();
    cyborg.addComponent<mango::ModelRendererComponent>(cyborg_model);
    cyborg.setPosition(0.0f, 0.0f, 0.0f);
    cyborg.setScale(1.0f);

    auto zen3c = mango::CoreAssetManager::createGameObject();
    zen3c.addComponent<mango::ModelRendererComponent>(zen3c_model);
    zen3c.setPosition(-3.0f, -2.3f, 0.0f);
    zen3c.setScale(0.018f);

    auto damaged_helmet = mango::CoreAssetManager::createGameObject();
    damaged_helmet.addComponent<mango::ModelRendererComponent>(damaged_helmet_model);
    damaged_helmet.setPosition(3.0f, 2.5f, 0.0f);
    damaged_helmet.setScale(1.0f);

    auto sponza = mango::CoreAssetManager::createGameObject();
    sponza.addComponent<mango::ModelRendererComponent>(sponza_model);
    sponza.setPosition(-1.5f, 0.0f, 10.0f);
    sponza.setOrientation(0.0f, -90.0f, 0.0f);
    sponza.setScale(6.0f);

    auto wall = mango::CoreAssetManager::createGameObject();
    wall.addComponent<mango::ModelRendererComponent>(wall_model);
    wall.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DIFFUSE, brickwall_tex);
    wall.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::NORMAL, brickwall_normal_tex);
    //wall.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DEPTH, bricks2_depth);
    wall.setOrientation(90.0f, 0.0f, 0.0f);
    wall.setPosition(0, 2.0, -9);

    auto wall2 = mango::CoreAssetManager::createGameObject();
    wall2.addComponent<mango::ModelRendererComponent>(wall_model);
    wall2.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DIFFUSE, bricks2);
    wall2.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::NORMAL, bricks2_normal);
    wall2.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DEPTH, bricks2_depth);
    wall2.setOrientation(90.0f, 0.0f, 0.0f);
    wall2.setPosition(-5, 2.0, -9);

    auto grass = mango::CoreAssetManager::createGameObject();
    grass.addComponent<mango::ModelRendererComponent>(wall_model);
    grass.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DIFFUSE, grass_tex);
    grass.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addFloat("alpha_cutoff", 0.1f);
    grass.setOrientation(90.0f, 0.0f, 0.0f);
    grass.setPosition(-5, 1.2, 9);
    grass.setScale(0.5);

    auto window1 = mango::CoreAssetManager::createGameObject();
    window1.addComponent<mango::ModelRendererComponent>(wall_model, mango::ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window1.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DIFFUSE, window_tex);
    window1.setOrientation(90.0f, 0.0f, 0.0f);
    window1.setPosition(0, 1.2, 9);
    window1.setScale(0.51);

    auto window3 = mango::CoreAssetManager::createGameObject();
    window3.addComponent<mango::ModelRendererComponent>(wall_model, mango::ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window3.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DIFFUSE, window_tex);
    window3.setOrientation(90.0f, 0.0f, 0.0f);
    window3.setPosition(3, 1.2, 13);
    window3.setScale(0.5);

    auto window2 = mango::CoreAssetManager::createGameObject();
    window2.addComponent<mango::ModelRendererComponent>(wall_model, mango::ModelRendererComponent::RenderQueue::RQ_ALPHA);
    window2.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DIFFUSE, window_tex);
    window2.setOrientation(90.0f, 0.0f, 0.0f);
    window2.setPosition(5, 1.2, 9);
    window2.setScale(0.5);

    auto plane1 = mango::CoreAssetManager::createGameObject();
    plane1.addComponent<mango::ModelRendererComponent>(wall_model);
    plane1.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DIFFUSE, bricks2);
    plane1.setOrientation(90.0f, 0.0f, 0.0f);
    plane1.setPosition(5, 3.0, -14);

    auto plane2 = mango::CoreAssetManager::createGameObject();
    plane2.addComponent<mango::ModelRendererComponent>(wall_model);
    plane2.getComponent<mango::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(mango::Material::TextureType::DIFFUSE, bricks2);
    plane2.setOrientation(0.0f, 0.0f, 0.0f);
    plane2.setPosition(5, 0.5, -11.5);

    auto sphere1 = mango::CoreAssetManager::createGameObject();
    sphere1.addComponent<mango::ModelRendererComponent>(sphere_model, mango::ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_STATIC);
    sphere1.setPosition(5, 2, -11.5);
    sphere1.setScale(0.5f);

    auto sphere2 = mango::CoreAssetManager::createGameObject();
    sphere2.addComponent<mango::ModelRendererComponent>(sphere_model);
    sphere2.setPosition(5, 3, -12.5);
    sphere2.setScale(0.5f);

    float offset = 15.0f / 10.0f;
    float num_objects = 10;

    for(int i = 0; i < num_objects; ++i)
    {
        float pos_x = 15.0f / -2.0f + (i * offset);

        for(int j = 0; j < num_objects; ++j)
        {
            auto object = mango::CoreAssetManager::createGameObject();
            object.addComponent<mango::ModelRendererComponent>(sphere_model);
            object.setScale(0.75f);
            
            float pos_z = 15.0f / -2.0f + (j * offset);
            object.setPosition(pos_x + 0.5f * offset, 0.5f, pos_z + 0.5f * offset);

            if (i == 2 && j == 2)
            {
                cyborg.addChild(object);
                //object.setScale(4.0f);
            }
        }
    }

    /* Lights */
    auto dir_light = mango::CoreAssetManager::createGameObject();
    dir_light.addComponent<mango::DirectionalLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 4.0f, 200.0f);
    dir_light.setOrientation(-45.0f, 180.0f, 0.0f);

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
        auto point_light = mango::CoreAssetManager::createGameObject();
        point_light.addComponent<mango::PointLightComponent>();
        point_light.getComponent<mango::PointLightComponent>()->setAttenuation(3.0f, 4.0f, 10.0f);
        point_light.getComponent<mango::PointLightComponent>()->m_color = colors[i];
        point_light.getComponent<mango::PointLightComponent>()->m_intensity = 200.0f;
        point_light.setPosition(positions[i].x, positions[i].y, positions[i].z);
        point_light.addComponent<MoveSystemComponent>();

        //if (i == 0)
        //    point_light.getComponent<mango::TransformComponent>()->addChild(nanobot.getComponent<mango::TransformComponent>());
    }

    auto spot_light = mango::CoreAssetManager::createGameObject();
    spot_light.addComponent<mango::SpotLightComponent>();
    spot_light.getComponent<mango::SpotLightComponent>()->m_color = glm::vec3(255, 206, 250) / 255.0f;
    spot_light.getComponent<mango::SpotLightComponent>()->m_intensity = 1000;
    spot_light.getComponent<mango::SpotLightComponent>()->setCutOffAngle(30.0f);
    spot_light.setPosition(1.5, 5, 1.5);
    spot_light.setOrientation(-135, -45, 0);
    spot_light.getComponent<mango::SpotLightComponent>()->setCastsShadows(true);
    spot_light.addComponent<MoveSystemComponent>();
}

void TestDemo::input(float delta)
{
    static bool is_debug_render = false;

    if (mango::Input::getKeyUp(mango::KeyCode::Escape))
    {
        mango::CoreServices::getCore()->stop();
    }

    if (mango::Input::getKeyUp(mango::KeyCode::H))
    {
        is_debug_render = !is_debug_render;
        mango::RenderingSystem::M_DEBUG_RENDERING = is_debug_render;
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
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
    ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
    if (corner != -1)
    {
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
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
