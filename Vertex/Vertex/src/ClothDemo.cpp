#include <windows.h>

#include "Cloth.h"
#include "ClothDemo.h"
#include "CoreServices.h"
#include "FreeCamera.h"
#include "VertexEngineCore.h"
#include "DirectionalLight.h"

#include <imgui\imgui_impl_sdl.h>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\random.hpp>

void gotoxy(int x, int y)
{
    COORD p = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
}

ClothDemo::ClothDemo()
{
    cam = new FreeCamera(60.0f, 1024, 768, 0.1f, 1000.0f);
    cam->setPosition(6.84f, 8.12f, 18.93f);
    cam->setYaw(-109.2f);
    cam->setPitch(-24.8f);

    scene = new Scene(cam);
    CoreServices::getCore()->setScene(scene);

    Model * sphere = CoreAssetManager::createModel();
    sphere->genSphere(1.0f, 4);
    sphere->setTexture("res/texture/white_4x4.jpg");
    sphere->setDiffuseColor(0, glm::vec3(1, 0, 0));
    sphere->setPosition(glm::vec3(10, 0, 0));

    DirectionalLight * dirLight = new DirectionalLight();
    dirLight->setDirection(glm::vec3(-1, -1, -1));

    cloth = new Cloth(60, 60, 8.0f, 6.0f);
    cloth->setDiffuseTexture("res/texture/me_textile.tga");

    scene->addChild(sphere);
    scene->addChild(dirLight);
    scene->addChild(cloth);

    /* Skybox */
    CoreServices::getRenderer()->createSkybox("res/skyboxes/stormydays/stormydays_rt.tga",
                                              "res/skyboxes/stormydays/stormydays_lf.tga",
                                              "res/skyboxes/stormydays/stormydays_dn.tga",
                                              "res/skyboxes/stormydays/stormydays_up.tga",
                                              "res/skyboxes/stormydays/stormydays_ft.tga",
                                              "res/skyboxes/stormydays/stormydays_bk.tga");
}

ClothDemo::~ClothDemo()
{
    delete scene;
    scene = nullptr;
}

bool vsync = true;
bool isWire = false;
void ClothDemo::processInput()
{
    if (Input::getKeyDown(Input::Escape))
    {
        CoreServices::getCore()->quitApp();
    }

    if (Input::getKeyUp(Input::V))
    {
        vsync = !vsync;
        CoreServices::getCore()->setVSync(vsync);

        if (vsync)
        {
            printf("Vsync ON \r");
        }
        else
        {
            printf("Vsync OFF\r");
        }
    }

    if (Input::getKeyUp(Input::G))
    {
        isWire = !isWire;

        if(isWire)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}

void ClothDemo::update(float delta)
{

}

float cloth_shininess = 2.0f;
ImVec4 cloth_color = ImColor(200, 200, 200);

glm::vec3 trans, rot;
glm::vec3 trans_old, rot_old;

bool pins[] { 1, 1, 1, 1, 1 };

void ClothDemo::onGUI()
{
    {
        ImGui::Begin      ("Cloth settings");

        ImGui::Text       ("Material properties:");
        ImGui::SliderFloat("Shininess", &cloth_shininess, 1.0f, 200.0f);
        ImGui::ColorEdit3 ("Color", (float*)&cloth_color);

        ImGui::Text("\nPhysical properties:");
        ImGui::SliderFloat3("Gravity",       &cloth->gravity[0], -100.0f, 100.0f);
        ImGui::SliderFloat ("Particle mass", &cloth->particle_mass, 0.1f, 10.0f);
        ImGui::SliderFloat ("Spring k",      &cloth->spring_k, 0.0f, 20000.0f);
        ImGui::SliderFloat ("Damping",       &cloth->damping,  0.0f, 1.0f);

        ImGui::Text("\nStatic points:");
        ImGui::Checkbox("Pin 1", &pins[0]);
        ImGui::Checkbox("Pin 2", &pins[1]);
        ImGui::Checkbox("Pin 3", &pins[2]);
        ImGui::Checkbox("Pin 4", &pins[3]);
        ImGui::Checkbox("Pin 5", &pins[4]);

        ImGui::Text("\nTransformations:");
        ImGui::SliderFloat3("Translation", &trans[0], -100.0f, 100.0f);
        ImGui::SliderFloat3("Rotation",    &rot[0],    0.0f,   360.0f);

        if(cloth->simulate)
        {
            if (ImGui::Button("Pause simulation")) cloth->simulate ^= 1;
        }
        else
        {
            if (ImGui::Button("Start simulation")) cloth->simulate ^= 1;
        }

        if (ImGui::Button("Reset simulation")) cloth->reset();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        cloth->setShininess(cloth_shininess);
        cloth->setColor(glm::vec3(cloth_color.x, cloth_color.y, cloth_color.z));

        if(glm::notEqual(trans, trans_old).x || glm::notEqual(trans, trans_old).y || glm::notEqual(trans, trans_old).z)
        {
            cloth->setPosition(trans);
            trans_old = trans;
        }

        if(glm::notEqual(rot_old, rot).x)
        {
            cloth->setRotation(glm::vec3(1, 0, 0), rot.x);
            rot_old.x = rot.x;
        }

        if (glm::notEqual(rot_old, rot).y)
        {
            cloth->setRotation(glm::vec3(0, 1, 0), rot.y);
            rot_old.y = rot.y;
        }

        if (glm::notEqual(rot_old, rot).z)
        {
            cloth->setRotation(glm::vec3(0, 0, 1), rot.z);
            rot_old.z = rot.z;
        }

        for (int i = 0; i < sizeof(pins) / sizeof(bool); ++i)
        {
            cloth->setPin(i, pins[i]);
        }
    }
}
