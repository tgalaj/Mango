#include <Mango.h>

#include "Scenes/TestDemo.h"
#include "Systems/MoveSystem.h"

#include <memory>

class TestApp : public mango::Application
{
public:
    TestApp()
    {

    }

    ~TestApp()
    {

    }
};

mango::Application* mango::createApplication(int argc, char** argv)
{
    return new TestApp();
}
