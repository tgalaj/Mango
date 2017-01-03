#include <fstream>
#include <sstream>
#include <iostream>
#include <stack>

#include <glm/gtc/matrix_transform.hpp>

#include "Scene.h"

Scene::Scene()
    : atmosphere(nullptr),
      cam(nullptr)
{
    maxdepth = 5;
    width = 0;
    height = 0;
}

Scene::~Scene()
{
    if (atmosphere != nullptr)
    {
        delete atmosphere;
    }

    if (cam != nullptr)
    {
        delete cam;
    }
}

void Scene::loadScene(std::string filename)
{
    std::ifstream fs;
    std::string str, cmd;

    fs.open(filename);
    outputFilename = filename.substr(filename.find_first_of("/") + 1, filename.find('.') - filename.find_first_of("/")) + "png";
    
    int vertCounter = 0;
    float values[10];
    glm::vec3 * vertices = nullptr;

    Material m;
    glm::vec3 attenuation = glm::vec3(1.0f, 0.0f, 0.0f);

    std::stack<glm::mat4> transfstack;

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
                        width  = static_cast<int>(values[0]);
                        height = static_cast<int>(values[1]);
                    }
                }
                else if (cmd == "maxdepth")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        maxdepth = static_cast<int>(values[0]);
                    }
                }
                else if (cmd == "output")
                {
                    std::string output;
                    isValidInput = readvals(s, 1, output);

                    if (isValidInput)
                    {
                        outputFilename = output;
                    }
                }
                else if (cmd == "atmosphere")
                {
                    isValidInput = true;//readvals(s, 10, values);

                    if (isValidInput)
                    {
                        atmosphere = new Atmosphere();
                    }
                }
                else if (cmd == "camera")
                {
                    isValidInput = readvals(s, 10, values);
                    
                    if (isValidInput)
                    {
                        cam = new Camera(width, height);
                        cam->eye    = glm::vec3(values[0], values[1], values[2]);
                        cam->lookAt = glm::vec3(values[3], values[4], values[5]);
                        cam->up     = glm::vec3(values[6], values[7], values[8]);
                        cam->fovy   = values[9];
                        cam->update();

                        transfstack.push(glm::mat4(1.0f));
                    }
                }
                else if (cmd == "sphere")
                {
                    isValidInput = readvals(s, 4, values);

                    if (isValidInput)
                    {
                        Sphere sphere;
                        sphere.center = glm::vec3(values[0], values[1], values[2]);
                        sphere.radius = values[3];
                        sphere.mat = m;
                        sphere.transform = transfstack.top();

                        addSphere(sphere);
                    }
                }
                /* Triangle setup */
                else if (cmd == "maxverts")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        int size = static_cast<int>(values[0]);
                        vertices = new glm::vec3[size];
                    }
                }
                else if (cmd == "vertex")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        glm::vec3 v(values[0], values[1], values[2]);
                        
                        vertices[vertCounter] = v;
                        
                        ++vertCounter;
                    }
                }
                else if (cmd == "tri")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        Triangle t;
                        t.p0 = vertices[int(values[0])];
                        t.p1 = vertices[int(values[1])];
                        t.p2 = vertices[int(values[2])];
                        t.n = glm::normalize(glm::cross(t.p1 - t.p0, t.p2 - t.p0));
                        t.mat = m;
                        t.transform = transfstack.top();

                        addTriangle(t);
                    }
                }
                /* Material */
                else if (cmd == "ambient")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        m.ambient = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "diffuse")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        m.diffuse = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "specular")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        m.specular = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "emission")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        m.emission = glm::vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "shininess")
                {
                    isValidInput = readvals(s, 1, values);

                    if (isValidInput)
                    {
                        m.shininess = values[0];
                    }
                }
                /* Lights */
                else if (cmd == "directional")
                {
                    isValidInput = readvals(s, 6, values);

                    if (isValidInput)
                    {
                        Light l;
                        l.type    = LIGHT_TYPE::DIRECTIONAL;
                        l.dir_pos = glm::vec3(values[0], values[1], values[2]);
                        l.color   = glm::vec3(values[3], values[4], values[5]);
                        l.transform = transfstack.top();
                        l.light_intensity = 1.0f;

                        lights.push_back(l);
                    }
                }
                else if (cmd == "point")
                {
                    isValidInput = readvals(s, 6, values);

                    if (isValidInput)
                    {
                        Light l;
                        l.type    = LIGHT_TYPE::POINT;
                        l.dir_pos = glm::vec3(values[0], values[1], values[2]);
                        l.color   = glm::vec3(values[3], values[4], values[5]);
                        l.transform = transfstack.top();
                        l.light_intensity = 1.0f;

                        l.attenuation = attenuation;

                        lights.push_back(l);
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
                        glm::mat4 & T = transfstack.top();
                        T = T * glm::translate(glm::mat4(1.0f), glm::vec3(values[0], values[1], values[2]));
                    }
                }
                else if (cmd == "rotate")
                {
                    isValidInput = readvals(s, 4, values);

                    if (isValidInput)
                    {
                        glm::mat4 & T = transfstack.top();
                        T = T * glm::rotate(glm::mat4(1.0f), glm::radians(values[3]), glm::vec3(values[0], values[1], values[2]));
                    }
                }
                else if (cmd == "scale")
                {
                    isValidInput = readvals(s, 3, values);

                    if (isValidInput)
                    {
                        glm::mat4 & T = transfstack.top();
                        T = T * glm::scale(glm::mat4(1.0f), glm::vec3(values[0], values[1], values[2]));
                    }
                }
                /* Matrix stack */
                else if (cmd == "pushTransform")
                {
                    transfstack.push(transfstack.top());
                }
                else if (cmd == "popTransform")
                {
                    if (transfstack.size() <= 1)
                    {
                        printf("Stack has no elements. Cannot pop.\n");
                    }
                    else
                    {
                        transfstack.pop();
                    }
                }
            }

            std::getline(fs, str);
        }
    }
    else
    {
        printf("Unable to open file %s\n", filename.c_str());
    }

    delete[] vertices;
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

void Scene::addSphere(Sphere & s)
{
    spheres.push_back(s);
}

void Scene::addTriangle(Triangle & t)
{
    triangles.push_back(t);
}
