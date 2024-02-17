#pragma once
#include "Mango.h"

class Sandbox : public mango::System
{
public:
    Sandbox();
    ~Sandbox() = default;

    void onInit() override;
    void onDestroy() override;
    void onUpdate(float dt) override;
    void onGui() override;

private:
    void moveLights(float dt);

private:
    mango::ref<mango::Scene> m_mainScene;
    mango::ref<mango::FreeCameraController> m_freeCameraController;

    mango::Entity m_camera1;
    mango::Entity m_camera2;
};