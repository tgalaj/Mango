#include "mgpch.h"
#include "AssetManager.h"

namespace mango
{
    std::unordered_map<std::string, Model>                    AssetManager::m_loadedModels;
    std::unordered_map<std::string, std::shared_ptr<Texture>> AssetManager::m_loadedTextures;
    std::unordered_map<std::string, std::shared_ptr<Shader>>  AssetManager::m_loadedShaders;
    std::unordered_map<std::string, std::shared_ptr<Font>>    AssetManager::m_loadedFonts;

    std::shared_ptr<Font> AssetManager::createFont(const std::string & fontNname, const std::filesystem::path& filepath, GLuint fontHeight)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedFonts.count(fontNname))
        {
            return m_loadedFonts[fontNname];
        }

        auto font = std::make_shared<Font>(filepath, fontHeight);
        m_loadedFonts[fontNname] = font;

        return font;
    }

    std::shared_ptr<Texture> AssetManager::createTexture2D(const std::filesystem::path & filepath, bool isSrgb /*= false*/, GLint numMipmaps /*= 1*/)
    {
        MG_PROFILE_ZONE_SCOPED;
        std::string filepathString = filepath.string();

        if (m_loadedTextures.count(filepathString))
        {
            return m_loadedTextures[filepathString];
        }

        auto texture2D = std::make_shared<Texture>();
        texture2D->genTexture2D(filepath, numMipmaps, isSrgb);
        m_loadedTextures[filepathString] = texture2D;

        return texture2D;
    }

    std::shared_ptr<Texture> AssetManager::createTexture2D1x1(const std::string& textureName, const glm::uvec4& color)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedTextures.count(textureName))
        {
            return m_loadedTextures[textureName];
        }

        auto texture2D = std::make_shared<Texture>();
        texture2D->genTexture2D1x1(color);
        m_loadedTextures[textureName] = texture2D;

        return texture2D;
    }

    std::shared_ptr<Texture> AssetManager::createCubeMapTexture(const std::filesystem::path* filepaths, bool isSrgb /*= false*/, GLint numMipmaps /*= 1*/)
    {
        MG_PROFILE_ZONE_SCOPED;
        std::string filepathString = filepaths[0].parent_path().string();

        if (m_loadedTextures.count(filepathString))
        {
            return m_loadedTextures[filepathString];
        }

        auto textureCube = std::make_shared<Texture>();
        textureCube->genCubeMapTexture(filepaths, numMipmaps, isSrgb);
        m_loadedTextures[filepathString] = textureCube;

        return textureCube;
    }

    Model AssetManager::createModel(const std::filesystem::path & filepath)
    {
        MG_PROFILE_ZONE_SCOPED;
        std::string filepathString = filepath.string();

        if (m_loadedModels.count(filepathString))
        {
            return m_loadedModels[filepathString];
        }

        Model model;
        model.load(filepath);
        m_loadedModels[filepathString] = model;

        return model;
    }

    Model AssetManager::createModel()
    {
        MG_PROFILE_ZONE_SCOPED;
        Model model;
        return model;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName,
                                                           const std::filesystem::path & computeShaderFilepath)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(computeShaderFilepath);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName, 
                                                           const std::filesystem::path & vertexShaderFilepath, 
                                                           const std::filesystem::path & fragmentShaderFilepath)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(vertexShaderFilepath, fragmentShaderFilepath);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName, 
                                                           const std::filesystem::path & vertexShaderFilepath, 
                                                           const std::filesystem::path & fragmentShaderFilepath,
                                                           const std::filesystem::path & geometryShaderFilepath)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(vertexShaderFilepath, 
                                               fragmentShaderFilepath, 
                                               geometryShaderFilepath);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName,
                                                           const std::filesystem::path & vertexShaderFilepath,
                                                           const std::filesystem::path & fragmentShaderFilepath, 
                                                           const std::filesystem::path & tessellationControlShaderFilepath, 
                                                           const std::filesystem::path & tessellationEvaluationShaderFilepath)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(vertexShaderFilepath,
                                               fragmentShaderFilepath,
                                               tessellationControlShaderFilepath,
                                               tessellationEvaluationShaderFilepath);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName, 
                                                           const std::filesystem::path & vertexShaderFilepath, 
                                                           const std::filesystem::path & fragmentShaderFilepath, 
                                                           const std::filesystem::path & geometryShaderFilepath, 
                                                           const std::filesystem::path & tessellationControlShaderFilepath, 
                                                           const std::filesystem::path & tessellationEvaluationShaderFilepath)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(vertexShaderFilepath,
                                               fragmentShaderFilepath,
                                               geometryShaderFilepath,
                                               tessellationControlShaderFilepath,
                                               tessellationEvaluationShaderFilepath);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Font> AssetManager::getFont(const std::string& fontName)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedFonts.count(fontName))
        {
            return m_loadedFonts[fontName];
        }

        return nullptr;
    }

    std::shared_ptr<Texture> AssetManager::getTexture2D(const std::string& textureName)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedTextures.count(textureName))
        {
            return m_loadedTextures[textureName];
        }

        return nullptr;
    }

    std::shared_ptr<Shader> AssetManager::getShader(const std::string& shaderName)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        return nullptr;
    }
}
