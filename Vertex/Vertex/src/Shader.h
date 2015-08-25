#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

#include <map>
#include <string>

class Shader
{
public:
    Shader(std::string const &filename);
    ~Shader();

    bool link();
    void apply();

    void setUniformMatrix4fv(std::string uniformName, glm::mat4 &matrix);

protected:

private:
    GLuint program_id;
    bool isLinked;
};

