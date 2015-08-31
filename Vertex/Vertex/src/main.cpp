#include "SimpleGame.h"
#include "VertexEngineCore.h"
#include <assimp\Importer.hpp>

int main(int argc, char* args[])
{
    Assimp::Importer imp;
    VertexEngineCore vec("Hello Vertex!", 800, 600);
    VertexEngineCore::setVSync(true);
    vec.start(new SimpleGame());
    
	return 0;
}