#pragma once
#include "Core/Application.h"

extern mango::Application* mango::createApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
    auto app = mango::createApplication({ argc, argv });
    app->start();
    delete app;

    return EXIT_SUCCESS;
}