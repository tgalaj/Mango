#include "Raytracer.h"

glm::vec3 Raytracer::castRay(const Ray & primary_ray, const Scene & scene, uint32_t depth)
{
    glm::vec3 hit_color(0.0f);

    const Object *hit_object = nullptr; // this is a pointer to the hit object 
    float t; // this is the intersection distance from the ray origin to the hit point 

    if (trace(primary_ray, scene, t, hit_object))
    {
        glm::vec3 hit_point = primary_ray.m_origin + primary_ray.m_direction * t;
        glm::vec3 normal;
        glm::vec2 texcoord;
        hit_object->getSurfaceData(hit_point, normal, texcoord);

        // Use the normal and texture coordinates to shade the hit point.
        // The normal is used to compute a simple facing ratio and the texture coordinate
        // to compute a basic checker board pattern
        float scale = 4;
        float pattern = (fmodf(texcoord.x * scale, 1) > 0.5) ^ (fmodf(texcoord.y * scale, 1) > 0.5);
        hit_color = glm::max(0.0f, glm::dot(normal, -primary_ray.m_direction)) * glm::mix(hit_object->color, hit_object->color * 0.8f, pattern);
    }

    return hit_color;
}

bool Raytracer::trace(const Ray & ray, const Scene & scene, float & t_near, const Object *& hit_object)
{
    t_near = std::numeric_limits<float>::max();
    auto iter = scene.m_objects.begin();

    for(; iter != scene.m_objects.end(); ++iter)
    {
        float t = std::numeric_limits<float>::max();

        if((*iter)->intersect(ray, t) && t < t_near)
        {
            hit_object = *iter;
            t_near = t;
        }
    }

    return hit_object != nullptr;
}
