#include "Raytracer.h"
#include "Framebuffer.h"

glm::vec3 Raytracer::castRay(const Ray & primary_ray, const Scene & scene, const Options & options, uint32_t depth)
{
    glm::vec3 hit_color = options.background_color;
    float t_near = std::numeric_limits<float>::max();

    glm::vec2 uv;
    uint32_t index = 0;
    Object *hit_object = nullptr; // this is a pointer to the hit object 

    if (trace(primary_ray, scene, t_near, index, uv, &hit_object))
    {
        glm::vec3 hit_point = primary_ray.m_origin + primary_ray.m_direction * t_near;
        glm::vec3 hit_normal;
        glm::vec2 hit_texcoord;
        hit_object->getSurfaceData(hit_point, primary_ray.m_direction, index, uv, hit_normal, hit_texcoord);

        // Use the normal and texture coordinates to shade the hit point.
        // The normal is used to compute a simple facing ratio and the texture coordinate
        // to compute a basic checker board pattern
        float n_dot_view = glm::max(glm::dot(hit_normal, -primary_ray.m_direction), 0.0f);
        const int M = 10;
        
        float checker = (fmod(hit_texcoord.x * M, 1.0f) > 0.5f) ^ (fmod(hit_texcoord.y * M, 1.0f) > 0.5f);
        float c = 0.3f * (1 - checker) + 0.7f * checker;

        hit_color = glm::clamp(glm::vec3(glm::dot(-primary_ray.m_direction, hit_normal)), 0.0f, 1.0f);//glm::vec3(c * n_dot_view); //glm::vec3(uv.x, uv.y, 0.0f);
    }

    return hit_color;
}

bool Raytracer::trace(const Ray & ray, const Scene & scene, float &t_near, uint32_t & index, glm::vec2 & uv, Object **hit_object)
{
    static const float INF = std::numeric_limits<float>::max();

    *hit_object = nullptr;

    for (uint32_t j = 0; j < scene.m_objects.size(); ++j)
    {
        for (uint32_t k = 0; k < scene.m_objects[j]->m_meshes.size(); ++k)
        {
            float t_near_triangle = INF;
            uint32_t index_triangle;
            glm::vec2 uv_triangle;

            if (scene.m_objects[j]->m_meshes[k]->intersect(ray, t_near_triangle, index_triangle, uv_triangle) && t_near_triangle < t_near)
            {
                *hit_object = scene.m_objects[j]->m_meshes[k];
                t_near = t_near_triangle;
                index = index_triangle;
                uv = uv_triangle;
            }
        }
    }

    return *hit_object != nullptr;
}
