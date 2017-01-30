#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/matrix_transform.inl>
#include <glm/gtc/quaternion.hpp>

#include "Scene.h"
#include "PointLight.h"
#include "DirectionalLight.h"

Scene::Scene()
{
}

Scene::~Scene()
{
    for(size_t i = 0; i < m_objects.size(); ++i)
    {
        delete m_objects[i];
    }

    m_objects.clear();

    for (size_t i = 0; i < m_lights.size(); ++i)
    {
        delete m_lights[i];
    }

    m_lights.clear();
}

void Scene::loadScene(const std::string & scene_file_name, Options & options)
{
    std::ifstream fs;
    std::string str, cmd;

    fs.open("res/scenes/" + scene_file_name);
    options.output_file_name = scene_file_name.substr(scene_file_name.find_first_of("/") + 1, scene_file_name.find('.') - 1);

    glm::mat4 current_transformation(1.0f);
    float values[10];
    glm::vec3 attenuation = glm::vec3(1.0f, 0.0f, 0.0f);

    Material * m = new Material();
    DirectionalLight * dir_light = nullptr;

    bool was_dir_light_loaded = false;

    if (fs.is_open())
    {
        std::getline(fs, str);

        while (fs)
        {
            if ((str.find_first_not_of(" \t\r\n") != std::string::npos) && (str[0] != '#'))
            {
                std::stringstream s(str);
                s >> cmd;

                bool isValidInput;

                if (cmd == "size")
                {
                    isValidInput = readvals(s, 2, values);

                    if (isValidInput)
                    {
                        options.width = static_cast<int>(values[0]);
                        options.height = static_cast<int>(values[1]);
                    }
                }
                else if (cmd == "maxdepth")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        options.max_depth = static_cast<int>(values[0]);
                    }
                }
                else if (cmd == "output")
                {
                    std::string output;
                    isValidInput = readvals(s, 1, output);

                    if (isValidInput)
                    {
                        options.output_file_name = output;
                    }
                }
                else if (cmd == "camera_origin")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        options.cam_origin = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "camera_lookat")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        options.cam_lookat = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "camera_up")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        options.cam_up = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "camera_fov")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        options.fov = values[0];
                    }
                }
                /* Materials */
                else if (cmd == "ambient")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        //m.ambient = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "albedo")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        m->m_albedo = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "texture")
                {
                    std::string tex_file_name;
                    isValidInput = readvals(s, 1, tex_file_name);

                    if (isValidInput)
                    {
                        m->m_textrue = new Texture();
                        m->m_textrue->load(tex_file_name);
                    }
                }
                else if (cmd == "texture_linear")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        m->m_textrue->use_linear = values[0];
                    }
                }
                else if (cmd == "specular")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        //m.specular = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "emission")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        //m.emission = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "shininess")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        m->m_specular_exponent = values[0];
                    }
                }
                else if (cmd == "ior")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        m->m_index_of_refreaction = values[0];
                    }
                }
                else if (cmd == "flat")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        m->use_flat_shading = values[0];
                    }
                }
                else if (cmd == "ka")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        m->m_ka = values[0];
                    }
                }
                else if (cmd == "kd")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        m->m_kd = values[0];
                    }
                }
                else if (cmd == "ks")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        m->m_ks = values[0];
                    }
                }
                else if (cmd == "phong")
                {
                    m->m_type = MaterialType::DIFFUSE;
                }
                else if (cmd == "reflective")
                {
                    m->m_type = MaterialType::REFLECTION;
                }
                else if (cmd == "refractive")
                {
                    m->m_type = MaterialType::REFLECTION_AND_REFRACTION;
                }
                /* Models/Objects */
                else if (cmd == "object")
                {
                    std::string model_file_name;
                    isValidInput = readvals(s, 1, model_file_name);

                    if (isValidInput)
                    {
                        Model * object = new Model(current_transformation, model_file_name);
                        object->m_material = new Material(*m);
                        m_objects.push_back(object);
                    }
                }
                /* Lights */
                else if (cmd == "atmosphere")
                {
                    isValidInput = readvals(s, 2, values);

                    if (isValidInput && dir_light != nullptr)
                    {
                        atmosphere = new Atmosphere();

                        if (values[0] > 0.0f && values[1] > 0.0f)
                        {
                            atmosphere->planet_radius     = values[0];
                            atmosphere->atmosphere_radius = values[1];
                        }

                        atmosphere->sun_direction = glm::normalize(-dir_light->m_direction);
                    }
                }
                else if (cmd == "directional" && !was_dir_light_loaded)
                {
                    isValidInput = readvals(s, 7, values);

                    if (isValidInput)
                    {
                        DirectionalLight * l = new DirectionalLight();
                        l->m_direction = glm::vec3(values[0], values[1], values[2]);
                        l->m_color = glm::vec3(values[3], values[4], values[5]);
                        l->m_model_matrix = current_transformation;
                        l->m_intensity = values[6];

                        l->update();
                        m_lights.push_back(l);

                        dir_light = l;
                        was_dir_light_loaded = true;
                    }
                }
                else if (cmd == "point")
                {
                    isValidInput = readvals(s, 7, values);

                    if (isValidInput)
                    {
                        PointLight * l = new PointLight();
                        l->m_position = glm::vec3(values[0], values[1], values[2]);
                        l->m_color = glm::vec3(values[3], values[4], values[5]);
                        l->m_model_matrix = current_transformation;
                        l->m_intensity = values[6];
                        l->update();
                        //TODO: l->attenuation = attenuation;

                        m_lights.push_back(l);
                    }
                }
                else if (cmd == "attenuation")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        attenuation = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                /* Transformations */
                else if (cmd == "translate")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        current_transformation = current_transformation * glm::translate(glm::mat4(1.0f), glm::vec3(values[0], values[1], values[2]));
                    }
                }
                else if (cmd == "rotate")
                {
                    isValidInput = readvals(s, 4, values);

                    if (isValidInput)
                    {
                        glm::quat q = glm::angleAxis(glm::radians(values[3]), glm::vec3(values[0], values[1], values[2]));
                        current_transformation = current_transformation * glm::mat4(q);
                    }
                }
                else if (cmd == "scale")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        current_transformation = current_transformation * glm::scale(glm::mat4(1.0f), glm::vec3(values[0], values[1], values[2]));
                    }
                }
                else if (cmd == "look_at")
                {
                    isValidInput = readvals(s, 9, values);

                    if (isValidInput)
                    {
                        current_transformation = glm::lookAt(glm::vec3(values[0], values[1], values[2]),
                                                             glm::vec3(values[3], values[4], values[5]),
                                                             glm::vec3(values[6], values[7], values[8]));
                    }
                }
                else if (cmd == "reset")
                {
                    current_transformation = glm::mat4(1.0f);
                    
                    delete m;
                    m = new Material();
                }
                else if (cmd == "antialiasing")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                       options.enable_antialiasing = values[0];
                    }
                }
                else if (cmd == "antialiasing_max_depth")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        options.aa_max_depth = values[0];
                    }
                }
                else if (cmd == "antialiasing_epsilon")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        options.aa_epsilon = values[0];
                    }
                }
            }

            std::getline(fs, str);
        }
    }
    else
    {
        printf("Unable to open file %s\n", scene_file_name.c_str());
    }
}

bool Scene::readvals(std::stringstream &s, const int numvals, float* values) const
{
    for (int i = 0; i < numvals; i++) {
        s >> values[i];
        if (s.fail()) {
            std::cout << "Failed reading value " << i << " will skip\n";
            return false;
        }
    }
    return true;
}

bool Scene::readvals(std::stringstream &s, const int numvals, std::string & values) const
{
    std::string ts;

    for (int i = 0; i < numvals; i++) {
        s >> ts;
        values += ts;
        if (s.fail()) {
            std::cout << "Failed reading value " << i << " will skip\n";
            return false;
        }
    }
    return true;
}
