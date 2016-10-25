#include "Scene.h"
#include "Framebuffer.h"

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
        scene.loadScene("subm/scene4-specular.test");
    #endif

    Framebuffer framebuffer(scene.width, scene.height);
    framebuffer.raytrace(scene, scene.outputFilename);

    return EXIT_SUCCESS;
}