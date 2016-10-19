#include <iostream>

#include <glm/glm.hpp>
#include <FreeImage.h>

#include "Scene.h"

#define BPP 24 //cause three 8 bit RGB values
//#define CMD

glm::vec3 illuminate(Scene * scene, Ray & ray, int depth);

int main(int argc, char * argv[])
{
    Scene * scene = new Scene();

    #ifdef CMD
        if (argc < 2)
        {
            printf("Error: no scene file provided.\n");
            return EXIT_FAILURE;
        }

        scene->loadScene(argv[1]);
    #else
        scene->loadScene("subm/scene5.test");
    #endif

    FreeImage_Initialise();

    /* Raytracing code */
    int width = scene->width;
    int height = scene->height;

    int total_pixels = width * height;
    int pixels_counter = 0;

    FIBITMAP* image = FreeImage_Allocate(width, height, BPP);
    RGBQUAD color;
    
    if (!image)
    {
        printf("ERROR: Could not allocate the image!\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            int depth = 1;

            Ray ray = scene->cam->rayFromCamera(i, j);

            glm::vec3 c = illuminate(scene, ray, depth);

            color.rgbRed   = static_cast<BYTE>(c.r * 255.0f + 0.5f);
            color.rgbGreen = static_cast<BYTE>(c.g * 255.0f + 0.5f);
            color.rgbBlue  = static_cast<BYTE>(c.b * 255.0f + 0.5f);

            FreeImage_SetPixelColor(image, width-j, i, &color);

            ++pixels_counter;
            if (pixels_counter % 1000 == 0)
                printf("%d / %d pixels \r", pixels_counter, total_pixels);
        }
    }

    if (FreeImage_Save(FIF_PNG, image, scene->outputFilename.c_str(), 0))
    {
        printf("Image %s successfully saved!\n", scene->outputFilename.c_str());
    }

    delete scene;

    FreeImage_DeInitialise();
    return EXIT_SUCCESS;
}

