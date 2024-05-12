#pragma once

#include "Mango/Core/Base.h"
#include "Mango/Rendering/Mesh.h"

#include <filesystem>

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace mango
{
    class AssimpMeshImporter
    {
    public:
        AssimpMeshImporter()  = delete;
        ~AssimpMeshImporter() = delete;

        static ref<Mesh> load(const std::string& filename);

    protected:
        static bool parseScene   (      ref<Mesh>& mesh, const aiScene*    scene,  const std::filesystem::path& parentDirectory);
        static void loadMeshPart (const aiMesh*    mesh,       VertexData& vertexData);
        static bool loadMaterials(      ref<Mesh>& mesh, const aiScene*    scene, const std::filesystem::path& parentDirectory);

        static bool loadMaterialTextures(const aiScene*               scene, 
                                         const aiMaterial*            material, 
                                               ref<Material>&         mangoMaterial, 
                                               aiTextureType          aiType, 
                                               Material::TextureType  textureType, 
                                         const std::filesystem::path& parentDirectory);

        // For converting between ASSIMP and glm
        static inline glm::vec3 vec3_cast(const aiVector3D&   v) { return glm::vec3(v.x, v.y, v.z); }
        static inline glm::vec2 vec2_cast(const aiVector3D&   v) { return glm::vec2(v.x, v.y); }
        static inline glm::quat quat_cast(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
        static inline glm::mat4 mat4_cast(const aiMatrix4x4&  m) { return glm::transpose(glm::make_mat4(&m.a1)); }
        static inline glm::mat4 mat4_cast(const aiMatrix3x3&  m) { return glm::transpose(glm::make_mat3(&m.a1)); }
    };
}