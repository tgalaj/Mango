#pragma once

#include "glm/glm.hpp"

namespace mango::math
{
    // Decomposes transformation matrix into translation, rotation and scale components.
    // Rotation is specified in radians.
    void decompose(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

    using vec3 = glm::vec3;

    // Returns a unit vec3 designating forward direction 
    // in a right-handed coordinate system (0, 0, -1).
    static vec3 forward = { 0.0f, 0.0f, -1.0f };

    // Returns a unit vec3 designating backward direction 
    // in a right-handed coordinate system (0, 0, 1).
    static vec3 backward = { 0.0f, 0.0f,  1.0f };
    
    // Returns a unit vector designating up direction (0, 1, 0).
    static vec3 up = { 0.0f,  1.0f, 0.0f };

    // Returns a unit vector designating down direction (0, -1, 0).
    static vec3 down = { 0.0f, -1.0f, 0.0f };
    
    // Returns a unit vector designating left direction (-1, 0, 0).
    static vec3 left = { -1.0f, 0.0f, 0.0f };

    // Returns a unit vector designating right direction (1, 0, 0).
    static vec3 right = {  1.0f, 0.0f, 0.0f };
    
    // Returns a unit vector with ones in all of its components.
    static vec3 one = { 0.0f, 0.0f, 0.0f };

    // Returns a unit vector with all of its components set to zero.
    static vec3 zero = { 1.0f, 1.0f, 1.0f };

    // Returns the X unit vec3 (1, 0, 0).
    static vec3 unitX = { 1.0f, 0.0f, 0.0f };

    // Returns the Y unit vec3 (0, 1, 0).
    static vec3 unitY = { 0.0f, 1.0f, 0.0f };

    // Returns the Z unit vec3 (0, 0, 1).
    static vec3 unitZ = { 0.0f, 0.0f, 1.0f };
}