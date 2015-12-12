#include "SimpleGame.h"
#include "VertexEngineCore.h"
#include "CoreAssetManager.h"
#include "FreeCamera.h"
#include "PointLight.h"

#include <glm\gtc\matrix_transform.hpp>
#include <imgui\imgui_impl_sdl.h>

SimpleGame::SimpleGame()
{
    scene = new Scene(new FreeCamera(60.0f, 1024, 768, 0.01f, 50000.0f));
    CoreServices::getCore()->setScene(scene);

    //Model * model = CoreAssetManager::createModel("res/models/nanosuit/nanosuit.obj");
    //model->setPosition(glm::vec3(0.0f, -1.75f, 0.0f));
    //model->setScale(glm::vec3(0.2f));
    //
    //Model * model4 = CoreAssetManager::createModel("res/models/nanosuit/nanosuit.obj");
    //model4->setPosition(glm::vec3(2.0f, -1.75f, 0.0f));
    //model4->setScale(glm::vec3(0.2f));
    //
    //Model * model5 = CoreAssetManager::createModel("res/models/nanosuit/nanosuit.obj");
    //model5->setPosition(glm::vec3(-2.0f, -1.75f, 0.0f));
    //model5->setScale(glm::vec3(0.2f));
    //
    //Model * model2 = CoreAssetManager::createModel();
    //model2->genPlane(5, 5, 15, 15);
    //model2->setTexture("res/texture/rooftiles.jpg");
    //model2->setScale(glm::vec3(5.0f));
    //
    //Model * model3 = CoreAssetManager::createModel();
    //model3->genCone(1.0, 0.5, 12, 12);
    //model3->setPosition(glm::vec3(-1.0f, 0.51f, 1.0f));
    //
    //#ifndef _DEBUG
    //    Model * knife = CoreAssetManager::createModel("res/models/damagedknife/Model/knife6.obj");
    //    knife->setPosition(glm::vec3(0.0, 0.0, 3.0f));
    //    scene->addChild(knife);
    //#endif
    ////model->genTorus();
    ////model->setTexture("res/texture/wood.jpg");
    //
    //model->addChild(model2);
    //model2->addChild(model3);
    //
    //scene->addChild(model);
    //scene->addChild(model4);
    //scene->addChild(model5);
    
    //SpotLight * sl          = new SpotLight();
    //sl->ambient             = glm::vec3(0.0f);
    //sl->diffuse             = glm::vec3(1.0f, 1.0f, 0.8f);
    //sl->position            = glm::vec3(0.0f, 5.74f, 0.0f);
    //sl->direction           = glm::vec3(0.0f, -1.0f, 0.0f);
    //sl->constantAttenuation = 1.0f;
    //sl->innerCutOffAngle    = 12.5f;
    //sl->outerCutOffAngle    = 17.5f;

    for(int i = 0; i < 10; ++i)
    {
        for(int j = 0; j < 10; ++j)
        {
            Model * m = CoreAssetManager::createModel("res/models/metal_barrel/metal_barrel.obj");
            m->setPosition(glm::vec3(0.0f + i*6, -2.0f, 0.0f + j*6));
            m->setScale(glm::vec3(0.05f));
            //m->setTexture((i*j % 2) == 0 ? "res/models/metal_barrel/diffus_black.tga" : "res/models/metal_barrel/diffus_blue.tga");
            if((i+j) % 2 == 0)
                m->setTexture("res/models/metal_barrel/diffus_black.tga");
            else
                m->setTexture("res/models/metal_barrel/diffus_blue.tga");

            m->setTexture("res/models/metal_barrel/specular.tga", Model::SPECULAR);

            scene->addChild(m);
        }
    }

    //DirectionalLight * dir = new DirectionalLight();
    //dir->setDirection(glm::vec3(1, -1.0f, 0));
    //
    ////scene->addChild(sl);
    //scene->addChild(dir);
}

SimpleGame::~SimpleGame()
{
    delete scene;
    scene = nullptr;
}

void SimpleGame::processInput()
{
    static bool vsync = true;

    if (Input::getKeyUp(Input::V))
        CoreServices::getCore()->setVSync(vsync = !vsync);

    if(Input::getKeyDown(Input::KeypadMinus))
        CoreServices::getRenderer()->getCamera()->setFieldOfView(CoreServices::getRenderer()->getCamera()->getFieldOfView() - 1.0f);

    if (Input::getKeyDown(Input::KeypadPlus))
        CoreServices::getRenderer()->getCamera()->setFieldOfView(CoreServices::getRenderer()->getCamera()->getFieldOfView() + 1.0f);
    
    if (Input::getKeyDown(Input::U))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (Input::getKeyDown(Input::I))
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (Input::getKeyDown(Input::Escape))
        CoreServices::getCore()->quitApp();
}

void SimpleGame::update(float delta)
{
}

//bool show_test_window = true;
//bool show_another_window = false;
//ImVec4 clear_color = ImColor(114, 144, 154);

void SimpleGame::onGUI()
{
    //// 1. Show a simple window
    //// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    //{
    //    static float f = 0.0f;
    //    ImGui::Text("Hello, world!");
    //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    //    ImGui::ColorEdit3("clear color", (float*)&clear_color);
    //    if (ImGui::Button("Test Window")) show_test_window ^= 1;
    //    if (ImGui::Button("Another Window")) show_another_window ^= 1;
    //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //}

    //// 2. Show another simple window, this time using an explicit Begin/End pair
    //if (show_another_window)
    //{
    //    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
    //    ImGui::Begin("Another Window", &show_another_window);
    //    ImGui::Text("Hello");
    //    ImGui::End();
    //}
}