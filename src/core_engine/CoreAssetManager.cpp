#include "core_engine/CoreAssetManager.h"

namespace Vertex
{
    std::vector<GameObject>      CoreAssetManager::m_game_objects;
    std::map<std::string, Model> CoreAssetManager::m_loaded_models;

    std::map<std::string, std::shared_ptr<Texture>> CoreAssetManager::m_loaded_textures;
    std::map<std::string, std::shared_ptr<Shader>>  CoreAssetManager::m_loaded_shaders;

    GameObject CoreAssetManager::createGameObject()
    {
        GameObject game_object;
        m_game_objects.push_back(game_object);

        return game_object;
    }

    std::shared_ptr<Texture> CoreAssetManager::createTexture2D(const std::string& filename, bool is_srgb, GLint base_level)
    {
        if(m_loaded_textures.count(filename))
        {
            return m_loaded_textures[filename];
        }

        auto texture2d = std::make_shared<Texture>();
        texture2d->genTexture2D(filename, base_level, is_srgb);
        m_loaded_textures[filename] = texture2d;

        return texture2d;
    }

    std::shared_ptr<Texture> CoreAssetManager::createCubeMapTexture(const std::string * filenames, bool is_srgb, GLint base_level)
    {
        if (m_loaded_textures.count(filenames[0]))
        {
            return m_loaded_textures[filenames[0]];
        }

        auto texture_cube = std::make_shared<Texture>();
        texture_cube->genCubeMapTexture(filenames, base_level, is_srgb);
        m_loaded_textures[filenames[0]] = texture_cube;

        return texture_cube;
    }

    Model CoreAssetManager::createModel(const std::string& filename)
    {
        if(m_loaded_models.count(filename))
        {
            return m_loaded_models[filename];
        }

        Model model;
        model.load(filename);
        m_loaded_models[filename] = model;

        return m_loaded_models[filename];
    }

    Model CoreAssetManager::createModel()
    {
        Model model;
        return model;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name,
                                                           const std::string& compute_shader_filename)
    {
        if (m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(compute_shader_filename);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name, 
                                                           const std::string& vertex_shader_filename, 
                                                           const std::string& fragment_shader_filename)
    {
        if(m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(vertex_shader_filename, fragment_shader_filename);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name, 
                                                           const std::string& vertex_shader_filename, 
                                                           const std::string& fragment_shader_filename,
                                                           const std::string& geometry_shader_filename)
    {
        if (m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(vertex_shader_filename, 
                                               fragment_shader_filename, 
                                               geometry_shader_filename);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name,
                                                           const std::string& vertex_shader_filename,
                                                           const std::string& fragment_shader_filename, 
                                                           const std::string& tessellation_control_shader_filename, 
                                                           const std::string& tessellation_evaluation_shader_filename)
    {
        if (m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(vertex_shader_filename,
                                               fragment_shader_filename,
                                               tessellation_control_shader_filename,
                                               tessellation_evaluation_shader_filename);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }

    std::shared_ptr<Shader> CoreAssetManager::createShader(const std::string& shader_name, 
                                                           const std::string& vertex_shader_filename, 
                                                           const std::string& fragment_shader_filename, 
                                                           const std::string& geometry_shader_filename, 
                                                           const std::string& tessellation_control_shader_filename, 
                                                           const std::string& tessellation_evaluation_shader_filename)
    {
        if (m_loaded_shaders.count(shader_name))
        {
            return m_loaded_shaders[shader_name];
        }

        auto shader = std::make_shared<Shader>(vertex_shader_filename,
                                               fragment_shader_filename,
                                               geometry_shader_filename,
                                               tessellation_control_shader_filename,
                                               tessellation_evaluation_shader_filename);
        m_loaded_shaders[shader_name]  = shader;

        return shader;
    }
}
