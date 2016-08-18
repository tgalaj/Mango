#pragma once

#include "SceneNode.h"

class PointLight : public SceneNode
{
public:
    friend class Renderer;
    PointLight();
    ~PointLight();

    void setPosition(glm::vec3 & _position)
    {
        position = _position;
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

    void setConstantAttenuation(float ca)
    {
        constantAttenuation = ca;
        isDirty = true;
    }

    void setLinearAttenuation(float la)
    {
        linearAttenuation = la;
        isDirty = true;
    }

    void setQuadraticAttenuation(float qa)
    {
        quadraticAttenuation = qa;
        isDirty = true;
    }

    glm::vec3 getPosition()
    {
        return position;
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

    float getConstantAttenuation()
    {
        return constantAttenuation;
    }

    float getLinearAttenuation()
    {
        return linearAttenuation;
    }

    float getQuadraticAttenuation()
    {
        return quadraticAttenuation;
    }

private:
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;

    bool isDirty;
};

