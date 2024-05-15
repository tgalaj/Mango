#include "mgpch.h"
#include "mgpch.h"
#include "TextureImporter.h"

#include "Mango/Project/Project.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYDDSLOADER_IMPLEMENTATION
#include "tinyddsloader.h"

using namespace tinyddsloader;

namespace
{
    struct GLFormat
    {
        DDSFile::DXGIFormat dxgiFormat;
        GLenum              type;
        GLenum              format;
        GLenum              internalFormat;
        glm::ivec4          swizzle;
    };

    bool translateDdsFormat(DDSFile::DXGIFormat fmt, GLFormat* outFormat)
    {
        MG_PROFILE_ZONE_SCOPED;

        static const glm::ivec4 sws[] =
        {
            { GL_RED,  GL_GREEN, GL_BLUE, GL_ALPHA },
            { GL_BLUE, GL_GREEN, GL_RED,  GL_ALPHA },
            { GL_BLUE, GL_GREEN, GL_RED,  GL_ONE   },
            { GL_RED,  GL_GREEN, GL_BLUE, GL_ONE   },
            { GL_RED,  GL_ZERO,  GL_ZERO, GL_ZERO  },
            { GL_RED,  GL_GREEN, GL_ZERO, GL_ZERO  },
        };

        using DXGIFmt = DDSFile::DXGIFormat;
        static const GLFormat formats[] =
        {
            { DXGIFmt::R8G8B8A8_UNorm,      GL_UNSIGNED_BYTE, GL_RGBA,                                  GL_RGBA8UI,                               sws[0] },
            { DXGIFmt::R8G8B8A8_UNorm_SRGB, GL_UNSIGNED_BYTE, GL_RGBA,                                  GL_SRGB8_ALPHA8,                          sws[0] },
            { DXGIFmt::B8G8R8A8_UNorm,      GL_UNSIGNED_BYTE, GL_RGBA,                                  GL_RGBA8UI,                               sws[1] },
            { DXGIFmt::B8G8R8X8_UNorm,      GL_UNSIGNED_BYTE, GL_RGBA,                                  GL_RGBA8UI,                               sws[2] },
            { DXGIFmt::R32G32_Float,        GL_FLOAT,         GL_RG,                                    GL_RG32F,                                 sws[0] },
            { DXGIFmt::R32G32B32A32_Float,  GL_FLOAT,         GL_RGBA,                                  GL_RGBA32F,                               sws[0] },
            { DXGIFmt::BC1_UNorm,           0,                GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,         GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,         sws[0] },
            { DXGIFmt::BC2_UNorm,           0,                GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,         GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,         sws[0] },
            { DXGIFmt::BC3_UNorm,           0,                GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,         GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,         sws[0] },
            { DXGIFmt::BC4_UNorm,           0,                GL_COMPRESSED_RED_RGTC1_EXT,              GL_COMPRESSED_RED_RGTC1_EXT,              sws[0] },
            { DXGIFmt::BC4_SNorm,           0,                GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,       GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,       sws[0] },
            { DXGIFmt::BC5_UNorm,           0,                GL_COMPRESSED_RED_GREEN_RGTC2_EXT,        GL_COMPRESSED_RED_GREEN_RGTC2_EXT,        sws[0] },
            { DXGIFmt::BC5_SNorm,           0,                GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT, GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT, sws[0] },
        };

        for (const auto& format : formats)
        {
            if (format.dxgiFormat == fmt)
            {
                if (outFormat)
                {
                    *outFormat = format;
                }
                return true;
            }
        }

        MG_CORE_ASSERT_MSG(false, "Format not supported.");
        return false;
    }

    bool isDdsCompressed(GLenum fmt)
    {
        MG_PROFILE_ZONE_SCOPED;

        switch (fmt)
        {
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            case GL_COMPRESSED_RED_RGTC1_EXT:
            case GL_COMPRESSED_SIGNED_RED_RGTC1_EXT:
            case GL_COMPRESSED_RED_GREEN_RGTC2_EXT:
            case GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT:
                return true;
            default:
                return false;
        }
    }
}

namespace mango
{

    ref<Texture> TextureImporter::importTexture2D(AssetHandle handle, const AssetMetadata& metadata)
    {
        MG_PROFILE_ZONE_SCOPED;

        return loadTexture2D(Project::getActiveAssetDirectory() / metadata.filepath);
    }

    ref<Texture> TextureImporter::loadTexture2D(const std::filesystem::path& path)
    {
        MG_PROFILE_ZONE_SCOPED;

        /*std::string pathStr = metadata.filepath.string();

        auto filepath = VFI::getFilepath(filename);

        if (flip) stbi_set_flip_vertically_on_load(true);

        int width, height, channelsCount;
        uint8_t* data = stbi_load(filepath.string().c_str(), &width, &height, &channelsCount, 0);

        if (flip) stbi_set_flip_vertically_on_load(false);

        if (data)
        {
            setDescriptor(width, height, channelsCount, isSrgb);
        }

        return data;*/

        return nullptr;
    }

}