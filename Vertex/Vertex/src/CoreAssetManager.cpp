#include "CoreAssetManager.h"

CoreAssetManager::~CoreAssetManager()
{
}

std::string CoreAssetManager::loadFile(const std::string &filename)
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

std::map<std::string, std::string> CoreAssetManager::loadShaderCode(const std::string &filename)
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