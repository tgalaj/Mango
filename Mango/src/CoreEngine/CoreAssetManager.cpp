#include "mgpch.h"

#include "CoreEngine/CoreAssetManager.h"

namespace mango
{
    std::vector<GameObject>      CoreAssetManager::m_game_objects;
    std::map<std::string, Model> CoreAssetManager::m_loaded_models;

    std::map<std::string, std::shared_ptr<Texture>> CoreAssetManager::m_loaded_textures;
    std::map<std::string, std::shared_ptr<Shader>>  CoreAssetManager::m_loaded_shaders;
    std::map<std::string, std::shared_ptr<Font>>    CoreAssetManager::m_loaded_fonts;

    GameObject CoreAssetManager::createGameObject()
    {
        GameObject game_object;
        m_game_objects.push_back(game_object);

        return game_object;
    }

    std::shared_ptr<Font> CoreAssetManager::createFont(const std::string & font_name, const std::string& filepathname, GLuint font_height)
    {        
        if(m_loaded_fonts.count(font_name))
        {
            return m_loaded_fonts[font_name];
        }

        auto font = std::make_shared<Font>(filepathname, font_height);
        m_loaded_fonts[font_name] = font;

        return font;
    }

    std::shared_ptr<Texture> CoreAssetManager::createTexture2D(const std::string& filepathname, bool is_srgb, GLint num_mipmaps)
    {
        if(m_loaded_textures.count(filepathname))
        {
            return m_loaded_textures[filepathname];
        }

        auto texture2d = std::make_shared<Texture>();
        texture2d->genTexture2D(filepathname, num_mipmaps, is_srgb);
        m_loaded_textures[filepathname] = texture2d;

        return texture2d;
    }

    std::shared_ptr<Texture> CoreAssetManager::createTexture2D1x1(const std::string& texture_name, const glm::uvec4& color)
    {
        if(m_loaded_textures.count(texture_name))
        {
            return m_loaded_textures[texture_name];
        }

        auto texture2d = std::make_shared<Texture>();
        texture2d->genTexture2D1x1(color);
        m_loaded_textures[texture_name] = texture2d;

        return texture2d;
    }

    std::shared_ptr<Texture> CoreAssetManager::createCubeMapTexture(const std::string * filepathnames, bool is_srgb, GLint num_mipmaps)
    {
        if (m_loaded_textures.count(filepathnames[0]))
        {
            return m_loaded_textures[filepathnames[0]];
        }

        auto texture_cube = std::make_shared<Texture>();
        texture_cube->genCubeMapTexture(filepathnames, num_mipmaps, is_srgb);
        m_loaded_textures[filepathnames[0]] = texture_cube;

        return texture_cube;
    }

    Model CoreAssetManager::createModel(const std::string& filepathname)
    {
        if(m_loaded_models.count(filepathname))
        {
            return m_loaded_models[filepathname];
        }

        Model model;
        model.load(filepathname);
        m_loaded_models[filepathname] = model;

        return m_loaded_models[filepathname];
    }

    Model CoreAssetManager::createModel()
    {
        Model model;
        return model;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name,
                                                           const std::string& compute_shader_filepathname)
    {
        if (m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(compute_shader_filepathname);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name, 
                                                           const std::string& vertex_shader_filepathname, 
                                                           const std::string& fragment_shader_filepathname)
    {
        if(m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(vertex_shader_filepathname, fragment_shader_filepathname);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name, 
                                                           const std::string& vertex_shader_filepathname, 
                                                           const std::string& fragment_shader_filepathname,
                                                           const std::string& geometry_shader_filepathname)
    {
        if (m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(vertex_shader_filepathname, 
                                               fragment_shader_filepathname, 
                                               geometry_shader_filepathname);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name,
                                                           const std::string& vertex_shader_filepathname,
                                                           const std::string& fragment_shader_filepathname, 
                                                           const std::string& tessellation_control_shader_filepathname, 
                                                           const std::string& tessellation_evaluation_shader_filepathname)
    {
        if (m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(vertex_shader_filepathname,
                                               fragment_shader_filepathname,
                                               tessellation_control_shader_filepathname,
                                               tessellation_evaluation_shader_filepathname);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name, 
                                                           const std::string& vertex_shader_filepathname, 
                                                           const std::string& fragment_shader_filepathname, 
                                                           const std::string& geometry_shader_filepathname, 
                                                           const std::string& tessellation_control_shader_filepathname, 
                                                           const std::string& tessellation_evaluation_shader_filepathname)
    {
        if (m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(vertex_shader_filepathname,
                                               fragment_shader_filepathname,
                                               geometry_shader_filepathname,
                                               tessellation_control_shader_filepathname,
                                               tessellation_evaluation_shader_filepathname);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }

    std::shared_ptr<Font> CoreAssetManager::getFont(const std::string& font_name)
    {
        if(m_loaded_fonts.count(font_name))
        {
            return m_loaded_fonts[font_name];
        }

        return nullptr;
    }
    std::shared_ptr<Texture> CoreAssetManager::getTexture2D(const std::string& texture_name)
    {
        if(m_loaded_textures.count(texture_name))
        {
            return m_loaded_textures[texture_name];
        }

        return nullptr;
    }

    std::shared_ptr<Shader> CoreAssetManager::getShader(const std::string& shader_name)
    {
        if(m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        return nullptr;
    }
}
