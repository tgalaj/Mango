#include "Input.h"
#include "SimpleGame.h"
#include "VertexEngineCore.h"
#include <iostream>
int main(int argc, char* args[])
{
    VertexEngineCore vec("Hello Vertex!", 800, 600);
    VertexEngineCore::setVSync(true);
    vec.start(new SimpleGame());
    
	return 0;
}