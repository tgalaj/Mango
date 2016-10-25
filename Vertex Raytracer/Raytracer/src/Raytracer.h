#pragma once

#include <glm/glm.hpp>

#include "Scene.h"

class Raytracer
{
public:
    explicit Raytracer(Scene& scene);
    ~Raytracer();

    void setBackgroundColor(float r, float g, float b) { m_background_color = glm::vec3(r, g, b); }

    glm::vec3 illuminate(int x_idx, int y_idx, Ray* reflection_ray = nullptr);

private:
    bool checkSpheresIntersections(const Ray& ray);
    bool checkTrianglesIntersections(const Ray& ray);

    void calcShadows(const glm::vec3& hit_pos) const;
    glm::vec3 calcLight(const Ray& ray, const glm::vec3& hit_pos);
    glm::vec3 calcReflections(const Ray& ray, const glm::vec3& hit_pos);

    Material  m_objects_material;
    glm::mat4 m_objects_transformation;
    glm::vec3 m_objects_normal;
    glm::vec3 m_objects_normal_not_transformed;
    glm::vec3 m_objects_intersect_point;

    glm::vec3 m_background_color;
    int m_recurrence_depth;

    float m_max_distance;
    float m_distance;

    Scene* m_scene;

    /* Helper array for determining if a light is visible or not from the ray's point of view */
    bool* m_light_visibility;

    const int INITIAL_RECURRENCE_DEPTH = 1;
};
