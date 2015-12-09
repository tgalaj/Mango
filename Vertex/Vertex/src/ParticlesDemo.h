#pragma once
#include "BaseGame.h"
#include "Scene.h"
#include "Model.h"
#include "FreeCamera.h"
#include "ParticleEffect.h"

class ParticlesDemo : public BaseGame
{
public:
    ParticlesDemo();
    ~ParticlesDemo();

    void processInput();
    void update(float delta);
    void onGUI();

private:
    Scene      * scene;
    FreeCamera * cam;
    ParticleEffect * particles;
};

