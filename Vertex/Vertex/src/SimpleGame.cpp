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
    model->loadModel("res/models/nanosuit/nanosuit.obj");
    model->setPosition(glm::vec3(0.0f, -1.75f, 0.0f));
    model->setScale(glm::vec3(0.2f));

    model2 = new Model();
    model2->genPlane(5, 5, 15, 15);
    model2->setTexture("res/texture/rooftiles.jpg");
    model2->setScale(glm::vec3(5.0f));

    model3 = new Model();
    model3->genCone(1.0, 0.5, 12, 12);
    model3->setTexture("res/texture/aztec.jpg");
    model3->setPosition(glm::vec3(-1.0f, 0.51f, 1.0f));

    //model->genTorus();
    //model->setTexture("res/texture/wood.jpg");

    model->addChild(model2);
    model2->addChild(model3);
    model->update(glm::mat4(1.0f), true);

    cam = new PerspectiveCamera(60.0f, 1024, 768, 0.01f, 50.0f);
}

SimpleGame::~SimpleGame()
{
    delete shader;
    shader = nullptr;

    delete model;
    model = nullptr;

    /*delete model2;
    model2 = nullptr;

    delete model3;
    model3 = nullptr;*/
}

void SimpleGame::processInput()
{
    static bool vsync = true;

    if (Input::getKeyDown(Input::A))
        VertexEngineCore::setClearColor(0.0, 1.0, 0.0, 1.0);

    if (Input::getKeyDown(Input::W))
        VertexEngineCore::setClearColor(1.0, 0.0, 0.0, 1.0);

    if (Input::getKeyDown(Input::V))
        VertexEngineCore::setVSync(vsync = !vsync);

    if(Input::getKeyDown(Input::KeypadMinus))
        cam->setFieldOfView(cam->getFieldOfView() - 1.0f);

    if(Input::getKeyDown(Input::KeypadPlus))
        cam->setFieldOfView(cam->getFieldOfView() + 1.0f);

    if (Input::getKeyDown(Input::U))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (Input::getKeyDown(Input::I))
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (Input::getKeyDown(Input::Escape))
        VertexEngineCore::quitApp();
}

void SimpleGame::update(float delta)
{
    float r = 4.0f;
    static float t = 0;
    t += delta;

    cam->setPosition(r*cos(t), sin(t)*0, r*sin(t));
    cam->update();
}

void SimpleGame::render()
{
    shader->setUniformMatrix4fv("view", cam->getView());
    shader->setUniformMatrix4fv("viewProj", cam->getViewProjection());
    model->render(shader);
    model2->render(shader);
    model3->render(shader);
}