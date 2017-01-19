#include <iostream>
#include "Framebuffer.h"
#include "DirectionalLight.h"

int main()
{
    std::cout << "Vertextracer!\n";

    Model * monkey = new Model(glm::lookAt(glm::vec3(0.0f), glm::vec3(-1,-1,-1), glm::vec3(0, 1, 0)));
    monkey->loadModel("res/models/monkey.obj");
    monkey->m_material->m_albedo = glm::vec3(0.14f, 0.74f, 0.0f);
    monkey->m_material->m_ks = 0.1f;
    monkey->m_material->m_specular_exponent = 10.0f;
    //monkey->m_type = MaterialType::REFLECTION_AND_REFRACTION;
    //monkey->m_index_of_refreaction = 1.5f;

    Model * sphere = new Model(glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.0f, 2.2f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)));
    sphere->loadModel("res/models/sphere.obj");
    sphere->m_material->m_albedo = glm::vec3(0.52f, 0.0f, 0.46f);
    sphere->m_material->m_type = MaterialType::REFLECTION_AND_REFRACTION;
    sphere->m_material->m_index_of_refreaction = 1.8f;

    Model * plane = new Model(glm::translate(glm::mat4(), glm::vec3(0, 1, 0)));
    plane->loadModel("res/models/plane.obj");
    plane->m_material->m_albedo = glm::vec3(0.7f, 0.7f, 0.7f);
    //plane->m_type = MaterialType::REFLECTION;

    DirectionalLight * dir_light = new DirectionalLight();;
    dir_light->m_intensity = 1.5f;

    Scene scene; //Todo pass scene as a reference to be feed from a file OR create static loader function in Scene and return copy of options
    scene.m_objects.push_back(monkey);
    scene.m_objects.push_back(sphere);
    scene.m_objects.push_back(plane);

    scene.m_lights.push_back(dir_light);

    Options options {};
    options.width = 640;
    options.height = 480;
    options.fov = 60.0f;
    options.output_file_name = "output";
    //options.cam_origin = glm::vec3(0.0f, scene.atmosphere.planet_radius + 1000.0f, 30000.0f);
    options.cam_origin = glm::vec3(0.0f, -0.5f, 5.5f);

    Framebuffer framebuffer(options);
    framebuffer.render(scene);

    return EXIT_SUCCESS;
}
