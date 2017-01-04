#include <iostream>
#include "Framebuffer.h"
#include "Sphere.h"

int main()
{
    std::cout << "Vertextracer!\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    Scene scene; //Todo pass scene as a reference to be feed from a file OR create static loader function in Scene and return copy of options

    // generate a scene made of random spheres
    uint32_t numSpheres = 32;
    gen.seed(0);
    for (uint32_t i = 0; i < numSpheres; ++i) 
    {
        glm::vec3 randPos((0.5 - dis(gen)) * 10, (0.5 - dis(gen)) * 10, (0.5 + dis(gen) * 10));
        float randRadius = (0.5 + dis(gen) * 0.5);
        scene.m_objects.push_back(new Sphere(randPos, randRadius));
    }

    Options options;
    options.width = 640;
    options.height = 480;
    options.fov = 65.0f;
    options.output_file_name = "output";
    //options.cam_origin = glm::vec3(0.0f, scene.atmosphere.planet_radius + 1000.0f, 30000.0f);

    Framebuffer framebuffer(options);
    framebuffer.render(scene);
    
    system("pause");
    return EXIT_SUCCESS;
}
