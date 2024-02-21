#include "mgpch.h"
#include "AssetManager.h"

namespace mango
{
    std::unordered_map<std::string, ref<Font>>       AssetManager::m_loadedFonts;
    std::unordered_map<std::string, ref<Material>>   AssetManager::m_loadedMaterials;
    std::unordered_map<std::string, ref<Shader>>     AssetManager::m_loadedShaders;
    std::unordered_map<std::string, ref<StaticMesh>> AssetManager::m_loadedStaticMeshes;
    std::unordered_map<std::string, ref<Texture>>    AssetManager::m_loadedTextures;

    ref<Font> AssetManager::createFont(const std::string & fontNname, const std::string& filename, GLuint fontHeight)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedFonts.contains(fontNname))
        {
            return m_loadedFonts[fontNname];
        }

        auto font = createRef<Font>(filename, fontHeight);
        m_loadedFonts[fontNname] = font;

        return font;
    }

    ref<Material> AssetManager::createMaterial(const std::string& materialName)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedMaterials.contains(materialName))
        {
            return m_loadedMaterials[materialName];
        }

        auto material = createRef<Material>(materialName);
        m_loadedMaterials[materialName] = material;

        return material;
    }

    ref<Texture> AssetManager::createTexture2D(const std::string& filename, bool isSrgb /*= false*/, GLint numMipmaps /*= 1*/)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_loadedTextures.contains(filename))
        {
            return m_loadedTextures[filename];
        }

        auto extension = std::filesystem::path(filename).extension();

        auto texture2D = createRef<Texture>();

        if (extension == ".dds")
        {
            texture2D->createTextureDDS(filename);
        }
        else
        {
            texture2D->createTexture2d(filename, isSrgb, numMipmaps);
        }
        m_loadedTextures[filename] = texture2D;

        return texture2D;
    }

    ref<Texture> AssetManager::createTexture2D1x1(const std::string& textureName, const glm::uvec4& color)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_loadedTextures.contains(textureName))
        {
            return m_loadedTextures[textureName];
        }

        auto texture2D = createRef<Texture>();
        texture2D->createTexture2d1x1(color);
        m_loadedTextures[textureName] = texture2D;

        return texture2D;
    }

    ref<Texture> AssetManager::createCubeMapTexture(const std::string* filenames, bool isSrgb /*= false*/, GLint numMipmaps /*= 1*/)
    {
        MG_PROFILE_ZONE_SCOPED;

        auto filepath = VFI::getFilepath(filenames[0]);
        std::string filepathString = filepath.parent_path().string();

        if (m_loadedTextures.contains(filepathString))
        {
            return m_loadedTextures[filepathString];
        }

        auto textureCube = createRef<Texture>();
        textureCube->createTextureCubeMap(filenames, isSrgb, numMipmaps);
        m_loadedTextures[filepathString] = textureCube;

        return textureCube;
    }

    ref<StaticMesh> AssetManager::createStaticMesh(const std::string & filename)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_loadedStaticMeshes.contains(filename))
        {
            return m_loadedStaticMeshes[filename];
        }

        auto staticMesh = createRef<StaticMesh>();
        staticMesh->load(filename);
        m_loadedStaticMeshes[filename] = staticMesh;

        return staticMesh;
    }

    ref<Shader> AssetManager::createShader(const std::string & shaderName,
                                           const std::string & computeShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.contains(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = createRef<Shader>(computeShaderFilename);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    ref<Shader> AssetManager::createShader(const std::string & shaderName, 
                                           const std::string & vertexShaderFilename, 
                                           const std::string & fragmentShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.contains(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = createRef<Shader>(vertexShaderFilename, fragmentShaderFilename);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    ref<Shader> AssetManager::createShader(const std::string & shaderName, 
                                           const std::string & vertexShaderFilename, 
                                           const std::string & fragmentShaderFilename,
                                           const std::string & geometryShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.contains(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = createRef<Shader>(vertexShaderFilename, 
                                        fragmentShaderFilename, 
                                        geometryShaderFilename);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    ref<Shader> AssetManager::createShader(const std::string & shaderName,
                                           const std::string & vertexShaderFilename,
                                           const std::string & fragmentShaderFilename, 
                                           const std::string & tessellationControlShaderFilename, 
                                           const std::string & tessellationEvaluationShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.contains(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = createRef<Shader>(vertexShaderFilename,
                                        fragmentShaderFilename,
                                        tessellationControlShaderFilename,
                                        tessellationEvaluationShaderFilename);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    ref<Shader> AssetManager::createShader(const std::string & shaderName, 
                                           const std::string & vertexShaderFilename, 
                                           const std::string & fragmentShaderFilename, 
                                           const std::string & geometryShaderFilename, 
                                           const std::string & tessellationControlShaderFilename, 
                                           const std::string & tessellationEvaluationShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.contains(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = createRef<Shader>(vertexShaderFilename,
                                        fragmentShaderFilename,
                                        geometryShaderFilename,
                                        tessellationControlShaderFilename,
                                        tessellationEvaluationShaderFilename);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    ref<Font> AssetManager::getFont(const std::string& fontName)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedFonts.contains(fontName))
        {
            return m_loadedFonts[fontName];
        }

        return nullptr;
    }

    ref<Material> AssetManager::getMaterial(const std::string& materialName)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedMaterials.contains(materialName))
        {
            return m_loadedMaterials[materialName];
        }

        return nullptr;
    }

    ref<Texture> AssetManager::getTexture2D(const std::string& textureName)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedTextures.contains(textureName))
        {
            return m_loadedTextures[textureName];
        }

        return nullptr;
    }

    ref<Shader> AssetManager::getShader(const std::string& shaderName)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.contains(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        return nullptr;
    }

    ref<StaticMesh> AssetManager::getStaticMesh(const std::string& staticMeshName)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedStaticMeshes.contains(staticMeshName))
        {
            return m_loadedStaticMeshes[staticMeshName];
        }

        return nullptr;
    }

    void AssetManager::unload()
    {
        m_loadedFonts.clear();
        m_loadedMaterials.clear();
        m_loadedShaders.clear();
        m_loadedStaticMeshes.clear();
        m_loadedTextures.clear();
    }

}
