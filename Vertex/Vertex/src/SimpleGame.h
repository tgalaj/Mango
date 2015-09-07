#pragma once

#include "BaseGame.h"
#include "PerspectiveCamera.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

class SimpleGame : public BaseGame
{
public:
    SimpleGame();
    ~SimpleGame();

    void processInput()      override;
    void update(float delta) override;
    void render()            override;

private:
    Shader * shader;
    Model  * model;
    Model  * model2;
    Model  * model3;
    PerspectiveCamera * cam;
};