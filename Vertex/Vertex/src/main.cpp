#include "ClothDemo.h"
#include "MultiDrawIndirectDemo.h"
#include "ParticlesDemo.h"
#include "ReflectionsDemo.h"
#include "SimpleGame.h"
#include "VertexEngineCore.h"

int main(int argc, char * args[])
{
    VertexEngineCore vec("Hello Vertex!", 1024, 768);
    vec.start(new ReflectionsDemo());
    
    return 0;
}