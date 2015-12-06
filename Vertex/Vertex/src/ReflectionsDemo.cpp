#include "CoreServices.h"
#include "FreeCamera.h"
#include "ReflectionsDemo.h"
#include "VertexEngineCore.h"
#include "DirectionalLight.h"

#include <glm\gtc\constants.hpp>
#include <glm\gtc\random.hpp>

ReflectionsDemo::ReflectionsDemo()
{
    cam = new FreeCamera(60.0f, 1024, 768, 0.01f, 50000.0f);
    cam->setPosition(6.84f, 8.12f, 18.93f);
    cam->setDirection(-0.3f, -0.3f, -0.8f);

    scene = new Scene(cam);
    CoreServices::getCore()->setScene(scene);

    sphere = CoreAssetManager::createModel();
    sphere->genSphere(4.0f, 50);
    sphere->setTexture("res/texture/white_4x4.jpg");
    sphere->setDiffuseColor(0, glm::vec3(1, 0, 0));

    int numCubes = 40;
    float radius = 10.0f;
    float alpha  = glm::two_pi<float>() / numCubes;

    for(int i = 0; i < numCubes; ++i)
    {
        Model * cube = CoreAssetManager::createModel();
        cube->genCube(1);
        cube->setTexture("res/texture/white_4x4.jpg");
        cube->setDiffuseColor(0, glm::ballRand(1.0f));
        cube->setPosition(glm::vec3(radius * glm::cos(alpha * i), 0.0, radius * glm::sin(alpha * i)));
        sphere->addChild(cube);
    }
    
    DirectionalLight * dirLight = new DirectionalLight();
    dirLight->setDirection(glm::vec3(-1, -1, 1));
    
    scene->addChild(sphere);
    scene->addChild(dirLight);
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

    if (Input::getKeyUp(Input::C))
    {
        printf("========= Camera info =========\n"
               "| Position = %.2f, %.2f, %.2f |\n"
               "| Dir      = %.2f, %.2f, %.2f |\n"
               "| Up       = %.2f, %.2f, %.2f |\n",
               cam->getPosition().x,  cam->getPosition().y,  cam->getPosition().z,
               cam->getDirection().x, cam->getDirection().y, cam->getDirection().z,
               cam->getUpVector().x,  cam->getUpVector().y,  cam->getUpVector().z);
    }
}

void ReflectionsDemo::update(float delta)
{
    sphere->setRotation(glm::vec3(0, 1, 0), 5.0f * delta);
}

void ReflectionsDemo::onGUI()
{
}
