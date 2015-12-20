#include "ClothDemo.h"
#include "MultiDrawIndirectDemo.h"
#include "ParticlesDemo.h"
#include "ReflectionsDemo.h"
#include "SimpleGame.h"
#include "VertexEngineCore.h"

int main(int argc, char * args[])
{
    VertexEngineCore vec("Compute shader cloth simulation", 1280, 720);
    vec.start(new ClothDemo());
    
    return 0;
}