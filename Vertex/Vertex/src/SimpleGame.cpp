#include "SimpleGame.h"
#include "VertexEngineCore.h"
#include "CoreAssetManager.h"

#include <glm\gtc\matrix_transform.hpp>

SimpleGame::SimpleGame()
{
    shader = new Shader("res/shaders/AllShaders.glsl");
    shader->link();
    shader->apply();

    model = new Model();
    model->loadModel(std::string("res/models/nanosuit/nanosuit.obj"));
    //model->genTorus();
    //model->setTexture("res/texture/wood.jpg");

    cam = new PerspectiveCamera(60.0f, 1024, 768, 0.01f, 50.0f);
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
        cam->fieldOfView -= 1.0f;

    if(Input::getKeyDown(Input::KeypadPlus))
        cam->fieldOfView += 1.0f;

    if (Input::getKeyDown(Input::U))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (Input::getKeyDown(Input::I))
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (Input::getKeyDown(Input::Escape))
        VertexEngineCore::quitApp();
}

void SimpleGame::update()
{
    float r = 4.0f;
    float t = Time::getTime();

    printf("Camera fov = %.2f\r", cam->fieldOfView);
    cam->setPosition(r*cos(t), sin(t)*0, r*sin(t));
    cam->update();
}

void SimpleGame::render()
{
    glm::mat4 m(1.0f);
    m = glm::translate(m, glm::vec3(0.0f, -1.75f, 0.0f));
    m = glm::scale(m, glm::vec3(0.2f, 0.2f, 0.2f));

    shader->setUniformMatrix4fv("model", m);
    shader->setUniformMatrix4fv("view", cam->getView());
    shader->setUniformMatrix4fv("viewProj", cam->getViewProjection());
    model->render();
}