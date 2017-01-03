#include "Raytracer.h"

Raytracer::Raytracer(Scene& scene)
    : m_background_color(glm::vec3(0.0f)),
      m_recurrence_depth(INITIAL_RECURRENCE_DEPTH),
      m_max_distance    (std::numeric_limits<float>::max()),
      m_distance        (m_max_distance),
      m_scene           (&scene)
{
    m_light_visibility = new bool[m_scene->lights.size()];
    std::fill_n(m_light_visibility, m_scene->lights.size(), true);
}

Raytracer::~Raytracer()
{
    if(m_light_visibility != nullptr)
    {
        delete[] m_light_visibility;
    }
}

glm::vec3 Raytracer::illuminate(int x_idx, int y_idx, Ray* reflection_ray)
{
    m_max_distance = std::numeric_limits<float>::max();
    m_distance     = m_max_distance;

    Ray ray = (reflection_ray == nullptr) ? m_scene->cam->rayFromCamera(y_idx, x_idx) : *reflection_ray;

    /* Calculate atmoshperic scattering */
    /** Does the ray intersect the planetory body ? (the intersection test is against the Earth here 
      * not against the atmosphere).If the ray intersects the Earth body and that the intersection is ahead of us,
      * then the ray intersects the planet in 2 points, t0 and t1.But we only want to comupute the atmosphere
      * between t = 0 and t = t0(where the ray hits the Earth first).If the viewing ray doesn't hit the Earth,
      * or course the ray is then bounded to the range [0:INF]. In the method computeIncidentLight() we then
      * compute where this primary ray intersects the atmosphere and we limit the max t range  of the ray to
      * the point where it leaves the atmosphere. 
      */
    if(m_scene->atmosphere != nullptr)
    {
        // 1) Check if ray intersects with a planet
        float t0, t1, tMax = std::numeric_limits<float>::max();
        if (ray.checkIntersection(m_scene->atmosphere->planet_radius, t0, t1) && t1 > 0.0f)
        {
            tMax = glm::max(0.0f, t0);
            m_background_color = glm::vec3(1, 0, 0);
        }
        else
        {
            /* The *viewing or camera ray* is bounded to the range[0:tMax] */
            // 2) Compute atmosphere's incident light
            m_background_color = glm::vec3(1, 0, 1);//m_scene->atmosphere->computeIncidentLight(ray, 0, tMax);
        }
    }

    bool isIntersection = checkSpheresIntersections(ray) | checkTrianglesIntersections(ray);

    /* Calculate light */
    if (isIntersection)
    {
        glm::vec3 hit_pos = ray.origin + ray.direction * m_max_distance;

        calcShadows(hit_pos);

        glm::vec3 light_color      = calcLight      (ray, hit_pos);
        glm::vec3 reflection_color = calcReflections(ray, hit_pos);

        return light_color + reflection_color + m_objects_material.ambient + m_objects_material.emission + m_background_color;
    }

    /* return background color */
    return m_background_color;
}

bool Raytracer::checkSpheresIntersections(const Ray& ray)
{
    bool is_intersection = false;

    /* Spheres */
    for (auto & s : m_scene->spheres)
    {
        /* Get intersections with spheres */
        float distance1;
        if (ray.checkIntersection(s, m_distance, distance1))
        {
            if (m_distance < m_max_distance && m_distance > 0.0f)
            {
                is_intersection = true;

                glm::mat4 T = glm::inverse(s.transform);

                glm::vec3 r_dir    = glm::vec3(T * glm::vec4(ray.direction, 0.0f));
                glm::vec3 r_origin = glm::vec3(T * glm::vec4(ray.origin, 1.0f));

                m_objects_material               = s.mat;
                m_objects_transformation         = s.transform;
                m_objects_intersect_point        = glm::vec3(s.transform * glm::vec4((r_origin + r_dir * m_distance), 1.0f));
                m_objects_normal                 = glm::transpose(glm::inverse(glm::mat3(s.transform))) * glm::normalize(r_origin + r_dir * m_distance - s.center);
                m_objects_normal_not_transformed = glm::normalize(r_origin + r_dir * m_distance - s.center);

                m_max_distance = m_distance;
            }
        }
    }

    return is_intersection;
}

