#include "SimpleGame.h"
#include "VertexEngineCore.h"

int main(int argc, char * args[])
{
    VertexEngineCore vec("Hello Vertex!", 1024, 768);
    vec.start(new SimpleGame());
    
	return 0;
}