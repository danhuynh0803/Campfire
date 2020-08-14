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
    const Value& skyboxArray = environment.FindMember("Skybox")->value;
    assert(skyboxArray.IsArray());
    std::vector<std::string> faces;
    for (SizeType i = 0; i < skyboxArray.Size(); ++i)
    {
        std::string face = std::string(skyboxArray[i].GetString());
        faces.push_back(face);
    }
    SharedPtr<Skybox> skybox = CreateSharedPtr<Skybox>();
    skybox->Load(faces);
    scene->SetSkybox(skybox);

    const Value& sceneObjects = document["Entities"];
    assert(sceneObjects.IsArray());

    for (Value::ConstValueIterator itr = sceneObjects.Begin(); itr != sceneObjects.End(); ++itr)
    {
        // ID
        uint64_t ID = itr->FindMember("ID")->value.GetUint64();
        // Tag
        std::string tag = std::string(itr->FindMember("Tag")->value.GetString());
        auto entity = scene->CreateEntity(tag, ID);

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

        // Camera
        {
            auto foundItr = itr->FindMember("CameraComponent");
            if (foundItr != itr->MemberEnd())
            {
                const Value& comp = foundItr->value;
                bool isPerspective = comp.FindMember("IsPerspective")->value.GetBool();
                float vFov = comp.FindMember("VerticalFOV")->value.GetDouble();
                float nearPlane = comp.FindMember("NearPlane")->value.GetDouble();
                float farPlane = comp.FindMember("FarPlane")->value.GetDouble();
                float depth = comp.FindMember("Depth")->value.GetDouble();
                float x = comp.FindMember("X")->value.GetDouble();
                float y = comp.FindMember("Y")->value.GetDouble();
                float width = comp.FindMember("Width")->value.GetDouble();
                float height = comp.FindMember("Height")->value.GetDouble();
                float size = comp.FindMember("Size")->value.GetDouble();

                entity.AddComponent<CameraComponent>();
                auto& camera = entity.GetComponent<CameraComponent>().camera;
                camera->isPerspective = isPerspective;
                camera->vFov = vFov;
                camera->nearPlane = nearPlane;
                camera->farPlane = farPlane;
                camera->depth = depth;
                camera->x = x;
                camera->y = y;
                camera->width = width;
                camera->height = height;
                camera->size = size;
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

        // Rigidbody
        {
            auto foundItr = itr->FindMember("RigidbodyComponent");
            if (foundItr != itr->MemberEnd())
            {
                const Value& comp = foundItr->value;
                entity.AddComponent<RigidbodyComponent>();

                auto& rb = entity.GetComponent<RigidbodyComponent>().rigidbody;

                float mass = comp.FindMember("Mass")->value.GetDouble();
                float drag = comp.FindMember("Drag")->value.GetDouble();
                float angularDrag = comp.FindMember("AngularDrag")->value.GetDouble();

                bool useGravity = comp.FindMember("UseGravity")->value.GetBool();
                bool isDynamic = comp.FindMember("IsDynamic")->value.GetBool();
                bool isKinematic = comp.FindMember("IsKinematic")->value.GetBool();

                rb->mass = mass;
                rb->drag = drag;
                rb->angularDrag = angularDrag;

                rb->useGravity = useGravity;
                rb->isDynamic = isDynamic;
                rb->isKinematic = isKinematic;

                const Value& freezePos = comp.FindMember("FreezePosition")->value;
                for (int i = 0; i < 3; ++i)
                {
                    rb->freezePosition[i] = freezePos[i].GetBool();
                }

                {
                    const Value& freezeRot = comp.FindMember("FreezeRotation")->value;
                    for (int i = 0; i < 3; ++i)
                    {
                        rb->freezeRotation[i] = freezeRot[i].GetBool();
                    }
                }
            }
        }

        // Collider
        {
            // TODO refactor collider component
            auto foundItr = itr->FindMember("ColliderComponent");
            if (foundItr != itr->MemberEnd())
            {
                const Value& comp = foundItr->value;
                entity.AddComponent<ColliderComponent>();

                // Have to load in a specific order until collider comp is refactored
                // Since the collider component stores a ptr to the collider shape,
                // it needs to first know what type it is before intializing the shape
                auto& colliderComp = entity.GetComponent<ColliderComponent>();
                colliderComp.type = static_cast<ColliderComponent::Shape>(comp.FindMember("Type")->value.GetUint());

                // Now that the type is know, we can now initialize the correct collider shape
                colliderComp.InitShape();

                // With shape now initalized, set data that corresponds to that specific shape
                switch (colliderComp.type)
                {
                    case ColliderComponent::Shape::Box:
                    {
                        auto& collider = dynamic_cast<BoxCollider&>(*colliderComp.collider);
                        const Value& sizeArray = comp.FindMember("Size")->value;
                        glm::vec3 size;
                        for (int i = 0; i < 3; ++i)
                        {
                            size[i] = sizeArray[i].GetDouble();
                        }
                        collider.size = size;
                        break;
                    }
                    case ColliderComponent::Shape::Sphere:
                    {
                        auto collider = dynamic_cast<SphereCollider&>(*colliderComp.collider);
                        collider.radius = comp.FindMember("Radius")->value.GetDouble();
                        break;
                    }
                    case ColliderComponent::Shape::Capsule:
                    {
                        auto collider = dynamic_cast<CapsuleCollider&>(*colliderComp.collider);
                        collider.radius = comp.FindMember("Radius")->value.GetDouble();
                        collider.height = comp.FindMember("Height")->value.GetDouble();
                        break;
                    }
                }
            }
        }

        // Light
        {
            auto foundItr = itr->FindMember("LightComponent");
            if (foundItr != itr->MemberEnd())
            {
                const Value& comp = foundItr->value;
                entity.AddComponent<LightComponent>();
                {
                    const Value& type = comp.FindMember("Type")->value;
                    assert(type.IsUint());
                    entity.GetComponent<LightComponent>().type = static_cast<LightComponent::LightType>(type.GetUint());
                }
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
    Value envValue;
    envValue.SetObject();

    Value skyboxArray(kArrayType);
    auto skybox = scene->GetSkybox();
    std::vector<std::string> facePaths = skybox->GetFacePaths();
    for (int i = 0; i < facePaths.size(); ++i)
    {
        skyboxArray.PushBack(Value().SetString(StringRef(facePaths[i].c_str())), allocator);
    }
    envValue.AddMember("Skybox", skyboxArray, allocator);
    doc.AddMember("Environment", envValue, allocator);

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
            uint64_t ID = entity.GetComponent<IDComponent>().ID;
            Value idValue(ID);
            objValue.AddMember("ID", idValue, allocator);
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

        if (entity.HasComponent<CameraComponent>())
        {
            Value compValue;
            compValue.SetObject();

            auto camera = entity.GetComponent<CameraComponent>().camera;
            compValue.AddMember("IsPerspective", camera->isPerspective, allocator);
            compValue.AddMember("VerticalFOV", camera->vFov, allocator);
            compValue.AddMember("NearPlane", camera->nearPlane, allocator);
            compValue.AddMember("FarPlane", camera->farPlane, allocator);
            compValue.AddMember("Depth", camera->depth, allocator);

            compValue.AddMember("X", camera->x, allocator);
            compValue.AddMember("Y", camera->y, allocator);
            compValue.AddMember("Width", camera->width, allocator);
            compValue.AddMember("Height", camera->height, allocator);
            compValue.AddMember("Size", camera->size, allocator);

            objValue.AddMember("CameraComponent", compValue, allocator);
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

        if (entity.HasComponent<RigidbodyComponent>())
        {
            Value compValue;
            compValue.SetObject();

            auto rb = entity.GetComponent<RigidbodyComponent>().rigidbody;

            compValue.AddMember("Mass", rb->mass, allocator);
            compValue.AddMember("Drag", rb->drag, allocator);
            compValue.AddMember("AngularDrag", rb->angularDrag, allocator);
            compValue.AddMember("UseGravity", rb->useGravity, allocator);
            compValue.AddMember("IsDynamic", rb->isDynamic, allocator);
            compValue.AddMember("IsKinematic", rb->isKinematic, allocator);

            Value freezePositionArray(kArrayType);
            for (int i = 0; i < 3; ++i)
            {
                freezePositionArray.PushBack(rb->freezePosition[i], allocator);
            }
            compValue.AddMember("FreezePosition", freezePositionArray, allocator);

            Value freezeRotationArray(kArrayType);
            for (int i = 0; i < 3; ++i)
            {
                freezeRotationArray.PushBack(rb->freezeRotation[i], allocator);
            }
            compValue.AddMember("FreezeRotation", freezeRotationArray, allocator);

            objValue.AddMember("RigidbodyComponent", compValue, allocator);
        }

        if (entity.HasComponent<ColliderComponent>())
        {
            Value compValue;
            compValue.SetObject();

            auto comp = entity.GetComponent<ColliderComponent>();
            ColliderComponent::Shape type = comp.type;

            compValue.AddMember("IsTrigger", comp.collider->isTrigger, allocator);
            Value centerArray(kArrayType);
            for (int i = 0; i < 3; ++i)
            {
                centerArray.PushBack(comp.collider->center[i], allocator);
            }
            compValue.AddMember("Center", centerArray, allocator);

            compValue.AddMember("Type", static_cast<uint32_t>(type), allocator);
            switch (type)
            {
                // Each collider has specific data that is used by that particular shape.
                // E.g. Size for box and radius for spheres
                case ColliderComponent::Shape::Box:
                {
                    auto collider = dynamic_cast<BoxCollider&>(*comp.collider);
                    Value sizeArray(kArrayType);
                    for (int i = 0; i < 3; ++i)
                    {
                        sizeArray.PushBack(collider.size[i], allocator);
                    }
                    compValue.AddMember("Size", sizeArray, allocator);
                    break;
                }
                case ColliderComponent::Shape::Sphere:
                {
                    auto collider = dynamic_cast<SphereCollider&>(*comp.collider);
                    compValue.AddMember("Radius", collider.radius, allocator);
                    break;
                }
                case ColliderComponent::Shape::Capsule:
                {
                    auto collider = dynamic_cast<CapsuleCollider&>(*comp.collider);
                    compValue.AddMember("Radius", collider.radius, allocator);
                    compValue.AddMember("Height", collider.height, allocator);
                    break;
                }
            }

            objValue.AddMember("ColliderComponent", compValue, allocator);
        }

        if (entity.HasComponent<LightComponent>())
        {
            Value lightCompValue;
            lightCompValue.SetObject();

            auto comp = entity.GetComponent<LightComponent>();
            Value lightTypeValue(Value().SetUint(static_cast<uint32_t>(comp.type)), allocator);
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

        if (entity.HasComponent<AudioComponent>())
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

    MessageBox(NULL, activeSceneName.c_str(), TEXT("SaveAs returned..."), MB_OK);

    LOG_INFO("Scene {0} has been saved at {1}", activeSceneName, savePath);
}

void SceneLoader::DeleteScene()
{

}
