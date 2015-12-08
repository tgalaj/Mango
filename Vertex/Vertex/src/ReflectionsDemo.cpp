#include "CoreServices.h"
#include "FreeCamera.h"
#include "ReflectionsDemo.h"
#include "VertexEngineCore.h"
#include "DirectionalLight.h"

#include <glm\gtc\constants.hpp>
#include <glm\gtc\random.hpp>

ReflectionsDemo::ReflectionsDemo()
{
    cam = new FreeCamera(60.0f, 1024, 768, 0.1f, 1000.0f);
    cam->setPosition    (6.84f, 8.12f, 18.93f);
    cam->setYaw         (-109.2f);
    cam->setPitch       (-24.8f);

    scene = new Scene(cam);
    CoreServices::getCore()->setScene(scene);

    sphere = CoreAssetManager::createModel();
    sphere->genSphere(4.0f, 50);
    //sphere->genCube(4);
    sphere->setTexture("res/texture/white_4x4.jpg");
    sphere->setDiffuseColor(0, glm::vec3(1, 0, 0));

    int numCubes = 10000;
    float radius = 120.0f;
    float alpha  = glm::two_pi<float>() / numCubes;

    for(int i = 0; i < numCubes; ++i)
    {
        Model * cube = CoreAssetManager::createModel();
        cube->genCube(1);
        cube->setTexture("res/texture/white_4x4.jpg");
        cube->setDiffuseColor(0, glm::ballRand(1.0f));
        //cube->setPosition(glm::vec3(radius * glm::cos(alpha * i), 0, radius * glm::sin(alpha * i)));
        cube->setPosition(glm::ballRand(radius));
        sphere->addChild(cube);
    }
    
    DirectionalLight * dirLight = new DirectionalLight();
    dirLight->setDirection(glm::vec3(0, -1, 1));
    
    scene->addChild(sphere, 1);
    scene->addChild(dirLight);

    /* Skybox */
    CoreServices::getRenderer()->createSkybox("res/skyboxes/stormydays/stormydays_rt.tga",
                                              "res/skyboxes/stormydays/stormydays_lf.tga", 
                                              "res/skyboxes/stormydays/stormydays_dn.tga", 
                                              "res/skyboxes/stormydays/stormydays_up.tga", 
                                              "res/skyboxes/stormydays/stormydays_ft.tga", 
                                              "res/skyboxes/stormydays/stormydays_bk.tga" );
}


ReflectionsDemo::~ReflectionsDemo()
{
}

void ReflectionsDemo::processInput()
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

    if (Input::getKeyUp(Input::C))
    {
        printf("========= Camera info =========\n"
               "| Position = %.2f, %.2f, %.2f\n"
               "| Yaw      = %.2f\n"
               "| Pitch    = %.2f\n"
               "| Up       = %.2f, %.2f, %.2f\n\n",
               cam->getPosition().x,  cam->getPosition().y,  cam->getPosition().z,
               cam->getYaw(), cam->getPitch(),
               cam->getUpVector().x,  cam->getUpVector().y,  cam->getUpVector().z);
    }
}

void ReflectionsDemo::update(float delta)
{
    printf("FPS =  %d     \r", CoreServices::getCore()->getFPS());
    sphere->setRotation(glm::vec3(0, 1, 0), 7.0f * delta);
}

void ReflectionsDemo::onGUI()
{
}
