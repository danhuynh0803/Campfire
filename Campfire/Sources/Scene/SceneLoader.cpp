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

    SharedPtr<Scene> scene = CreateSharedPtr<Scene>(false);

    const Value& environment = document["Environment"];
    assert(environment.IsArray());
    std::vector<std::string> faces;
    for (SizeType i = 0; i < environment.Size(); ++i)
    {
        std::string face = std::string(environment[i].GetString());
        faces.push_back(face);
    }

    SharedPtr<Skybox> skybox = CreateSharedPtr<Skybox>();
    skybox->Load(faces);
    scene->SetSkybox(skybox);

    const Value& sceneObjects = document["Entities"];
    assert(sceneObjects.IsArray());

    for (Value::ConstValueIterator itr = sceneObjects.Begin(); itr != sceneObjects.End(); ++itr)
    {
        // Tag
        std::string tag = std::string(itr->FindMember("Tag")->value.GetString());
        auto entity = scene->CreateEntity(tag);

        // Transform
        {
            auto foundItr = itr->FindMember("TransformComponent");
            if (foundItr != itr->MemberEnd())
            {
                const Value& comp = foundItr->value;
                {
                    const Value& a = comp.FindMember("Position")->value;
                    assert(a.IsArray());
                    glm::vec3 position = glm::vec3(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble());
                    entity.GetComponent<TransformComponent>().position = position;
                }

                {
                    const Value& a = comp.FindMember("Rotation")->value;
                    assert(a.IsArray());
                    glm::vec3 rotation = glm::vec3(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble());
                    entity.GetComponent<TransformComponent>().rotation = rotation;
                }

                {
                    const Value& a = comp.FindMember("Scale")->value;
                    assert(a.IsArray());
                    glm::vec3 scale = glm::vec3(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble());
                    entity.GetComponent<TransformComponent>().scale = scale;
                }
            }
        }

        // Mesh
        {
            auto foundItr = itr->FindMember("MeshComponent");
            if (foundItr != itr->MemberEnd())
            {
                const Value& comp = foundItr->value;
                const Value& path = comp.FindMember("Mesh")->value;
                entity.AddComponent<MeshComponent>(path.GetString());
            }
        }

        // Light
        {
            auto foundItr = itr->FindMember("LightComponent");
            if (foundItr != itr->MemberEnd())
            {
                const Value& comp = foundItr->value;
                // TODO: Create map of string and enums
                //switch (foundItr->value.GetString())
                //{
                //    LightComponent::LightType type;
                //    case "DIRECTIONAL":
                //        type = LightComponent::LightType::DIRECTIONAL;
                //        break;
                //    case "POINT":
                //        type = LightComponent::LightType::POINT;
                //        break;
                //    case "SPOT":
                //        type = LightComponent::LightType::SPOT;
                //        break;
                //    case "AREA":
                //        type = LightComponent::LightType::AREA;
                //        break;
                //}
                entity.AddComponent<LightComponent>();
                {
                    const Value& a = comp.FindMember("Color")->value;
                    assert(a.IsArray());
                    glm::vec4 color = glm::vec4(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble(), a[3].GetDouble());
                    entity.GetComponent<LightComponent>().color = color;
                }
            }
        }
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
    Document::AllocatorType& allocator = doc.GetAllocator();

    // Environment
    Value skyboxArray(kArrayType);
    auto skybox = scene->GetSkybox();
    std::vector<std::string> facePaths = skybox->GetFacePaths();
    for (int i = 0; i < facePaths.size(); ++i)
    {
        skyboxArray.PushBack(Value().SetString(StringRef(facePaths[i].c_str())), allocator);
    }
    doc.AddMember("Environment", skyboxArray, allocator);

    // Entities
    Value myArray(kArrayType);
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
            Value compValue;
            compValue.SetObject();

            auto position = entity.GetComponent<TransformComponent>().position;
            Value positionValue(kArrayType);
            for (int i = 0; i < 3; ++i)
            {
                positionValue.PushBack(Value().SetDouble(position[i]), allocator);
            }
            compValue.AddMember("Position", positionValue, allocator);

            auto rotation = entity.GetComponent<TransformComponent>().rotation;
            Value rotationValue(kArrayType);
            for (int i = 0; i < 3; ++i)
            {
                rotationValue.PushBack(Value().SetDouble(rotation[i]), allocator);
            }
            compValue.AddMember("Rotation", rotationValue, allocator);

            auto scale = entity.GetComponent<TransformComponent>().scale;
            Value scaleValue(kArrayType);
            for (int i = 0; i < 3; ++i)
            {
                scaleValue.PushBack(Value().SetDouble(scale[i]), allocator);
            }
            compValue.AddMember("Scale", scaleValue, allocator);

            objValue.AddMember("TransformComponent", compValue, allocator);
        }

        if (entity.HasComponent<MeshComponent>())
        {
            Value compValue;
            compValue.SetObject();

            auto mesh = entity.GetComponent<MeshComponent>().mesh;
            Value meshValue(mesh->GetFilePath().c_str(), allocator);
            compValue.AddMember("Mesh", meshValue, allocator);

            // TODO add material info

            objValue.AddMember("MeshComponent", compValue, allocator);
        }

        if (entity.HasComponent<LightComponent>())
        {
            Value lightCompValue;
            lightCompValue.SetObject();

            auto comp = entity.GetComponent<LightComponent>();
            std::string type;
            switch (comp.type)
            {
                case LightComponent::LightType::DIRECTIONAL:
                    type = "DIRECTIONAL";
                    break;
                case LightComponent::LightType::POINT:
                    type = "POINT";
                    break;
                case LightComponent::LightType::SPOT:
                    type = "SPOT";
                    break;
                case LightComponent::LightType::AREA:
                    type = "AREA";
                    break;
            }
            Value lightTypeValue(type.c_str(), allocator);
            lightCompValue.AddMember("Type", lightTypeValue, allocator);

            auto color = entity.GetComponent<LightComponent>().color;
            Value colorValue(kArrayType);
            for (int i = 0; i < 4; ++i)
            {
                colorValue.PushBack(Value().SetDouble(color[i]), allocator);
            }
            lightCompValue.AddMember("Color", colorValue, allocator);

            objValue.AddMember("LightComponent", lightCompValue, allocator);
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

    doc.AddMember("Entities", myArray, allocator);

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    writer.SetFormatOptions(PrettyFormatOptions::kFormatSingleLineArray);
    doc.Accept(writer);

    std::ofstream out(savePath);
    out << buffer.GetString();
    out.close();

    LOG_INFO("Scene {0} has been saved", activeSceneName);
}

void SceneLoader::DeleteScene()
{

}
