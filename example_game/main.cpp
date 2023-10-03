#include "scenes/TestDemo.h"
#include "systems/MoveSystem.h"
#include "core_engine/VertexCore.h"
#include <memory>

int main(int argc, char * args[])
{
    Vertex::VertexCore vec(std::make_shared<TestDemo>(), 999.0f);

    vec.addSystem<MoveSystem>();
    vec.init(1920, 1080, "Sample Game");
    vec.start();
   
    return 0;
}