glm::vec3 illuminate(Scene * scene, Ray & ray, int depth)
{
    Material currentMat;
    //Ray current_ray(glm::vec3(1.0f), glm::vec3(1.0f));

    glm::vec3 pixel_color(0.0);
    glm::vec3 current_normal;
    glm::vec3 current_normal_not_transformed;
    glm::vec3 current_intersect_point;
    glm::mat4 current_transf;

    float minDistance   = 99999999.0f;
    float d             = minDistance;
    bool isIntersection = false;

#pragma region Spheres
    /* Spheres */
    for (auto & s : scene->spheres)
    {
        /* Get intersections with spheres */
        if (ray.checkIntersection(s, d))
        {
            if (d < minDistance && d > 0.0f)
            {
                isIntersection = true;

                glm::mat4 T = glm::inverse(s.transform);

                glm::vec3 r_dir = glm::vec3(T * glm::vec4(ray.direction, 0.0f));
                glm::vec3 r_origin = glm::vec3(T * glm::vec4(ray.origin, 1.0f));

                currentMat = s.mat;
                current_transf = s.transform;
                //current_ray = ray;
                current_intersect_point = glm::vec3(s.transform * glm::vec4((r_origin + r_dir * d), 1.0f));
                current_normal = glm::transpose(glm::inverse(glm::mat3(s.transform))) * glm::normalize(r_origin + r_dir * d - s.center);
                current_normal_not_transformed = glm::normalize(r_origin + r_dir * d - s.center);

                minDistance = d;
            }
        }
    }
#pragma endregion

#pragma region Triangles
    /* Triangles */
    for (auto & t : scene->triangles)
    {
        /* Get intersections with triangles */
        if (ray.checkIntersection(t, d))
        {
            if (d < minDistance && d > 0.0f)
            {
                isIntersection = true;

                glm::mat4 T = glm::inverse(t.transform);

                glm::vec3 r_dir = glm::vec3(T * glm::vec4(ray.direction, 0.0f));
                glm::vec3 r_origin = glm::vec3(T * glm::vec4(ray.origin, 1.0f));

                currentMat = t.mat;
                current_transf = t.transform;
                //current_ray = ray;
                current_intersect_point = glm::vec3(t.transform * glm::vec4((r_origin + r_dir * d), 1.0f));
                current_normal = glm::transpose(glm::inverse(glm::mat3(t.transform))) * t.n;
                current_normal_not_transformed = t.n;

                minDistance = d;
            }
        }
    }
#pragma endregion

    /* Calculate light */
    if (isIntersection)
    {
        glm::vec3 hit_pos = ray.origin + ray.direction * minDistance;
        pixel_color = currentMat.ambient + currentMat.emission;

#pragma region shadows
        /* Shadows */
        for (auto & l : scene->lights)
        {
            float distance = 999999.0f;
            l.isVisible = true;

            /* Construct ray from intersect point to light */
            glm::vec3 dir = l.dir_pos; //get light direction for directional light

            if (l.type == POINT)
            {
                dir = l.dir_pos - hit_pos;// current_intersect_point;
                distance = glm::distance(l.dir_pos, hit_pos);
            }

            dir = glm::normalize(dir);
            glm::vec3 origin = hit_pos + (dir * 0.00001f);

            Ray sr(origin, dir);

            /* Check if ray intersects any object */
            for (auto & s : scene->spheres)
            {
                float dummy;
                if (sr.checkIntersection(s, dummy))
                {
                    if (dummy >= -0.0005f && dummy < distance)
                    {
                        l.isVisible = false;
                        break;
                    }
                }
            }

            if (l.isVisible)
            {
                for (auto & t : scene->triangles)
                {
                    float dummy;
                    if (sr.checkIntersection(t, dummy))
                    {
                        if (dummy >= -0.0005f && dummy < distance)
                        {
                            l.isVisible = false;
                            break;
                        }
                    }
                }
            }
        }
#pragma endregion

        /* Calculate final color */
        glm::vec3 l_color = glm::vec3(0.0f);
        current_normal = glm::normalize(current_normal);

        for (auto & l : scene->lights)
        {
            if (l.isVisible == true)
            {
                float attenuation = 1.0f;

                glm::vec3 dir_to_light = glm::normalize(l.dir_pos);

                if (l.type == POINT)
                {
                    dir_to_light = glm::normalize(l.dir_pos - hit_pos);
                    float r = glm::distance(l.dir_pos, hit_pos);
                    attenuation = l.light_intensity / (l.attenuation.x + l.attenuation.y * r + l.attenuation.z * r * r);
                }

                glm::vec3 half = glm::normalize(dir_to_light - ray.direction);
                glm::vec3 lambert = l.color * currentMat.diffuse * glm::max(glm::dot(current_normal, dir_to_light), 0.0f);
                glm::vec3 phong = l.color * currentMat.specular * glm::pow(glm::max(glm::dot(current_normal, half), 0.0f), currentMat.shininess);

                l_color += attenuation * (lambert + phong);
            }
            else
            {
                //l_color += glm::vec3(0.2, 0.2, 0.2);//shadow color;
            }
        }

#pragma region recursive_raytrace
        /* Recursive raytracing */
        glm::vec3 r_color(0.0);

        if (currentMat.specular.r > 0.0f ||
            currentMat.specular.g > 0.0f ||
            currentMat.specular.b > 0.0f)
        {
            if (depth < scene->maxdepth)
            {
                /* spawn reflection ray */
                glm::vec3 ref_dir = glm::reflect(ray.direction, current_normal);

                Ray reflection_ray(hit_pos + ref_dir * 0.00001f, glm::normalize(ref_dir));

                /* Compute reflection color */
                r_color += currentMat.specular * illuminate(scene, reflection_ray, depth + 1);
            }
        }
#pragma endregion

        pixel_color += l_color + r_color;

        return pixel_color;
    }
    
    /* return bg color */
    return glm::vec3(0.0f);
}