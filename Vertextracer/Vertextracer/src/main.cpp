#include <iostream>
#include "Framebuffer.h"

int main()
{
    std::cout << "Vertextracer!\n";

    Options options;
    options.width = 640;
    options.height = 480;
    options.fov = 60.0f;
    options.output_file_name = "output";

    Scene scene; //Todo pass scene as a reference to be feed from a file OR create static loader function in Scene and return copy of options

    Framebuffer framebuffer(options);
    framebuffer.render(scene);
    
    system("pause");
    return EXIT_SUCCESS;
}
