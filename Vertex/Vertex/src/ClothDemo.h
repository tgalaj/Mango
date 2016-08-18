#pragma once
#include "BaseGame.h"
#include "Scene.h"
#include "Model.h"
#include "FreeCamera.h"
#include "ParticleEffect.h"

class ClothDemo : public BaseGame
{
public:
    ClothDemo();
    ~ClothDemo();

    void processInput();
    void update(float delta);
    void onGUI();

private:
    Scene      * scene;
    FreeCamera * cam;
    Cloth      * cloth;
};

