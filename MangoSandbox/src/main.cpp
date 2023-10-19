#include "CoreEngine/Core.h"
#include "Scenes/TestDemo.h"
#include "Systems/MoveSystem.h"

#include <memory>

int main(int argc, char * args[])
{
    mango::Core engine(std::make_shared<TestDemo>(), 999.0f);

    engine.addSystem<MoveSystem>();
    engine.init(1920, 1080, "Sample Game");
    engine.start();
   
    return 0;
}
