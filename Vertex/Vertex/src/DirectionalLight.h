#pragma once

#include "SceneNode.h"

class DirectionalLight : public SceneNode
{
public:
    friend class Renderer;

    DirectionalLight();
    ~DirectionalLight();

    void setDirection(glm::vec3 & _direction)
    {
        direction = _direction;
        isDirty = true;
    }

    void setIntensity(glm::vec3 & _intensity)
    {
        intensity = _intensity;
        isDirty = true;
    }

    void setAmbientColor(glm::vec3 & _ambient)
    {
        ambient = _ambient;
        isDirty = true;
    }

    void setDiffuseColor(glm::vec3 & _diffuse)
    {
        diffuse = _diffuse;
        isDirty = true;
    }

    void setSpecularColor(glm::vec3 & _specular)
    {
        specular = _specular;
        isDirty = true;
    }

    glm::vec3 getDirection()
    {
        return direction;
    }

    glm::vec3 getIntensity()
    {
        return intensity;
    }

    glm::vec3 getAmbientColor()
    {
        return ambient;
    }

    glm::vec3 getDiffuseColor()
    {
        return diffuse;
    }

    glm::vec3 getSpecularColor()
    {
        return specular;
    }

private:
    glm::vec3 direction;
    glm::vec3 intensity;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    bool isDirty;
};

