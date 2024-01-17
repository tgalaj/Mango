#pragma once
#include "Mango.h"

class EditorSystem : public mango::System
{
public:
    EditorSystem();
    ~EditorSystem() = default;

    void onInit() override;
    void onDestroy() override;
    void onUpdate(float dt) override;
    void onGui() override;

private:
    void moveLights(float dt);

private:
    std::shared_ptr<mango::Scene> m_mainScene;
    std::shared_ptr<mango::FreeCameraController> m_freeCameraController;

    mango::Entity m_camera1;
    mango::Entity m_camera2;
};