#include "ParticlesDemo.h"
#include "CoreServices.h"
#include "FreeCamera.h"
#include "VertexEngineCore.h"
#include "DirectionalLight.h"

#include <glm\gtc\constants.hpp>
#include <glm\gtc\random.hpp>


ParticlesDemo::ParticlesDemo()
{
    cam = new FreeCamera(60.0f, 1024, 768, 0.1f, 1000.0f);
    cam->setPosition(6.84f, 8.12f, 18.93f);
    cam->setYaw(-109.2f);
    cam->setPitch(-24.8f);

    scene = new Scene(cam);
    CoreServices::getCore()->setScene(scene);

    Model * sphere = CoreAssetManager::createModel();
    sphere->genSphere(0.1f, 10);
    sphere->setTexture("res/texture/white_4x4.jpg");
    sphere->setDiffuseColor(0, glm::vec3(1, 0, 0));
    sphere->setPosition(glm::vec3(10, 0, 0));

    Model * sphere2 = CoreAssetManager::createModel();
    sphere2->genSphere(0.1f, 10);
    sphere2->setTexture("res/texture/white_4x4.jpg");
    sphere2->setDiffuseColor(0, glm::vec3(1, 0, 0));
    sphere2->setPosition(glm::vec3(-10, 0, 0));

    Model * sphere3 = CoreAssetManager::createModel();
    sphere3->genSphere(0.1f, 10);
    sphere3->setTexture("res/texture/white_4x4.jpg");
    sphere3->setDiffuseColor(0, glm::vec3(1, 0, 0));
    sphere3->setPosition(glm::vec3(0, 15, 0));

    Model * sphere4 = CoreAssetManager::createModel();
    sphere4->genSphere(0.1f, 10);
    sphere4->setTexture("res/texture/white_4x4.jpg");
    sphere4->setDiffuseColor(0, glm::vec3(1, 0, 0));
    sphere4->setPosition(glm::vec3(0, -15, 0));

    DirectionalLight * dirLight = new DirectionalLight();
    dirLight->setDirection(glm::vec3(0, -1, 1));

    particles = new ParticleEffect(8000000);

    scene->addChild(sphere);
    scene->addChild(sphere2);
    scene->addChild(sphere3);
    scene->addChild(sphere4);
    scene->addChild(dirLight);
    scene->addChild(particles);

    /* Skybox */
    CoreServices::getRenderer()->createSkybox("res/skyboxes/stormydays/stormydays_rt.tga",
                                              "res/skyboxes/stormydays/stormydays_lf.tga",
                                              "res/skyboxes/stormydays/stormydays_dn.tga",
                                              "res/skyboxes/stormydays/stormydays_up.tga",
                                              "res/skyboxes/stormydays/stormydays_ft.tga",
                                              "res/skyboxes/stormydays/stormydays_bk.tga");
}


ParticlesDemo::~ParticlesDemo()
{
    delete scene;
    scene = nullptr;
}

void ParticlesDemo::processInput()
{
    if (Input::getKeyDown(Input::Escape))
    {
        CoreServices::getCore()->quitApp();
    }

    if (Input::getKeyUp(Input::V))
    {
        CoreServices::getCore()->setVSync(true);
    }

    if (Input::getKeyUp(Input::B))
    {
        CoreServices::getCore()->setVSync(false);
    }

    if (Input::getKeyUp(Input::P))
    {
        particles->simulate = !particles->simulate;
    }

    if (Input::getKeyUp(Input::R))
    {
        particles->reset();
    }

    if (Input::getKeyUp(Input::C))
    {
        printf("========= Camera info =========\n"
               "| Position = %.2f, %.2f, %.2f\n"
               "| Yaw      = %.2f\n"
               "| Pitch    = %.2f\n"
               "| Up       = %.2f, %.2f, %.2f\n\n",
               cam->getPosition().x, cam->getPosition().y, cam->getPosition().z,
               cam->getYaw(), cam->getPitch(),
               cam->getUpVector().x, cam->getUpVector().y, cam->getUpVector().z);
    }
}

void ParticlesDemo::update(float delta)
{
    printf("FPS = %d || MS =  %.3f     \r", CoreServices::getCore()->getFPS(), 1000.0f / CoreServices::getCore()->getFPS());
}

void ParticlesDemo::onGUI()
{
}
