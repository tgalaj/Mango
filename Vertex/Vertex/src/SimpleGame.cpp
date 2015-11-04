#include "SimpleGame.h"
#include "VertexEngineCore.h"
#include "CoreAssetManager.h"

#include <glm\gtc\matrix_transform.hpp>

SimpleGame::SimpleGame()
{
    scene = new Scene(new PerspectiveCamera(60.0f, 1024, 768, 0.01f, 50.0f));
    CoreServices::getCore()->setScene(scene);

    mat = new Shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");

    Model * model = new Model();
    model->loadModel("res/models/nanosuit/nanosuit.obj");
    model->setPosition(glm::vec3(0.0f, -1.75f, 0.0f));
    model->setScale(glm::vec3(0.2f));
    model->setMaterial(mat);

    Model * model2 = new Model();
    model2->genPlane(5, 5, 15, 15);
    model2->setTexture("res/texture/rooftiles.jpg");
    model2->setScale(glm::vec3(5.0f));
    model2->setMaterial(mat);

    Model * model3 = new Model();
    model3->genCone(1.0, 0.5, 12, 12);
    model3->setTexture("res/texture/aztec.jpg");
    model3->setPosition(glm::vec3(-1.0f, 0.51f, 1.0f));
    model3->setMaterial(mat);

    //model->genTorus();
    //model->setTexture("res/texture/wood.jpg");

    model->addChild(model2);
    model2->addChild(model3);

    scene->addChild(model);
}

SimpleGame::~SimpleGame()
{
    delete scene;
    scene = nullptr;

    delete mat;
    mat = nullptr;
}

void SimpleGame::processInput()
{
    static bool vsync = true;

    if (Input::getKeyDown(Input::A))
        CoreServices::getRenderer()->setClearColor(0.0, 1.0, 0.0, 1.0);

    if (Input::getKeyDown(Input::W))
        CoreServices::getRenderer()->setClearColor(1.0, 0.0, 0.0, 1.0);

    if (Input::getKeyDown(Input::V))
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
    float r = 4.0f;
    static float t = 0;
    t += delta;

    CoreServices::getRenderer()->getCamera()->setPosition(r*cos(t), sin(t)*0, r*sin(t));

    printf("FPS = %d      \r", CoreServices::getCore()->getFPS());
}