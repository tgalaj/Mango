#include <iostream>
#include "Framebuffer.h"

int main()
{
    std::cout << "Vertextracer!\n";

    Model * monkey = new Model();
    monkey->loadModel("res/models/monkey.obj");

    Scene scene; //Todo pass scene as a reference to be feed from a file OR create static loader function in Scene and return copy of options
    scene.m_objects.push_back(monkey);

    Options options;
    options.width = 640;
    options.height = 480;
    options.fov = 65.0f;
    options.output_file_name = "output";
    //options.cam_origin = glm::vec3(0.0f, scene.atmosphere.planet_radius + 1000.0f, 30000.0f);
    options.cam_origin = glm::vec3(0.0f, 0.0f, 4.0f);

    Framebuffer framebuffer(options);
    framebuffer.render(scene);
    
    system("pause");
    return EXIT_SUCCESS;
}
