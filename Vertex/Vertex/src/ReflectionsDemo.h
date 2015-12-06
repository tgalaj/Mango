#pragma once

#include "BaseGame.h"
#include "Scene.h"
#include "Model.h"
#include "FreeCamera.h"

class ReflectionsDemo : public BaseGame
{
public:
    ReflectionsDemo();
    ~ReflectionsDemo();

    void processInput()      override;
    void update(float delta) override;
    void onGUI()             override;

private:
    Scene      * scene;
    Model      * sphere;
    FreeCamera * cam;
};

