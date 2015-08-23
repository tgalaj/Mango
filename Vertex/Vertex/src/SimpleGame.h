#pragma once

#include "BaseGame.h"
#include "Shader.h"
#include "Model.h"

class SimpleGame : public BaseGame
{
public:
    SimpleGame();
    ~SimpleGame();

    void processInput() override;
    void update()       override;

private:
    Shader *shader;
    Model  *model;
};