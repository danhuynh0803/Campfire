#include <iostream>
#include <cstdio>
#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>

#include "Scene/SceneLoader.h"
#include "Scene/Component.h"

using namespace rapidjson;

std::string SceneLoader::activeScenePath;
std::string SceneLoader::activeSceneName;

SharedPtr<Scene> SceneLoader::LoadNewScene()
{
    activeScenePath.clear();
    activeSceneName.clear();

    SharedPtr<Scene> newScene = CreateSharedPtr<Scene>();
    return newScene;
}

SharedPtr<Scene> SceneLoader::LoadScene(const std::string& loadPath)
{
    FILE* fp = fopen(loadPath.c_str(), "r");
    if (fp == 0)
    {
        LOG_ERROR("Failed to load SCENE file : {0}", loadPath);
    }

    LOG_TRACE("Loading SCENE file: {0}", loadPath);

    activeScenePath.assign(loadPath);
    activeSceneName = activeScenePath.substr(
            activeScenePath.find_last_of('/')+1
    );

    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    Document document;
    document.ParseStream(is);

    assert(document.IsObject());

    fclose(fp);

    const Value& sceneObjects = document["SceneObjects"];
    assert(sceneObjects.IsArray());

    SharedPtr<Scene> scene = CreateSharedPtr<Scene>(false);

    for (Value::ConstValueIterator itr = sceneObjects.Begin(); itr != sceneObjects.End(); ++itr)
    {
        //===============================================================================
        // Load mesh
        {
            const Value& a = itr->FindMember("position")->value;
            assert(a.IsArray());
            //gameObject->position = glm::vec3(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble());
        }

        {
            const Value& a = itr->FindMember("rotation")->value;
            assert(a.IsArray());
            //gameObject->rotation = glm::vec3(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble());
        }

        {
            const Value& a = itr->FindMember("scale")->value;
            assert(a.IsArray());
            //gameObject->scale = glm::vec3(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble());
        }
        //===============================================================================
    }
    return scene;
}

Entity SceneLoader::LoadEntity()
{
    return Entity();
}

void SceneLoader::SaveEntity(const Entity& entity)
{

}


void SceneLoader::SaveCurrentScene(const SharedPtr<Scene>& scene)
{
    SaveScene(scene, activeScenePath.c_str());
}

void SceneLoader::SaveScene(const SharedPtr<Scene>& scene, const std::string& savePath)
{
    // If no scene was currently loaded,
    // then designate this new path as the currently loaded scene
    // This is for cases where users create a new scene
    // and that new scene currently has no name associated with it
    if (!HasSceneLoaded())
    {
        activeScenePath.assign(savePath);
        activeSceneName = activeScenePath.substr(
                activeScenePath.find_last_of('/')+1
        );
    }

    Document doc;
    doc.SetObject();
    Value myArray(kArrayType);
    Document::AllocatorType& allocator = doc.GetAllocator();

    for (auto entityPair : scene->GetEntityMap())
    {
        auto entity = entityPair.second;

        Value objValue;
        objValue.SetObject();

        // TODO
        //objValue.AddMember("isActive", object->isActive, allocator);

        if (entity.HasComponent<IDComponent>())
        {
            //auto ID = entity.GetComponent<IDComponent>().ID;
            //Value idValue(ID);
            //objValue.AddMember("ID", idValue, allocator);
        }

        if (entity.HasComponent<TagComponent>())
        {
            auto tag = entity.GetComponent<TagComponent>().tag;
            Value tagValue(tag.c_str(), allocator);
            objValue.AddMember("Tag", tagValue, allocator);
        }

        if (entity.HasComponent<TransformComponent>())
        {
            auto position = entity.GetComponent<TransformComponent>().position;
            Value positionValue(kArrayType);
            for (int i = 0; i < 3; ++i)
            {
                positionValue.PushBack(Value().SetDouble(position[i]), allocator);
            }
            objValue.AddMember("Position", positionValue, allocator);

            auto rotation = entity.GetComponent<TransformComponent>().rotation;
            Value rotationValue(kArrayType);
            for (int i = 0; i < 3; ++i)
            {
                rotationValue.PushBack(Value().SetDouble(rotation[i]), allocator);
            }
            objValue.AddMember("Rotation", rotationValue, allocator);

            auto scale = entity.GetComponent<TransformComponent>().scale;
            Value scaleValue(kArrayType);
            for (int i = 0; i < 3; ++i)
            {
                scaleValue.PushBack(Value().SetDouble(scale[i]), allocator);
            }
            objValue.AddMember("Scale", scaleValue, allocator);
        }

        if (entity.HasComponent<MeshComponent>())
        {
            auto mesh = entity.GetComponent<MeshComponent>().mesh;
            Value meshValue(mesh->GetFilePath().c_str(), allocator);
            objValue.AddMember("Mesh", meshValue, allocator);
        }

        if (entity.HasComponent<LightComponent>())
        {

        }

        if (entity.HasComponent<RigidbodyComponent>())
        {
            //objValue.AddMember("mass", rb->mass, allocator);
            //objValue.AddMember("isDynamic", rb->isDynamic, allocator);
            //Value tempValueType(ConvertColShapeToString(rb->colShape), allocator);
            //objValue.AddMember("shape", tempValueType, allocator);
        }

        if (entity.HasComponent<ColliderComponent>())
        {

        }

        if (entity.HasComponent<AudioComponent>())
        {

        }

        if (entity.HasComponent<CameraComponent>())
        {

        }

        if (entity.HasComponent<ParticleSystemComponent>())
        {

        }

        if (entity.HasComponent<ScriptComponent>())
        {

        }

        myArray.PushBack(objValue, allocator);
    }

    doc.AddMember("SceneObjects", myArray, allocator);

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::ofstream out(savePath);
    out << buffer.GetString();
    out.close();

    LOG_INFO("Scene {0} has been saved", activeSceneName);
}

void SceneLoader::DeleteScene()
{

}
