#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include <iostream>
#include <glad/glad.h>

#include <algorithm>

#include "stb_image.h"
#include "Log.h"
#include "ResourceManager.h"

class Texture
{
public:
    unsigned int ID;
    int width, height;
    std::string type = "N/A"; // diffuse/specular/etc (for models with multiple texture maps)

    Texture() {}

    ~Texture() {}

    Texture(const char* path)
    {
        std::string fullPath = std::string(path);
        std::size_t pos = fullPath.rfind("Assets");
        if (pos == std::string::npos)
        {
            LOG_WARN("Please include the file into the Assets directory: {0}", path);
            return;
        }
        // FIXME?
        // hacky way of getting path beyond "Assets/"
        // Only save the location within the Assets dir
        // Since the relative position of Assets dir varies by platform

        texturePath = fullPath.substr(pos+7); // Save off everything after ASSETS for storing in scenes
        // Replace '\' from windows filesystems with '/', which works on all platforms
        std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
        std::string relPath = ASSETS + texturePath;

        glGenTextures(1, &ID);

        int nrChannels;
        unsigned char *data = stbi_load(relPath.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, ID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // TODO create another constructor that allows changing these parameters
            // Wrapping/Filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
            // TODO
            // Flip texture coordinates on y-axis, since UV for most image software are inverted from how openGL UV coordinates are
            stbi_set_flip_vertically_on_load(true);
        }
        else
        {
            LOG_WARN("Failed to load: {0}", relPath);
        }

        stbi_image_free(data);
    }

    std::string GetName() { return texturePath; }

private:
    std::string texturePath; // The relative path within assets folder, excluding "../Assets/"
    bool hasAlphaChannel = false;
};

#endif // TEXTURE_H
