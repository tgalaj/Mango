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


    FIBITMAP* Util::loadTexture(const std::string & filename)
    {
        /* Image format */
        FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

        /* Pointer to the image */
        FIBITMAP *dib = nullptr;

        /* Check the file signature and deduce its format */
        fif = FreeImage_GetFileType(filename.c_str(), 0);

        /* If still unknown, try to guess the file format from the file extension */
        if (fif == FIF_UNKNOWN)
        {
            fif = FreeImage_GetFIFFromFilename(filename.c_str());
        }

        /* If still unknown, set status to false */
        if (fif == FIF_UNKNOWN)
        {
            fprintf(stderr, "Error while loading texture %s. Can't deduce file format.\n", filename.c_str());
            return nullptr;
        }

        /* Check if FreeImage supports loading the file */
        if (FreeImage_FIFSupportsReading(fif))
        {
            dib = FreeImage_Load(fif, filename.c_str());
        }

        if (!dib)
        {
            fprintf(stderr, "Error while loading texture %s. Vertex Engine doesn't support such files or file name is incorrect.\n", filename.c_str());
            return nullptr;
        }

        return dib;
    }
}
