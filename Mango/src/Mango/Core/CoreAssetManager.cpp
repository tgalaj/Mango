#include "mgpch.h"

#include "CoreAssetManager.h"

namespace mango
{
    std::vector<GameObject>                CoreAssetManager::m_gameObjects;
    std::unordered_map<std::string, Model> CoreAssetManager::m_loadedModels;

    std::unordered_map<std::string, std::shared_ptr<Texture>> CoreAssetManager::m_loadedTextures;
    std::unordered_map<std::string, std::shared_ptr<Shader>>  CoreAssetManager::m_loadedShaders;
    std::unordered_map<std::string, std::shared_ptr<Font>>    CoreAssetManager::m_loadedFonts;

    GameObject CoreAssetManager::createGameObject()
    {
        GameObject gameObject;
        m_gameObjects.push_back(gameObject);

        return gameObject;
    }

    std::shared_ptr<Font> CoreAssetManager::createFont(const std::string & fontNname, const std::filesystem::path& filepath, GLuint fontHeight)
    {        
        if (m_loadedFonts.count(fontNname))
        {
            return m_loadedFonts[fontNname];
        }

        auto font = std::make_shared<Font>(filepath, fontHeight);
        m_loadedFonts[fontNname] = font;

        return font;
    }

    std::shared_ptr<Texture> CoreAssetManager::createTexture2D(const std::filesystem::path & filepath, bool isSrgb /*= false*/, GLint numMipmaps /*= 1*/)
    {
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

    std::shared_ptr<Texture> CoreAssetManager::createTexture2D1x1(const std::string& textureName, const glm::uvec4& color)
    {
        if (m_loadedTextures.count(textureName))
        {
            return m_loadedTextures[textureName];
        }

        auto texture2D = std::make_shared<Texture>();
        texture2D->genTexture2D1x1(color);
        m_loadedTextures[textureName] = texture2D;

        return texture2D;
    }

    std::shared_ptr<Texture> CoreAssetManager::createCubeMapTexture(const std::filesystem::path* filepaths, bool isSrgb /*= false*/, GLint numMipmaps /*= 1*/)
    {
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

    Model CoreAssetManager::createModel(const std::filesystem::path & filepath)
    {
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

    Model CoreAssetManager::createModel()
    {
        Model model;
        return model;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string           & shaderName,
                                                           const std::filesystem::path & computeShaderFilepath)
    {
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(computeShaderFilepath);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string           & shaderName, 
                                                           const std::filesystem::path & vertexShaderFilepath, 
                                                           const std::filesystem::path & fragmentShaderFilepath)
    {
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(vertexShaderFilepath, fragmentShaderFilepath);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string           & shaderName, 
                                                           const std::filesystem::path & vertexShaderFilepath, 
                                                           const std::filesystem::path & fragmentShaderFilepath,
                                                           const std::filesystem::path & geometryShaderFilepath)
    {
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

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string           & shaderName,
                                                           const std::filesystem::path & vertexShaderFilepath,
                                                           const std::filesystem::path & fragmentShaderFilepath, 
                                                           const std::filesystem::path & tessellationControlShaderFilepath, 
                                                           const std::filesystem::path & tessellationEvaluationShaderFilepath)
    {
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

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string           & shaderName, 
                                                           const std::filesystem::path & vertexShaderFilepath, 
                                                           const std::filesystem::path & fragmentShaderFilepath, 
                                                           const std::filesystem::path & geometryShaderFilepath, 
                                                           const std::filesystem::path & tessellationControlShaderFilepath, 
                                                           const std::filesystem::path & tessellationEvaluationShaderFilepath)
    {
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

    std::shared_ptr<Font> CoreAssetManager::getFont(const std::string& fontName)
    {
        if (m_loadedFonts.count(fontName))
        {
            return m_loadedFonts[fontName];
        }

        return nullptr;
    }

    std::shared_ptr<Texture> CoreAssetManager::getTexture2D(const std::string& textureName)
    {
        if (m_loadedTextures.count(textureName))
        {
            return m_loadedTextures[textureName];
        }

        return nullptr;
    }

    std::shared_ptr<Shader> CoreAssetManager::getShader(const std::string& shaderName)
    {
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        return nullptr;
    }
}
