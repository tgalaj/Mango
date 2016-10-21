#include <iostream>

#include <glm/glm.hpp>
#include <FreeImage.h>

#include "Scene.h"
#include "Time.h"
#include "Raytracer.h"

#define BPP 24 //cause three 8 bit RGB values
//#define CMD

int main(int argc, char * argv[])
{
    Scene scene;

    #ifdef CMD
        if (argc < 2)
        {
            printf("Error: no scene file provided.\n");
            return EXIT_FAILURE;
        }

        scene.loadScene(argv[1]);
    #else
        scene.loadScene("subm/scene5.test");
    #endif

    Raytracer raytracer(scene);

    FreeImage_Initialise();

    /* Raytracing code */
    int width  = scene.width;
    int height = scene.height;

    int total_pixels = width * height;
    int pixels_counter = 0;

    FIBITMAP* image = FreeImage_Allocate(width, height, BPP);
    RGBQUAD color;
    
    if (!image)
    {
        printf("ERROR: Could not allocate the image!\n");
        return EXIT_FAILURE;
    }

    double start_time = Time::getTime();
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            glm::vec3 c = raytracer.illuminate(j, i);

            color.rgbRed   = static_cast<BYTE>(c.r * 255.0f + 0.5f);
            color.rgbGreen = static_cast<BYTE>(c.g * 255.0f + 0.5f);
            color.rgbBlue  = static_cast<BYTE>(c.b * 255.0f + 0.5f);

            FreeImage_SetPixelColor(image, width-j, i, &color);
            
            ++pixels_counter;
            if (pixels_counter % 1000 == 0)
                printf("%d / %d pixels \r", pixels_counter, total_pixels);
        }
    }

    if (FreeImage_Save(FIF_PNG, image, scene.outputFilename.c_str(), 0))
    {
        printf("Image %s successfully saved!\n", scene.outputFilename.c_str());
        printf("Processing time = %.2fs\n", Time::getTime() - start_time);
    }

    FreeImage_DeInitialise();

    printf("Press any key to continue...\n");
    getchar();

    return EXIT_SUCCESS;
}