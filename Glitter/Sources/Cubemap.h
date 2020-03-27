#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include <string>
#include <vector>

class Cubemap
{
public:

    unsigned int ID;

    Cubemap(std::vector<std::string> textures)
    {
        textureFaces = textures;

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

        int width, height, nrChannels;
        unsigned char* data;
        for (GLuint i = 0; i < textureFaces.size(); ++i)
        {
            data = stbi_load(textureFaces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        0, GL_RGB, width, height, 0,
                        GL_RGB, GL_UNSIGNED_BYTE, data
                );
            }
            else
            {
                std::cout << "ERROR: Cubemap texture failed to load: " << textureFaces[i] << '\n';
            }
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

private:
    char* texturePath;
    std::vector<std::string> textureFaces;
};

#endif // CUBE_MAP_H
