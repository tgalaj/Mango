#pragma once
#include "Mango.h"

class TestDemo : public mango::System
{
public:
    TestDemo();
    ~TestDemo() = default;

    void onInit() override;
    void onDestroy() override;
    void onUpdate(float dt) override;
    void onGui() override;

private:
    std::shared_ptr<mango::Scene> m_mainScene;
    std::shared_ptr<mango::FreeCameraController> m_freeCameraController;
};
