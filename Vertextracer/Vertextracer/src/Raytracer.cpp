#include "Raytracer.h"
#include "Framebuffer.h"

glm::vec3 Raytracer::castRay(const Ray & primary_ray, const Scene & scene, const Options & options, const uint32_t & depth)
{
    if(depth > options.max_depth)
    {
        return options.background_color;
    }

    glm::vec3 hit_color(0.0f);
    IntersectInfo intersect_info = {};

    if (trace(primary_ray, scene, intersect_info))
    {
        glm::vec3 hit_point = primary_ray.m_origin + primary_ray.m_direction * intersect_info.tNear;
        glm::vec3 hit_normal;
        glm::vec2 hit_texcoord;
        intersect_info.hitObject->getSurfaceData(hit_point, 
                                                 primary_ray.m_direction,
                                                 intersect_info.triangle_index, 
                                                 intersect_info.uv, 
                                                 hit_normal,
                                                 hit_texcoord);

        switch(scene.m_objects[intersect_info.parent_index]->m_type)
        {
            case MaterialType::DIFFUSE:
            {
                for(uint32_t i = 0; i < scene.m_lights.size(); ++i)
                {
                    glm::vec3 light_dir, light_intensity;
                    IntersectInfo intersect_info_shading = {};

                    scene.m_lights[i]->illuminate(hit_point, 
                                                  light_dir, 
                                                  light_intensity, 
                                                  intersect_info_shading.tNear);

                    Ray shadow_ray = Ray(hit_point + hit_normal * (0.0f + options.shadow_bias), -light_dir);
                    bool is_visible = !trace(shadow_ray,
                                             scene,
                                             intersect_info_shading,
                                             RayType::SHADOW);

                    /* Checkered pattern */
                    float angle = glm::radians(45.0f);
                    float s = hit_texcoord.x * glm::cos(angle) - hit_texcoord.y * glm::sin(angle);
                    float t = hit_texcoord.y * glm::cos(angle) + hit_texcoord.x * glm::sin(angle);
                    float scale_s = 20.0f, scale_t = 20.0f;
                    float pattern = 1.0f;//(glm::fract(s * scale_s) < 0.5f) ^ (glm::fract(t * scale_t) < 0.5f);

                    if (is_visible)
                    {
                        hit_color += is_visible * pattern * light_intensity * glm::max(0.0f, glm::dot(hit_normal, -light_dir)) * scene.m_objects[intersect_info.parent_index]->m_albedo;
                    }
                    else
                    {
                        hit_color += glm::vec3(0.1f) * pattern * light_intensity * glm::max(0.0f, glm::dot(hit_normal, -light_dir)) * scene.m_objects[intersect_info.parent_index]->m_albedo;;
                    }
                }

                break;
            }
        }
    }
    else
    {
        hit_color = options.background_color;
    }

    return hit_color;
}

bool Raytracer::trace(const Ray & ray, const Scene & scene, IntersectInfo & intersect_info, RayType ray_type)
{
    static const float INF = std::numeric_limits<float>::max();

    intersect_info.hitObject = nullptr;

    for (uint32_t j = 0; j < scene.m_objects.size(); ++j)
    {
        for (uint32_t k = 0; k < scene.m_objects[j]->m_meshes.size(); ++k)
        {
            float t_near_triangle = INF;
            uint32_t index_triangle;
            glm::vec2 uv_triangle;

            if (scene.m_objects[j]->m_meshes[k]->intersect(ray, t_near_triangle, index_triangle, uv_triangle) && t_near_triangle < intersect_info.tNear)
            {
                if(ray_type == RayType::SHADOW )
                {
                    if (scene.m_objects[j]->m_type == MaterialType::REFLECTION_AND_REFRACTION)
                    {
                        continue;
                    }
                    //else
                    {
                        return true;
                    }
                }

                intersect_info.hitObject      = scene.m_objects[j]->m_meshes[k];
                intersect_info.tNear          = t_near_triangle;
                intersect_info.triangle_index = index_triangle;
                intersect_info.parent_index   = j;
                intersect_info.uv             = uv_triangle;
            }
        }
    }

    return intersect_info.hitObject != nullptr;
}
