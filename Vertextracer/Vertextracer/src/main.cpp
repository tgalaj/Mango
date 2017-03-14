#include <iostream>
#include "Framebuffer.h"
#include "DirectionalLight.h"
#include "Time.h"

int main()
{
    Options options;
    Scene scene;
    scene.loadScene("city.txt", options);

    Framebuffer framebuffer(options);
    options.printConfiguration();

    if (!options.RENDER_MULTI_FRAMES)
    {
        framebuffer.render(scene);
    }
    else
    {
        /* Render sequence of images */
        int num_angles = 128;//options.NUM_MULTI_FRAMES_MAX - options.NUM_MULTI_FRAMES_MIN;

        std::string output_file_name = framebuffer.m_options.OUTPUT_FILE_NAME;

        auto start_time = Time::getTime();
        for (uint32_t i = options.NUM_MULTI_FRAMES_MIN; i < options.NUM_MULTI_FRAMES_MAX; ++i)
        {
            printf("Image %d/%d\n", i + 1, options.NUM_MULTI_FRAMES_MAX);
            double angle = i / float(num_angles) * glm::pi<double>() * 0.6;

            glm::vec3 sun_dir = glm::normalize(-glm::vec3(0.0f, glm::cos(angle), -glm::sin(angle)));
            scene.atmosphere->sun_light = static_cast<DirectionalLight*>(scene.m_lights[0]);
            scene.atmosphere->sun_light->m_direction = -sun_dir;
            framebuffer.m_options.OUTPUT_FILE_NAME = output_file_name + std::to_string(i+1);

            framebuffer.render(scene);
        }
        std::cout << "\rRaytracing time = " << Time::getTime() - start_time << "s" << std::endl;
        system((output_file_name + std::to_string(options.NUM_MULTI_FRAMES_MIN + 1) + ".ppm").c_str());
    }

    return EXIT_SUCCESS;
}
