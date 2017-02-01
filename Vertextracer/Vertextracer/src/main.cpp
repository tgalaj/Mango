#include <iostream>
#include "Framebuffer.h"
#include "DirectionalLight.h"
#include "Time.h"

int main()
{
    std::cout << "Vertextracer!\n";

    Options options;
    Scene scene;
    scene.loadScene("city.txt", options);

    Framebuffer framebuffer(options);

#if RENDER_SINGLE
    framebuffer.render(scene);
#else
    /* Render sequence of images */
    int num_angles = 128;

    std::string output_file_name = framebuffer.m_options.output_file_name;

    auto start_time = Time::getTime();
    for (int i = 0; i < num_angles; ++i)
    {
        printf("Image %d/%d\n", i+1, num_angles);
        double angle = i / float(num_angles) * glm::pi<double>() * 0.6;

        glm::vec3 sun_dir = glm::normalize(-glm::vec3(0.0f, glm::cos(angle), -glm::sin(angle)));
        scene.atmosphere->sun_direction = sun_dir;
        static_cast<DirectionalLight*>(scene.m_lights[0])->m_direction = -sun_dir;
        framebuffer.m_options.output_file_name = output_file_name + std::to_string(i);

        framebuffer.render(scene);
    }
    std::cout << "\rRaytracing time = " << Time::getTime() - start_time << "s" << std::endl;
#endif

    return EXIT_SUCCESS;
}
