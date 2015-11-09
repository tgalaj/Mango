#pragma once

#include "BaseGame.h"
#include "PerspectiveCamera.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Scene.h"

class SimpleGame : public BaseGame
{
public:
    SimpleGame();
    ~SimpleGame();

    void processInput()      override;
    void update(float delta) override;
    void onGUI()             override;

private:
    Scene  * scene;
};