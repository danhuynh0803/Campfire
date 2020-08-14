#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

class ResourceManager
{
public:
    void AddResource(const std::string& path);

    //SharedPtr<Audio> GetAudio(const std::string& path);
    SharedPtr<Texture> GetTexture(const std::string& path);
    SharedPtr<Mesh> GetMesh(const std::string& path);
private:

};

#endif // RESOURCE_MANAGER_H
