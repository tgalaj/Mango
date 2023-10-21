#pragma once
#include "CoreEngine/Application.h"

extern mango::Application* mango::createApplication(int argc, char** argv);

int main(int argc, char** argv)
{
    //mango::Core engine(std::make_shared<TestDemo>(), 999.0f);
    //
    //engine.addSystem<MoveSystem>();
    //engine.init(1920, 1080, "Sample Game");
    //engine.start();

    auto app = mango::createApplication(argc, argv);
    app->run();
    delete app;

    return 0;
}