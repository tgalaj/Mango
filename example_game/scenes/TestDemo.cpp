#include "TestDemo.h"
#include "core_engine/CoreAssetManager.h"
#include "core_components/CameraComponent.h"
#include "framework/window/Window.h"
#include "core_components/FreeLookComponent.h"
#include "core_components/FreeMoveComponent.h"
#include "core_components/ModelRendererComponent.h"
#include "core_components/PointLightComponent.h"
#include "core_components/DirectionalLightComponent.h"
#include "core_components/TransformComponent.h"
#include "core_components/SpotLightComponent.h"
#include "framework/window/Input.h"
#include "core_engine/CoreServices.h"
#include "imgui.h"
#include "core_engine/GameObject.h"
#include "systems/MoveSystem.h"

TestDemo::TestDemo()
{
}

TestDemo::~TestDemo()
{

}

void TestDemo::init()
{
    auto camera = Vertex::CoreAssetManager::createGameObject();
    camera.addComponent<Vertex::CameraComponent>(45.0f, Vertex::Window::getAspectRatio(), 0.001f, 500.0f);
    camera.addComponent<Vertex::FreeLookComponent>();
    camera.addComponent<Vertex::FreeMoveComponent>();
    camera.setPosition(0, 4, 30);

    /*auto skybox = std::make_shared<Vertex::Skybox>("res/skyboxes/stormydays/",
                                                   "stormydays_lf.tga",
                                                   "stormydays_rt.tga",
                                                   "stormydays_up.tga", 
                                                   "stormydays_dn.tga", 
                                                   "stormydays_ft.tga", 
                                                   "stormydays_bk.tga" );*/
    auto skybox = std::make_shared<Vertex::Skybox>("res/skyboxes/cold/",
                                                   "right.jpg",
                                                   "left.jpg",
                                                   "top.jpg", 
                                                   "bottom.jpg", 
                                                   "front.jpg", 
                                                   "back.jpg" );
    Vertex::CoreServices::getRenderer()->setSkybox(skybox);

    auto sphere_model = Vertex::CoreAssetManager::createModel();
    sphere_model.genSphere(0.5f, 24);

    auto cyborg_model = Vertex::CoreAssetManager::createModel("res/models/cyborg/cyborg.obj");
    auto damaged_helmet_model = Vertex::CoreAssetManager::createModel("res/models/damaged_helmet/DamagedHelmet.gltf");
    auto sponza_model = Vertex::CoreAssetManager::createModel("res/models/sponza/Sponza.gltf");

    auto wall_model = Vertex::CoreAssetManager::createModel();
    wall_model.genPlane(5, 5, 1, 1);

    auto ground_tex           = Vertex::CoreAssetManager::createTexture2D("res/textures/trak_tile_g.jpg", true);
    auto brickwall_tex        = Vertex::CoreAssetManager::createTexture2D("res/textures/brickwall.jpg", true);
    auto brickwall_normal_tex = Vertex::CoreAssetManager::createTexture2D("res/textures/brickwall_normal.jpg");
    auto bricks2              = Vertex::CoreAssetManager::createTexture2D("res/textures/bricks2.jpg", true);
    auto bricks2_depth        = Vertex::CoreAssetManager::createTexture2D("res/textures/bricks2_disp.jpg");
    auto bricks2_normal       = Vertex::CoreAssetManager::createTexture2D("res/textures/bricks2_normal.jpg");

    auto cyborg = Vertex::CoreAssetManager::createGameObject();
    cyborg.addComponent<Vertex::ModelRendererComponent>(cyborg_model);
    cyborg.setPosition(0.0f, 0.0f, 0.0f);
    cyborg.setScale(1.0f);

    auto damaged_helmet = Vertex::CoreAssetManager::createGameObject();
    damaged_helmet.addComponent<Vertex::ModelRendererComponent>(damaged_helmet_model);
    damaged_helmet.setPosition(3.0f, 2.5f, 0.0f);
    damaged_helmet.setScale(1.0f);

    auto sponza = Vertex::CoreAssetManager::createGameObject();
    sponza.addComponent<Vertex::ModelRendererComponent>(sponza_model);
    sponza.setPosition(-1.5f, 0.0f, 10.0f);
    sponza.setOrientation(0.0f, -90.0f, 0.0f);
    sponza.setScale(6.0f);

    auto wall = Vertex::CoreAssetManager::createGameObject();
    wall.addComponent<Vertex::ModelRendererComponent>(wall_model);
    wall.getComponent<Vertex::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(Vertex::Material::TextureType::DIFFUSE, brickwall_tex);
    wall.getComponent<Vertex::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(Vertex::Material::TextureType::NORMAL, brickwall_normal_tex);
    //wall.getComponent<Vertex::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(Vertex::Material::TextureType::DEPTH, bricks2_depth);
    wall.setOrientation(90.0f, 0.0f, 0.0f);
    wall.setPosition(0, 2.0, -9);

    auto wall2 = Vertex::CoreAssetManager::createGameObject();
    wall2.addComponent<Vertex::ModelRendererComponent>(wall_model);
    wall2.getComponent<Vertex::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(Vertex::Material::TextureType::DIFFUSE, bricks2);
    wall2.getComponent<Vertex::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(Vertex::Material::TextureType::NORMAL, bricks2_normal);
    wall2.getComponent<Vertex::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(Vertex::Material::TextureType::DEPTH, bricks2_depth);
    wall2.setOrientation(90.0f, 0.0f, 0.0f);
    wall2.setPosition(-5, 2.0, -9);

    auto plane1 = Vertex::CoreAssetManager::createGameObject();
    plane1.addComponent<Vertex::ModelRendererComponent>(wall_model);
    plane1.getComponent<Vertex::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(Vertex::Material::TextureType::DIFFUSE, bricks2);
    plane1.setOrientation(90.0f, 0.0f, 0.0f);
    plane1.setPosition(5, 3.0, -14);

    auto plane2 = Vertex::CoreAssetManager::createGameObject();
    plane2.addComponent<Vertex::ModelRendererComponent>(wall_model);
    plane2.getComponent<Vertex::ModelRendererComponent>()->m_model.getMesh().m_material.addTexture(Vertex::Material::TextureType::DIFFUSE, bricks2);
    plane2.setOrientation(0.0f, 0.0f, 0.0f);
    plane2.setPosition(5, 0.5, -11.5);

    auto sphere1 = Vertex::CoreAssetManager::createGameObject();
    sphere1.addComponent<Vertex::ModelRendererComponent>(sphere_model);
    sphere1.setPosition(5, 2, -11.5);
    sphere1.setScale(0.5f);

    auto sphere2 = Vertex::CoreAssetManager::createGameObject();
    sphere2.addComponent<Vertex::ModelRendererComponent>(sphere_model);
    sphere2.setPosition(5, 3, -12.5);
    sphere2.setScale(0.5f);

    auto point_light1 = Vertex::CoreAssetManager::createGameObject();
    point_light1.addComponent<Vertex::PointLightComponent>();
    point_light1.getComponent<Vertex::PointLightComponent>()->m_color = glm::vec3(1.0f);
    point_light1.getComponent<Vertex::PointLightComponent>()->m_intensity = 10.0f;
    point_light1.setPosition(-10, 15, -6.5);
    
    auto plight = Vertex::CoreAssetManager::createGameObject();
    plight.addComponent<Vertex::ModelRendererComponent>(sphere_model);
    plight.setPosition(-10, 15, -6.5);
    plight.setScale(0.25f);

    float offset = 15.0f / 10.0f;
    float num_objects = 10;

    for(int i = 0; i < num_objects; ++i)
    {
        float pos_x = 15.0f / -2.0f + (i * offset);

        for(int j = 0; j < num_objects; ++j)
        {
            auto object = Vertex::CoreAssetManager::createGameObject();
            object.addComponent<Vertex::ModelRendererComponent>(sphere_model);
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
    auto dir_light = Vertex::CoreAssetManager::createGameObject();
    dir_light.addComponent<Vertex::DirectionalLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 70.0f);
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
        auto point_light = Vertex::CoreAssetManager::createGameObject();
        point_light.addComponent<Vertex::PointLightComponent>();
        point_light.getComponent<Vertex::PointLightComponent>()->m_color = colors[i];
        point_light.getComponent<Vertex::PointLightComponent>()->m_intensity = 50.0f;
        point_light.setPosition(positions[i].x, positions[i].y, positions[i].z);
        point_light.addComponent<MoveSystemComponent>();

        //if (i == 0)
        //    point_light.getComponent<Vertex::TransformComponent>()->addChild(nanobot.getComponent<Vertex::TransformComponent>());
    }

    auto spot_light = Vertex::CoreAssetManager::createGameObject();
    spot_light.addComponent<Vertex::SpotLightComponent>();
    spot_light.getComponent<Vertex::SpotLightComponent>()->m_color = glm::vec3(255, 206, 250) / 255.0f;
    spot_light.getComponent<Vertex::SpotLightComponent>()->m_intensity = 1000;
    spot_light.getComponent<Vertex::SpotLightComponent>()->setCutOffAngle(30.0f);
    spot_light.setPosition(1.5, 5, 1.5);
    spot_light.setOrientation(-90, 0, 0);
}

void TestDemo::input(float delta)
{
    static bool is_debug_render = false;

    if (Vertex::Input::getKeyUp(Vertex::KeyCode::Escape))
    {
        Vertex::CoreServices::getCore()->stop();
    }

    if (Vertex::Input::getKeyUp(Vertex::KeyCode::H))
    {
        is_debug_render = !is_debug_render;
        Vertex::RenderingSystem::M_DEBUG_RENDERING = is_debug_render;
    }
}

void TestDemo::onGUI(float delta)
{
    ImGui::Begin("Cloth settings");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
}
