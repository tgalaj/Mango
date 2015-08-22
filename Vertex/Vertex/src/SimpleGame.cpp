#include "SimpleGame.h"
#include "VertexEngineCore.h"
#include "CoreAssetManager.h"
#include "Shader.h"

SimpleGame::SimpleGame()
{
    
}

SimpleGame::~SimpleGame()
{

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

    if (Input::getKeyDown(Input::D))
    {
        Shader s("res/shaders/AllShaders.glsl");
    }

    if (Input::getKeyDown(Input::V))
        VertexEngineCore::setVSync(vsync = !vsync);

    if (Input::getKeyDown(Input::Escape))
        VertexEngineCore::quitApp();
}

void SimpleGame::update()
{
    printf("Game update!!\r");
}
