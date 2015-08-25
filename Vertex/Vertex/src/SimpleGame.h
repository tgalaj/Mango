#pragma once

#include "BaseGame.h"
#include "PerspectiveCamera.h"
#include "Shader.h"
#include "Model.h"

class SimpleGame : public BaseGame
{
public:
    SimpleGame();
    ~SimpleGame();

    void processInput() override;
    void update()       override;
    void render()       override;

private:
    Shader *shader;
    Model  *model;
    PerspectiveCamera *cam;
};