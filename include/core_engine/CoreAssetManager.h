#pragma once
#include <map>

#include "GameObject.h"
#include "framework/rendering/Texture.h"
#include "framework/rendering/Model.h"
#include "framework/gui/Font.h"

namespace Vertex
{
    class CoreAssetManager final
    {
    public:
        static GameObject createGameObject();

        static std::shared_ptr<Font>    createFont          (const std::string & font_name, const std::string& filepathname, GLuint font_height);
        static std::shared_ptr<Texture> createTexture2D     (const std::string & filepathname,  bool is_srgb = false, GLint base_level = 0);
        static std::shared_ptr<Texture> createCubeMapTexture(const std::string * filepathnames, bool is_srgb = false, GLint base_level = 0);

        static Model createModel(const std::string & filepathname);
        static Model createModel();

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & compute_shader_filepathname);

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & vertex_shader_filepathname,
                                                    const std::string & fragment_shader_filepathname);

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & vertex_shader_filepathname,
                                                    const std::string & fragment_shader_filepathname,
                                                    const std::string & geometry_shader_filepathname);

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & vertex_shader_filepathname,
                                                    const std::string & fragment_shader_filepathname,
                                                    const std::string & tessellation_control_shader_filepathname,
                                                    const std::string & tessellation_evaluation_shader_filepathname);

        static std::shared_ptr<Shader> createShader(const std::string & shader_name,
                                                    const std::string & vertex_shader_filepathname,
                                                    const std::string & fragment_shader_filepathname,
                                                    const std::string & geometry_shader_filepathname,
                                                    const std::string & tessellation_control_shader_filepathname,
                                                    const std::string & tessellation_evaluation_shader_filepathname);

    private:
        CoreAssetManager() {}
        ~CoreAssetManager() {}

        static std::vector<GameObject>      m_game_objects;
        static std::map<std::string, Model> m_loaded_models;

        static std::map<std::string, std::shared_ptr<Texture>> m_loaded_textures;
        static std::map<std::string, std::shared_ptr<Shader>>  m_loaded_shaders;
        static std::map<std::string, std::shared_ptr<Font>>    m_loaded_fonts;
    };
}
