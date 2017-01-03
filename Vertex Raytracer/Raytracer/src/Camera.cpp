#include "Camera.h"

Camera::Camera(size_t width, size_t height)
    : fovy(0), 
      tanFovy(0), 
      tanFovx(0)
{
    img_width = width;
    img_height = height;
    aspect = static_cast<float>(width) / static_cast<float>(height);
}


Camera::~Camera()
{
}

void Camera::update()
{
    n = glm::normalize(eye - lookAt);
    
    u = glm::normalize(glm::cross(-up, n));

    v = glm::cross(n, u);

    tanFovy = glm::tan(glm::radians(fovy) * 0.5f);
    tanFovx = aspect * tanFovy;
}

Ray Camera::rayFromCamera(int i, int j) const
{    
    float ii = i + 0.5f;
    float jj = j - 0.5f;
    
    float a = tanFovx * ((jj - (img_width  * 0.5f)) / (img_width  * 0.5f));
    float b = tanFovy * (((img_height * 0.5f) - ii) / (img_height * 0.5f));
   
    glm::vec3 s = glm::normalize(u * a + v * b - n);
   
    return Ray(eye, s);
}

