#include "mgpch.h"
#include "AssetManager.h"

namespace mango
{
    std::unordered_map<std::string, Model>                    AssetManager::m_loadedModels;
    std::unordered_map<std::string, std::shared_ptr<Texture>> AssetManager::m_loadedTextures;
    std::unordered_map<std::string, std::shared_ptr<Shader>>  AssetManager::m_loadedShaders;
    std::unordered_map<std::string, std::shared_ptr<Font>>    AssetManager::m_loadedFonts;

    std::shared_ptr<Font> AssetManager::createFont(const std::string & fontNname, const std::string& filename, GLuint fontHeight)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedFonts.count(fontNname))
        {
            return m_loadedFonts[fontNname];
        }

        auto font = std::make_shared<Font>(filename, fontHeight);
        m_loadedFonts[fontNname] = font;

        return font;
    }

    std::shared_ptr<Texture> AssetManager::createTexture2D(const std::string & filename, bool isSrgb /*= false*/, GLint numMipmaps /*= 1*/)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_loadedTextures.count(filename))
        {
            return m_loadedTextures[filename];
        }

        auto extension = std::filesystem::path(filename).extension();
        if (extension == ".dds")
        {
            auto textureDDS = std::make_shared<TextureDDS>();
            textureDDS->genTexture(filename);
            m_loadedTextures[filename] = textureDDS;

            return textureDDS;
        }
        else
        {
            auto texture2D = std::make_shared<Texture>();
            texture2D->genTexture2D(filename, numMipmaps, isSrgb);
            m_loadedTextures[filename] = texture2D;

            return texture2D;
        }
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

    std::shared_ptr<Texture> AssetManager::createCubeMapTexture(const std::string* filenames, bool isSrgb /*= false*/, GLint numMipmaps /*= 1*/)
    {
        MG_PROFILE_ZONE_SCOPED;

        auto filepath = VFI::getFilepath(filenames[0]);
        std::string filepathString = filepath.parent_path().string();

        if (m_loadedTextures.count(filepathString))
        {
            return m_loadedTextures[filepathString];
        }

        auto textureCube = std::make_shared<Texture>();
        textureCube->genCubeMapTexture(filenames, numMipmaps, isSrgb);
        m_loadedTextures[filepathString] = textureCube;

        return textureCube;
    }

    Model AssetManager::createModel(const std::string & filename)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_loadedModels.count(filename))
        {
            return m_loadedModels[filename];
        }

        Model model;
        model.load(filename);
        m_loadedModels[filename] = model;

        return model;
    }

    Model AssetManager::createModel()
    {
        MG_PROFILE_ZONE_SCOPED;
        Model model;
        return model;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string & shaderName,
                                                       const std::string & computeShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(computeShaderFilename);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string & shaderName, 
                                                       const std::string & vertexShaderFilename, 
                                                       const std::string & fragmentShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(vertexShaderFilename, fragmentShaderFilename);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string & shaderName, 
                                                       const std::string & vertexShaderFilename, 
                                                       const std::string & fragmentShaderFilename,
                                                       const std::string & geometryShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(vertexShaderFilename, 
                                               fragmentShaderFilename, 
                                               geometryShaderFilename);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string & shaderName,
                                                       const std::string & vertexShaderFilename,
                                                       const std::string & fragmentShaderFilename, 
                                                       const std::string & tessellationControlShaderFilename, 
                                                       const std::string & tessellationEvaluationShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(vertexShaderFilename,
                                               fragmentShaderFilename,
                                               tessellationControlShaderFilename,
                                               tessellationEvaluationShaderFilename);
        m_loadedShaders[shaderName] = shader;

        return shader;
    }

    std::shared_ptr<Shader> AssetManager::createShader(const std::string & shaderName, 
                                                       const std::string & vertexShaderFilename, 
                                                       const std::string & fragmentShaderFilename, 
                                                       const std::string & geometryShaderFilename, 
                                                       const std::string & tessellationControlShaderFilename, 
                                                       const std::string & tessellationEvaluationShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_loadedShaders.count(shaderName))
        {
            return m_loadedShaders[shaderName];
        }

        auto shader = std::make_shared<Shader>(vertexShaderFilename,
                                               fragmentShaderFilename,
                                               geometryShaderFilename,
                                               tessellationControlShaderFilename,
                                               tessellationEvaluationShaderFilename);
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
