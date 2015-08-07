#include "Input.h"
#include "SimpleGame.h"
#include "VertexEngineCore.h"

int main(int argc, char* args[])
{
    VertexEngineCore vec("Hello Vertex!", 800, 600, new SimpleGame());
    vec.setVSync(true);
    vec.start();
    
	return 0;
}