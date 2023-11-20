#pragma once
#include <filesystem>
#include <unordered_map>

#include "Mango/GUI/Font.h"
#include "Mango/Rendering/Model.h"
#include "Mango/Rendering/Texture.h"

namespace mango
{
    class AssetManager final
    {
    public:
        static std::shared_ptr<Font>    createFont          (const std::string  & fontName, const std::filesystem::path& filepath, GLuint fontHeight);
        static std::shared_ptr<Texture> createTexture2D     (const std::filesystem::path & filepath, bool isSrgb = false, GLint numMipmaps = 1);
        static std::shared_ptr<Texture> createTexture2D1x1  (const std::string & textureName,  const glm::uvec4 & color);
        static std::shared_ptr<Texture> createCubeMapTexture(const std::filesystem::path * filepaths, bool isSrgb = false, GLint numMipmaps = 1);

        static Model createModel(const std::filesystem::path & filepath);
        static Model createModel();

        static std::shared_ptr<Shader> createShader(const std::string           & shaderName,
                                                    const std::filesystem::path & computeShaderFilepath);

        static std::shared_ptr<Shader> createShader(const std::string           & shaderName,
                                                    const std::filesystem::path & vertexShaderFilepath,
                                                    const std::filesystem::path & fragmentShaderFilepath);

        static std::shared_ptr<Shader> createShader(const std::string           & shaderName,
                                                    const std::filesystem::path & vertexShaderFilepath,
                                                    const std::filesystem::path & fragmentShaderFilepath,
                                                    const std::filesystem::path & geometryShaderFilepath);

        static std::shared_ptr<Shader> createShader(const std::string           & shaderName,
                                                    const std::filesystem::path & vertexShaderFilepath,
                                                    const std::filesystem::path & fragmentShaderFilepath,
                                                    const std::filesystem::path & tessellationControlShaderFilepath,
                                                    const std::filesystem::path & tessellationEvaluationShaderFilepath);

        static std::shared_ptr<Shader> createShader(const std::string           & shaderName,
                                                    const std::filesystem::path & vertexShaderFilepath,
                                                    const std::filesystem::path & fragmentShaderFilepath,
                                                    const std::filesystem::path & geometryShaderFilepath,
                                                    const std::filesystem::path & tessellationControlShaderFilepath,
                                                    const std::filesystem::path & tessellationEvaluationShaderFilepath);

        static std::shared_ptr<Font>    getFont     (const std::string & fontName);
        static std::shared_ptr<Texture> getTexture2D(const std::string & textureName);
        static std::shared_ptr<Shader>  getShader   (const std::string & shaderName);

    private:
        AssetManager() {}
        ~AssetManager() {}

        static std::unordered_map<std::string, Model>                    m_loadedModels;
        static std::unordered_map<std::string, std::shared_ptr<Texture>> m_loadedTextures;
        static std::unordered_map<std::string, std::shared_ptr<Shader>>  m_loadedShaders;
        static std::unordered_map<std::string, std::shared_ptr<Font>>    m_loadedFonts;
    };
}
