#pragma once
#include "Application.h"

extern mango::Application* mango::createApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
    auto app = mango::createApplication({ argc, argv });
    app->run();
    delete app;

    return EXIT_SUCCESS;
}