#include "ResourceManager.h"


#ifdef _WIN32 || _WIN64
std::string ResourceManager::assetPath = "../Assets/";
std::string ResourceManager::corePath = "../Campfire/";
#else
std::string ResourceManager::assetPath = "../../Assets/";
std::string ResourceManager::corePath = "../../Campfire/";
#endif

void ResourceManager::Start()
{

}
