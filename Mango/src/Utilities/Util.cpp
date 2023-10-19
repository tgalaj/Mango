#include "mgpch.h"
#include "Util.h"

#include <fstream>

namespace mango
{
    std::string Util::loadFile(const std::filesystem::path & filepath)
    {
        if (filepath.empty())
        {
            return "";
        }

        std::string fileText;
        std::string line;

        std::ifstream inFile(filepath);

        if (!inFile)
        {
            fprintf(stderr, "Could not open file %s", filepath.string().c_str());
            inFile.close();

            return "";
        }

        while (getline(inFile, line))
        {
            fileText.append(line + "\n");
        }

        inFile.close();

        return fileText;
    }

    std::string Util::loadShaderIncludes(const std::string & shaderCode)
    {
        std::istringstream ss(shaderCode);

        std::string line, newShaderCode;
        std::string includePhrase = "#include";

        while(std::getline(ss, line))
        {
            if(line.substr(0, includePhrase.size()) == includePhrase)
            {
                std::string include_file_name = line.substr(includePhrase.size() + 2, line.size() - includePhrase .size() - 3);
                line = loadFile("assets/shaders/" + include_file_name);
            }

            newShaderCode.append(line + "\n");
        }

        return newShaderCode;
    }


    unsigned char* Util::loadTexture(const std::filesystem::path & filepath, ImageData & imageData)
    {
        int width, height, channelsCount;
        unsigned char* data = stbi_load(filepath.string().c_str(), &width, &height, &channelsCount, 0);

        imageData.width    = width;
        imageData.height   = height;
        imageData.channels = channelsCount;

        return data;
    }
}
