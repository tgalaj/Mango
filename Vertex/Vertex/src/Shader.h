#pragma once

#include <GL\glew.h>

#include <map>
#include <string>

class Shader
{
public:
    Shader(std::string const &filename);
    ~Shader();

    bool link();
    void apply();

protected:

private:
    GLuint program_id;
    bool isLinked;
};

