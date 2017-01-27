#include "Raytracer.h"
#include "Framebuffer.h"
#include "Light.h"

glm::vec3 Raytracer::castRay(const Ray & primary_ray, const Scene & scene, const Options & options, const uint32_t & depth)
{
    if(depth > options.max_depth)
    {
        return traceAtmosphere(primary_ray, scene, options);
    }

    glm::vec3 hit_color(0.0f);
    IntersectInfo intersect_info = {};
    bool is_hit = false;

    if (trace(primary_ray, scene, intersect_info))
    {
        is_hit = true;
        glm::vec3 hit_point = primary_ray.m_origin + primary_ray.m_direction * intersect_info.tNear;
        glm::vec3 hit_normal;
        glm::vec2 hit_texcoord;
        intersect_info.hitObject->getSurfaceData(hit_point, 
                                                 primary_ray.m_direction,
                                                 intersect_info.triangle_index, 
                                                 intersect_info.uv, 
                                                 hit_normal,
                                                 hit_texcoord,
                                                 scene.m_objects[intersect_info.parent_index]->m_material->use_flat_shading);

        switch(scene.m_objects[intersect_info.parent_index]->m_material->m_type)
        {
            case MaterialType::DIFFUSE:
            {
                glm::vec3 diffuse(0.0f), specular(0.0f);

                for(uint32_t i = 0; i < scene.m_lights.size(); ++i)
                {
                    glm::vec3 light_dir, light_intensity;
                    IntersectInfo intersect_info_shading = {};

                    scene.m_lights[i]->illuminate(hit_point, 
                                                  light_dir, 
                                                  light_intensity, 
                                                  intersect_info_shading.tNear);

                    bool is_visible = !trace(Ray(hit_point + hit_normal * (0.0f + options.shadow_bias), -light_dir),
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
                        diffuse += light_intensity * glm::max(0.0f, glm::dot(hit_normal, -light_dir)) * scene.m_objects[intersect_info.parent_index]->m_material->m_albedo;
                        
                        glm::vec3 reflected = glm::normalize(glm::reflect(light_dir, hit_normal));
                        specular += light_intensity * glm::pow(glm::max(0.0f, glm::dot(reflected, -primary_ray.m_direction)), scene.m_objects[intersect_info.parent_index]->m_material->m_specular_exponent);
                    }
                }

                glm::vec3 ambient = Light::AMBIENT * scene.m_objects[intersect_info.parent_index]->m_material->m_albedo;

                hit_color = ambient  * scene.m_objects[intersect_info.parent_index]->m_material->m_ka +
                            diffuse  * scene.m_objects[intersect_info.parent_index]->m_material->m_kd +
                            specular * scene.m_objects[intersect_info.parent_index]->m_material->m_ks;

                break;
            }
            case MaterialType::REFLECTION:
            {
                glm::vec3 reflection = glm::normalize(glm::reflect(primary_ray.m_direction, hit_normal));
                hit_color += 0.8f * castRay(Ray(hit_point + hit_normal * options.shadow_bias, reflection),
                                            scene,
                                            options,
                                            depth + 1);
                break;
            }
            case MaterialType::REFLECTION_AND_REFRACTION:
            {
                glm::vec3 refraction_color(0.0f), reflection_color(0.0f);

                /* Compute fresnel */
                float kr;
                fresnel(primary_ray.m_direction, hit_normal, scene.m_objects[intersect_info.parent_index]->m_material->m_index_of_refreaction, kr);

                bool is_outside = glm::dot(primary_ray.m_direction, hit_normal) < 0.0f;
                glm::vec3 bias = options.shadow_bias * hit_normal;

                /* Compute refraction if it is not a case of total internal reflection */
                if(kr < 1.0f)
                {
                    glm::vec3 refraction_direction = glm::normalize(refract(primary_ray.m_direction, hit_normal, scene.m_objects[intersect_info.parent_index]->m_material->m_index_of_refreaction));
                    glm::vec3 refraction_origin = is_outside ? hit_point - bias : hit_point + bias;
                    refraction_color = castRay(Ray(refraction_origin, refraction_direction), 
                                               scene, 
                                               options, 
                                               depth + 1);
                }

                glm::vec3 reflection_direction = glm::normalize(glm::reflect(primary_ray.m_direction, hit_normal));
                glm::vec3 reflection_origin = is_outside ? hit_point + bias : hit_point - bias;
                reflection_color = castRay(Ray(reflection_origin, reflection_direction),
                                           scene, 
                                           options, 
                                           depth + 1);

                hit_color += reflection_color * kr + refraction_color * (1.0f - kr);
                break;
            }
        }
    }
    else
    {
        hit_color = traceAtmosphere(primary_ray, scene, options);
    }

    /* Calculate atmosphere color and transmittance */
    glm::vec3 transmittance(0.0f);
    glm::vec3 atmosphere_color(0.0f);
    
    if (is_hit)
    {
        atmosphere_color = traceAtmosphere(primary_ray, scene, options, transmittance, intersect_info.tNear);
    }

    /* Aerial perspective - blending object's color with atmosphere's color*/
    return hit_color * (1.0f - transmittance) + atmosphere_color;
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
                    if (scene.m_objects[j]->m_material->m_type == MaterialType::REFLECTION_AND_REFRACTION)
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

glm::vec3 Raytracer::traceAtmosphere(const Ray & ray, const Scene & scene, const Options & options)
{
    if (scene.atmosphere != nullptr)
    {
        glm::vec3 transmittance(0.0f);
        float t_max = -1.0f;
        return scene.atmosphere->computeColor(ray, transmittance, t_max);
    }
    else
    {
        return options.background_color;
    }
}

glm::vec3 Raytracer::traceAtmosphere(const Ray & ray, const Scene & scene, const Options & options, glm::vec3& transmittance, float& t_max)
{
    if (scene.atmosphere != nullptr)
    {
        return scene.atmosphere->computeColor(ray, transmittance, t_max);
    }
    else
    {
        transmittance = glm::vec3(0.0f);
        return options.background_color;
    }
}

glm::vec3 Raytracer::refract(const glm::vec3 & I, const glm::vec3 & N, const float & ior) const
{
    float cosi = glm::clamp(glm::dot(I, N), -1.0f, 1.0f);
    float etai = 1, etat = ior;
    glm::vec3 n = N;

    if(cosi < 0.0f)
    {
        cosi = -cosi;
    }
    else
    {
        std::swap(etai, etat);
        n = -N;
    }

    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);

    return k < 0.0f ? glm::vec3(0.0f) : eta * I + (eta * cosi - std::sqrtf(k)) * n;
}

void Raytracer::fresnel(const glm::vec3 & I, const glm::vec3 & N, const float & ior, float & kr)
{
    float cosi = glm::clamp(glm::dot(I, N), -1.0f, 1.0f);
    float etai = 1, etat = ior;

    if (cosi > 0.0f)
    {
        std::swap(etai, etat);
    }

    /* Snell's law */
    float sint = etai / etat * std::sqrtf(glm::max(0.0f, 1.0f - cosi * cosi));

    /* Total internall refelction */
    if(sint >= 1.0f)
    {
        kr = 1.0f;
    }
    else
    {
        float cost = std::sqrtf(glm::max(0.0f, 1.0f - sint * sint));
        cosi = std::fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));

        kr = (Rs * Rs + Rp * Rp) / 2.0f;
    }
}
