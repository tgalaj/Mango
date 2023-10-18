#include "Scenes/TestDemo.h"
#include "Systems/MoveSystem.h"
#include "CoreEngine/Core.h"
#include <memory>

int main(int argc, char * args[])
{
    mango::Core vec(std::make_shared<TestDemo>(), 999.0f);

    vec.addSystem<MoveSystem>();
    vec.init(1920, 1080, "Sample Game");
    vec.start();
   
    return 0;
}