bool Raytracer::checkTrianglesIntersections(const Ray& ray)
{
    bool is_intersection = false;

    /* Triangles */
    for (auto & t : m_scene->triangles)
    {
        /* Get intersections with triangles */
        if (ray.checkIntersection(t, m_distance))
        {
            if (m_distance < m_max_distance && m_distance > 0.0f)
            {
                is_intersection = true;

                glm::mat4 T = glm::inverse(t.transform);

                glm::vec3 r_dir    = glm::vec3(T * glm::vec4(ray.direction, 0.0f));
                glm::vec3 r_origin = glm::vec3(T * glm::vec4(ray.origin, 1.0f));

                m_objects_material               = t.mat;
                m_objects_transformation         = t.transform;
                m_objects_intersect_point        = glm::vec3(t.transform * glm::vec4((r_origin + r_dir * m_distance), 1.0f));
                m_objects_normal                 = glm::transpose(glm::inverse(glm::mat3(t.transform))) * t.n;
                m_objects_normal_not_transformed = t.n;

                m_max_distance = m_distance;
            }
        }
    }

    return is_intersection;
}

glm::vec3 Raytracer::calcLight(const Ray& ray, const glm::vec3& hit_pos)
{
    /* Calculate light color */
    glm::vec3 light_color = glm::vec3(0.0f);

    m_objects_normal = glm::normalize(m_objects_normal);
    
    int light_index = 0;
    for (auto & light : m_scene->lights)
    {
        if (m_light_visibility[light_index])
        {
            auto attenuation = 1.0f;

            glm::vec3 dir_to_light = glm::normalize(light.dir_pos);

            if (light.type == LIGHT_TYPE::POINT)
            {
                dir_to_light = glm::normalize(light.dir_pos - hit_pos);
                auto radius  = glm::distance(light.dir_pos, hit_pos);
                attenuation  = light.light_intensity / (light.attenuation.x + light.attenuation.y * radius + light.attenuation.z * radius * radius);
            }

            glm::vec3 half    = glm::normalize(dir_to_light - ray.direction);
            glm::vec3 lambert = light.color * m_objects_material.diffuse * glm::max(glm::dot(m_objects_normal, dir_to_light), 0.0f);
            glm::vec3 phong   = light.color * m_objects_material.specular * glm::pow(glm::max(glm::dot(m_objects_normal, half), 0.0f), m_objects_material.shininess);

            light_color += attenuation * (lambert + phong);
        }

        light_index++;
    }

    return light_color;
}

void Raytracer::calcShadows(const glm::vec3& hit_pos) const
{
    std::fill_n(m_light_visibility, m_scene->lights.size(), true);

    /* Shadows */
    for (size_t i = 0; i < m_scene->lights.size(); ++i)
    {
        Light* light = &m_scene->lights[i];

        float distance = 999999.0f;

        /* Construct ray from intersect point to light */
        glm::vec3 dir = light->dir_pos; //get light direction for directional light

        if (light->type == LIGHT_TYPE::POINT)
        {
            dir = light->dir_pos - hit_pos;
            distance = glm::distance(light->dir_pos, hit_pos);
        }

        dir = glm::normalize(dir);
        glm::vec3 origin = hit_pos + (dir * 0.00001f);

        Ray shadow_ray(origin, dir);

        /* Check if ray intersects any object */
        for (auto & s : m_scene->spheres)
        {
            float shadow_distance0, shadow_distance1;
            if (shadow_ray.checkIntersection(s, shadow_distance0, shadow_distance1))
            {
                if (shadow_distance0 >= -0.0005f && shadow_distance0 < distance)
                {
                    m_light_visibility[i] = false;
                    break;
                }
            }
        }

        if (m_light_visibility[i])
        {
            for (auto & t : m_scene->triangles)
            {
                float shadow_distance;
                if (shadow_ray.checkIntersection(t, shadow_distance))
                {
                    if (shadow_distance >= -0.0005f && shadow_distance < distance)
                    {
                        m_light_visibility[i] = false;
                        break;
                    }
                }
            }
        }
    }
}

glm::vec3 Raytracer::calcReflections(const Ray& ray, const glm::vec3& hit_pos)
{
    /* Recursive raytracing */
    glm::vec3 reflection_color(0.0);

    if (m_objects_material.specular.r > 0.0f ||
        m_objects_material.specular.g > 0.0f ||
        m_objects_material.specular.b > 0.0f)
    {
        if (m_recurrence_depth < m_scene->maxdepth)
        {
            /* spawn reflection ray */
            glm::vec3 ref_dir = glm::reflect(ray.direction, m_objects_normal);

            Ray reflection_ray(hit_pos + ref_dir * 0.00001f, glm::normalize(ref_dir));

            /* Compute reflection color */
            m_recurrence_depth++;

            glm::vec3 specular = m_objects_material.specular;
            reflection_color += specular * illuminate(-1, -1, &reflection_ray);
        }
    }

    m_recurrence_depth = INITIAL_RECURRENCE_DEPTH;

    return reflection_color;
}
