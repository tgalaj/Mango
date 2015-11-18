#include "Material.h"

unsigned int Material::ID = 0;

Material::Material()
    : diffuse_color        (glm::vec3(1.0f)),
      shininess            (60.0f)
{
    ++ID;
}


Material::~Material()
{
}
