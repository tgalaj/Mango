#include "CoreAssetManager.h"

CoreAssetManager::~CoreAssetManager()
{
}

std::string CoreAssetManager::loadFile(const std::string & filename)
{
    std::string filetext;
    std::string line;

    std::ifstream inFile(filename);

    if (!inFile)
    {
        fprintf(stderr, "Could not open file %s", filename.c_str());
        inFile.close();

        return nullptr;
    }
    else
    {
        while (getline(inFile, line))
        {
            filetext.append(line + "\n");
        }

        inFile.close();

        return filetext;
    }
}

std::map<std::string, std::string> CoreAssetManager::loadShaderCode(const std::string & filename)
{
    std::map<std::string, std::string> codes { 
                                                 { "VS",  "" },
                                                 { "GS",  "" },
                                                 { "TES", "" },
                                                 { "TCS", "" },
                                                 { "FS",  "" },
                                                 { "CS",  "" },
                                             };

    std::regex  shaderName("(#VS|#GS|#TES|#TCS|#FS|#CS)");
    std::string filetext;
    std::string line;
    std::string tmpKey;

    std::ifstream inFile(filename);

    bool hasPrevMatch = false;

    if (!inFile)
    {
        fprintf(stderr, "Could not open file %s", filename.c_str());
        inFile.close();

        return codes;
    }
    else
    {
        while (getline(inFile, line))
        {
            /** If current line contains one of the 
             *  following strings [VS, GS, TES, TCS, FS, CS].
             */
            if (std::regex_match(line, shaderName))
            {
                if (hasPrevMatch)
                {
                    /** When we match the regular expression in a file
                     *  save current shader code that was read
                     *  to a corresponding key in the std::map
                     *  that was previously remembered.
                     */
                    codes[tmpKey] = filetext;
                    filetext      = "";
                    tmpKey        = line.substr(1);
                }
                else
                {
                    /** If it is the first hit just remember the key
                     *  which will be then used to save read shader code
                     *  when regex matches regular expression.
                     */
                    hasPrevMatch = true;
                    tmpKey       = line.substr(1);
                }
            }
            else
            {
                filetext.append(line + "\n");
            }
        }

        /* When we reach end of file, just save the read shader code. */
        codes[tmpKey] = filetext;
        inFile.close();

        return codes;
    }
}

FIBITMAP* CoreAssetManager::loadTexture(const std::string & filename)
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
