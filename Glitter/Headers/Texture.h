#ifndef TEXTURE_H
#define TEXTURE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>

#include <string>

class Texture
{
public:
    unsigned int ID;

    Texture() {}

    ~Texture() {}

    Texture(const char* path)
    {
        texturePath = std::string(path);

        // Check that the texture path is an image format with an alpha channel
        // For now just check if its png, then use alpha
        // all else assume nonalpha channel
        hasAlphaChannel = (texturePath.find(".png") != std::string::npos);

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        // Wrapping/Filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Flip texture coordinates on y-axis, since UV for most image software are inverted from how openGL UV coordinates are
        stbi_set_flip_vertically_on_load(true);

        int width, height, nrChannels;
        unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data)
        {
            if (hasAlphaChannel)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "ERROR: Failed to load " << path << std::endl;
        }

        stbi_image_free(data);
    }

private:
    std::string texturePath;
    bool hasAlphaChannel;
};

#endif // TEXTURE_H
