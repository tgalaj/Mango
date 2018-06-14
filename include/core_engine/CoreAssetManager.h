#pragma once
#include <map>

#include "GameObject.h"
#include "framework/rendering/Texture.h"
#include "framework/rendering/Model.h"

namespace Vertex
{
    class CoreAssetManager final
    {
    public:
        static GameObject createGameObject();

        static std::shared_ptr<Texture> createTexture2D     (const std::string & filename,  bool is_srgb = false, GLint base_level = 0);
        static std::shared_ptr<Texture> createCubeMapTexture(const std::string * filenames, bool is_srgb = false, GLint base_level = 0);

        static Model createModel(const std::string & filename);
        static Model createModel();

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & compute_shader_filename);

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & vertex_shader_filename,
                                                    const std::string & fragment_shader_filename);

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & vertex_shader_filename,
                                                    const std::string & fragment_shader_filename,
                                                    const std::string & geometry_shader_filename);

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & vertex_shader_filename,
                                                    const std::string & fragment_shader_filename,
                                                    const std::string & tessellation_control_shader_filename,
                                                    const std::string & tessellation_evaluation_shader_filename);

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & vertex_shader_filename,
                                                    const std::string & fragment_shader_filename,
                                                    const std::string & geometry_shader_filename,
                                                    const std::string & tessellation_control_shader_filename,
                                                    const std::string & tessellation_evaluation_shader_filename);

    private:
        CoreAssetManager() {}
        ~CoreAssetManager() {}

        static std::vector<GameObject>      m_game_objects;
        static std::map<std::string, Model> m_loaded_models;

        static std::map<std::string, std::shared_ptr<Texture>> m_loaded_textures;
        static std::map<std::string, std::shared_ptr<Shader>>  m_loaded_shaders;
    };
}
