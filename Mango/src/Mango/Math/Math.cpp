#pragma once
#include "mgpch.h"
#include "Math.h"

namespace mango::math
{
    void decompose(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
    {
        using namespace glm;

        mat4 mat = (transform);

        scale[0] = length(mat[0]);
        scale[1] = length(mat[1]);
        scale[2] = length(mat[2]);
        
        // orthonormalize the matrix
        mat[0] = glm::normalize(mat[0]);
        mat[1] = glm::normalize(mat[1]);
        mat[2] = glm::normalize(mat[2]);

        rotation[0] = atan2f( mat[1][2], mat[2][2]);
        rotation[1] = atan2f(-mat[0][2], sqrtf(mat[1][2] * mat[1][2] + mat[2][2] * mat[2][2]));
        rotation[2] = atan2f( mat[0][1], mat[0][0]);

        translation[0] = mat[3].x;
        translation[1] = mat[3].y;
        translation[2] = mat[3].z;
    }
}