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
        Model model;
        return model;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName,
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

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName, 
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

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName, 
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

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName,
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

    std::shared_ptr<Shader> AssetManager::createShader(const std::string           & shaderName, 
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

    std::shared_ptr<Font> AssetManager::getFont(const std::string& fontName)
    {
        if (m_loadedFonts.count(fontName))
        {
            return m_loadedFonts[fontName];
        }

        return nullptr;
    }

    std::shared_ptr<Texture> AssetManager::getTexture2D(const std::string& textureName)
    {
        if (m_loadedTextures.count(textureName))
        {
            return m_loadedTextures[textureName];
        }

        return nullptr;
    }

    std::shared_ptr<Shader> AssetManager::getShader(const std::string& shaderName)
    {
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        return nullptr;
    }
}
