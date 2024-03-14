#pragma once
#include <unordered_map>

#include "Mango/Rendering/Font.h"
#include "Mango/Rendering/Material.h"
#include "Mango/Rendering/Mesh.h"
#include "Mango/Rendering/Texture.h"

namespace mango
{
    class AssetManager final
    {
    public:
        static ref<Font>       createFont          (const std::string & fontName, const std::string& filename, GLuint fontHeight);
        static ref<Material>   createMaterial      (const std::string & materialName);
        static ref<Mesh>       createMesh          (const std::string & meshName);
        static ref<Mesh>       createMeshFromFile  (const std::string & filename);
        static ref<Texture>    createTexture2D     (const std::string & filename, bool isSrgb = false, GLint numMipmaps = 1);
        static ref<Texture>    createTexture2D1x1  (const std::string & textureName, const glm::uvec4 & color);
        static ref<Texture>    createCubeMapTexture(const std::string * filenames, bool isSrgb = false, GLint numMipmaps = 1);

        static ref<Shader> createShader(const std::string & shaderName,
                                        const std::string & computeShaderFilename);

        static ref<Shader> createShader(const std::string & shaderName,
                                        const std::string & vertexShaderFilename,
                                        const std::string & fragmentShaderFilename);

        static ref<Shader> createShader(const std::string & shaderName,
                                        const std::string & vertexShaderFilename,
                                        const std::string & fragmentShaderFilename,
                                        const std::string & geometryShaderFilename);

        static ref<Shader> createShader(const std::string & shaderName,
                                        const std::string & vertexShaderFilename,
                                        const std::string & fragmentShaderFilename,
                                        const std::string & tessellationControlShaderFilename,
                                        const std::string & tessellationEvaluationShaderFilename);

        static ref<Shader> createShader(const std::string & shaderName,
                                        const std::string & vertexShaderFilename,
                                        const std::string & fragmentShaderFilename,
                                        const std::string & geometryShaderFilename,
                                        const std::string & tessellationControlShaderFilename,
                                        const std::string & tessellationEvaluationShaderFilename);

        static ref<Font>       getFont      (const std::string & fontName);
        static ref<Material>   getMaterial  (const std::string & materialName);
        static ref<Shader>     getShader    (const std::string & shaderName);
        static ref<Mesh>       getMesh      (const std::string & staticMeshName);
        static ref<Texture>    getTexture2D (const std::string & textureName);

        static void initDefaultAssets();
        static void unload();

        static std::unordered_map<std::string, ref<Material>>& getMaterialList()   { return m_loadedMaterials; }
        static std::unordered_map<std::string, ref<Mesh>>&     getStaticMeshList() { return m_loadedStaticMeshes; }

    private:
        AssetManager() {}
        ~AssetManager() {}

        static std::unordered_map<std::string, ref<Font>>     m_loadedFonts;
        static std::unordered_map<std::string, ref<Material>> m_loadedMaterials;
        static std::unordered_map<std::string, ref<Shader>>   m_loadedShaders;
        static std::unordered_map<std::string, ref<Mesh>>     m_loadedStaticMeshes;
        static std::unordered_map<std::string, ref<Texture>>  m_loadedTextures;
    };
}
