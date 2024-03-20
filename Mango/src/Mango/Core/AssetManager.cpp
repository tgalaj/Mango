#include "mgpch.h"
#include "AssetManager.h"

#include "Mango/Assets/AssimpMeshImporter.h"

namespace mango
{
    std::unordered_map<std::string, ref<Font>>       AssetManager::m_loadedFonts;
    std::unordered_map<std::string, ref<Material>>   AssetManager::m_loadedMaterials;
    std::unordered_map<std::string, ref<Shader>>     AssetManager::m_loadedShaders;
    std::unordered_map<std::string, ref<Mesh>>       AssetManager::m_loadedStaticMeshes;
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
        texture2D->setName(textureName);
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

    ref<Mesh> AssetManager::createMesh(const std::string& meshName)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_loadedStaticMeshes.contains(meshName))
        {
            return m_loadedStaticMeshes[meshName];
        }

        auto staticMesh = createRef<Mesh>(meshName);
        m_loadedStaticMeshes[meshName] = staticMesh;

        return staticMesh;
    }

    ref<Mesh> AssetManager::createMeshFromFile(const std::string & filename)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (m_loadedStaticMeshes.contains(filename))
        {
            return m_loadedStaticMeshes[filename];
        }

        auto staticMesh = AssimpMeshImporter::load(filename);
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

    void AssetManager::initDefaultAssets()
    {
        // Default textures
        auto defaultDiffuse      = createTexture2D1x1("DefaultDiffuse",      glm::uvec4(255, 255, 255, 255));
        auto defaultSpecular     = createTexture2D1x1("DefaultSpecular",     glm::uvec4(0,   0,   0,   255));
        auto defaultNormal       = createTexture2D1x1("DefaultNormal",       glm::uvec4(128, 127, 254, 255));
        auto defaultEmission     = createTexture2D1x1("DefaultEmission",     glm::uvec4(0,   0,   0,   255));
        auto defaultDisplacement = createTexture2D1x1("DefaultDisplacement", glm::uvec4(0,   0,   0,   255));

        // Default material
        auto material = createMaterial("DefaultMaterial");

        // Default meshes
        {
            ref<Mesh> m = createMesh("Capsule");
            m->genCapsule(0.5f, 1.0f, 32, 16, 0);
        }

        {
            ref<Mesh> m = createMesh("Cone");
            m->genCone(1.0f, 0.5f, 12, 20);
        }

        {
            ref<Mesh> m = createMesh("Cube");
            m->genCube(0.5f);
        }

        {
            ref<Mesh> m = createMesh("Cylinder");
            m->genCylinder(1.0f, 0.5f, 20);
        }

        {
            ref<Mesh> m = createMesh("Plane");
            m->genPlane(10);
        }

        {
            ref<Mesh> m = createMesh("Sphere UV");
            m->genSphere(0.5f, 36);
        }

        {
            ref<Mesh> m = createMesh("Sphere");
            m->genSphere(3u);
        }

        {
            ref<Mesh> m = createMesh("Torus");
            m->genTorus(0.5f, 1.0f, 36, 24);
        }

        {
            ref<Mesh> m = createMesh("Torus Knot");
            m->genPQTorusKnot(256, 32, 2, 3);
        }

        {
            ref<Mesh> m = createMesh("Quad");
            m->genQuad();
        }

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

    ref<Mesh> AssetManager::getMesh(const std::string& staticMeshName)
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

        initDefaultAssets();
    }

}
