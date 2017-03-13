#include <iostream>
#include "Framebuffer.h"
#include "DirectionalLight.h"
#include "Time.h"

double f(double x)
{
    return (1.0 - x + ((x * x) / 2.0) - ((x*x*x) / 6.0) + ((x*x*x*x) / (24.0)));
}

double fhorner(double x, const double * CoefficientsOfPolynomial, const unsigned int & DegreeOfPolynomial)
{
    double dbResult = 0.0;
    int i;
    for (i = DegreeOfPolynomial; i >= 0; i--)
    {
        dbResult = dbResult * x + CoefficientsOfPolynomial[i];
    }
    return dbResult;
}

int main()
{
    Options options;
    Scene scene;
    scene.loadScene("simple.txt", options);

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
            printf("Image %d/%d\n", i + 1, options.NUM_MULTI_FRAMES_MAX - options.NUM_MULTI_FRAMES_MIN + 1);
            double angle = i / float(num_angles) * glm::pi<double>() * 0.6;

            glm::vec3 sun_dir = glm::normalize(-glm::vec3(0.0f, glm::cos(angle), -glm::sin(angle)));
            scene.atmosphere->sun_light = static_cast<DirectionalLight*>(scene.m_lights[0]);
            scene.atmosphere->sun_light->m_direction = -sun_dir;
            framebuffer.m_options.OUTPUT_FILE_NAME = output_file_name + std::to_string(i+1);

            framebuffer.render(scene);
        }
        std::cout << "\rRaytracing time = " << Time::getTime() - start_time << "s" << std::endl;
        system((framebuffer.m_options.OUTPUT_FILE_NAME + std::to_string(options.NUM_MULTI_FRAMES_MIN + 1) + ".ppm").c_str());
    }

    //const int num_iter = 10000000;
    //const double coeffs[6] = { -1, 1.0 / 2.0, 1.0 / 6.0, 1.0 / 24.0, 1.0 };
    //{
    //    double start = Time::getTime();
    //    for (int i = 0; i < num_iter; ++i)
    //    {
    //        double res = glm::exp(-i /(double)num_iter);
    //        printf("%d/%d\r", i + 1, num_iter);
    //    }
    //    printf("\nExp time = %.2f\n\n", Time::getTime() - start);
    //}

    //{
    //    double start = Time::getTime();
    //    for (int i = 0; i < num_iter; ++i)
    //    {
    //        double res = fhorner(-i /(double)num_iter, coeffs, 6);
    //        printf("%d/%d\r", i + 1, num_iter);
    //    }
    //    printf("\nF time = %.2f\n\n", Time::getTime() - start);
    //}

    //system("pause");
    return EXIT_SUCCESS;
}
