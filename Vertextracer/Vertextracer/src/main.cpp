#include <iostream>
#include "Framebuffer.h"

int main()
{
    std::cout << "Vertextracer!\n";

    Options options;
    Scene scene;
    scene.loadScene("simple.txt", options);

    Framebuffer framebuffer(options);
    framebuffer.render(scene);

    return EXIT_SUCCESS;
}
