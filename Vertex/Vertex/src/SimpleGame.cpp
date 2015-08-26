#include "SimpleGame.h"
#include "VertexEngineCore.h"
#include "CoreAssetManager.h"

SimpleGame::SimpleGame()
{
    shader = new Shader("res/shaders/AllShaders.glsl");
    shader->link();
    shader->apply();

    model = new Model();
    //model->genTorus(1.0f, 2.0f, 32, 32);
    //model->genCube(3.0f);
    model->genCylinder(3, 1, 6);

    cam = new PerspectiveCamera(60.0f, 800, 600, 0.01f, 50.0f);
}

SimpleGame::~SimpleGame()
{
    delete shader;
    shader = nullptr;

    delete model;
    model = nullptr;
}

void SimpleGame::processInput()
{
    static bool vsync = true;

    if (Input::getKeyDown(Input::Return))
        printf("GetKeyDown - Return\n");

    if (Input::getKeyDown(Input::A))
        VertexEngineCore::setClearColor(0.0, 1.0, 0.0, 1.0);

    if (Input::getKeyDown(Input::W))
        VertexEngineCore::setClearColor(1.0, 0.0, 0.0, 1.0);

    if (Input::getKeyDown(Input::S))
        CoreAssetManager::loadFile("res/shaders/Basic.vert");

    if (Input::getKeyDown(Input::V))
        VertexEngineCore::setVSync(vsync = !vsync);

    if (Input::getKeyDown(Input::V))
        VertexEngineCore::setVSync(vsync = !vsync);

    if(Input::getKeyDown(Input::KeypadMinus))
        cam->fieldOfView -= 0.1f;

    if(Input::getKeyDown(Input::KeypadPlus))
        cam->fieldOfView += 0.1f;

    if (Input::getKeyDown(Input::U))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (Input::getKeyDown(Input::I))
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (Input::getKeyDown(Input::Escape))
        VertexEngineCore::quitApp();
}

void SimpleGame::update()
{
    float r = 5.0f;
    float t = Time::getTime();

    printf("Camera fov = %.2f\r", cam->fieldOfView);
    cam->setPosition(r*cos(t), 4, r*sin(t));
    cam->update();
}

void SimpleGame::render()
{
    shader->setUniformMatrix4fv("view", cam->getView());
    shader->setUniformMatrix4fv("viewProj", cam->getViewProjection());
    model->render();
}