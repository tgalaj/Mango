#include "VertexEngineCore.h"

int main(int argc, char* args[])
{
    VertexEngineCore vec("Hello Vertex!", 800, 600);
    vec.setVSync(true);
    vec.start();

	return 0;
}