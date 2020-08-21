#pragma once
#include "Aphelion/Core/Core.h"
#include <glm/gtc/type_ptr.hpp>
#include <PxPhysicsAPI.h>

/**
 * This file contains common conversion functions from glm to physx math
 */
namespace ap
{
    inline glm::vec2 APHELION_API MakeVec2(const physx::PxVec2& vec) { return glm::make_vec2(&vec.x); }
    inline physx::PxVec2 APHELION_API MakeVec2(const glm::vec2& vec) { return physx::PxVec2(vec.x, vec.y); }
    inline glm::vec3 APHELION_API MakeVec3(const physx::PxVec3& vec) { return glm::make_vec3(&vec.x); }
    inline physx::PxVec3 APHELION_API MakeVec3(const glm::vec3& vec) { return physx::PxVec3(vec.x,vec.y,vec.z); }
    inline glm::vec4 APHELION_API MakeVec4(const physx::PxVec4& vec) { return glm::make_vec4(&vec.x); }
    inline physx::PxVec4 APHELION_API MakeVec4(const glm::vec4& vec) { return physx::PxVec4(vec.r, vec.g, vec.b, vec.a); }

    inline glm::mat4 APHELION_API MakeMat4(const physx::PxMat44& mat) { return glm::make_mat4(mat.front()); }
    /// PxMat44 doesn't take a constant (even tho it copies the values)
    inline physx::PxMat44 APHELION_API MakeMat4(const glm::mat4& mat) { auto m2 = mat; return physx::PxMat44(glm::value_ptr(m2)); }
}