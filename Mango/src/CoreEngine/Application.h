#pragma once

namespace mango
{
    class Application
    {
    public:
        //TODO: put code from the Core class here:
        Application();
        virtual ~Application();

        void run();
    };

    // Client must define this function
    Application* createApplication(int argc, char** argv);
}
