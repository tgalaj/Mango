#include "framework/utilities/Util.h"
#include <fstream>
#include <sstream>

namespace Vertex
{
    std::string Util::loadFile(const std::string & filename)
    {
        if (filename.empty())
        {
            return "";
        }

        std::string filetext;
        std::string line;

        std::ifstream inFile(filename);

        if (!inFile)
        {
            fprintf(stderr, "Could not open file %s", filename.c_str());
            inFile.close();

            return "";
        }

        while (getline(inFile, line))
        {
            filetext.append(line + "\n");
        }

        inFile.close();

        return filetext;
    }

    std::string Util::loadShaderIncludes(const std::string & shader_code)
    {
        std::istringstream ss(shader_code);

        std::string line, new_shader_code = "";
        std::string include_phrase = "#include";

        while(std::getline(ss, line))
        {
            if(line.substr(0, include_phrase.size()) == include_phrase)
            {
                std::string include_file_name = line.substr(include_phrase.size() + 2, line.size() - include_phrase .size() - 3);
                line = loadFile("res/shaders/" + include_file_name);
            }

            new_shader_code.append(line + "\n");
        }

        return new_shader_code;
    }


    unsigned char* Util::loadTexture(const std::string & filename)
    {
        int width, height, nr_channels;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nr_channels, 0);

        return data;
    }
}
