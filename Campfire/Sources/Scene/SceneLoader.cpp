#include <iostream>
#include <cstdio>
#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>

#include "Scene/SceneLoader.h"

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

    activeScenePath.assign(loadPath);
    activeSceneName = activeScenePath.substr(
            activeScenePath.find_last_of('/')+1
    );

    SharedPtr<Scene> loadedScene = CreateSharedPtr<Scene>(false);

    return loadedScene;
    /*
    LOG_TRACE("Clearing scene");
    for (auto objectPtr : manager.objectList)
    {
        delete objectPtr;
    }
    manager.objectList.clear();


    LOG_TRACE("Loading SCENE file: {0}", currentScenePath);
    LOG_TRACE("Loading SCENE file: {0}", currentSceneFileName);
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    Document document;
    document.ParseStream(is);

    assert(document.IsObject());

    fclose(fp);

    const Value& sceneObjects = document["SceneObjects"];
    assert(sceneObjects.IsArray());

    for (Value::ConstValueIterator itr = sceneObjects.Begin(); itr != sceneObjects.End(); ++itr)
    {
        GameObject* gameObject = new GameObject();
        gameObject->isActive = itr->FindMember("isActive")->value.GetBool();
        gameObject->name = std::string(itr->FindMember("tag")->value.GetString());

        //===============================================================================
        // Load mesh
        GlObject* mesh;
        Geometry type = ConvertStringToGeometry(itr->FindMember("type")->value.GetString());
        switch (type)
        {
            case CUBE: mesh = new Cube(); break;
            case QUAD: mesh = new Quad(); break;
            //case SPHERE: object = new Sphere(); break;
            case LIGHT:
                       mesh = new Light(); break;
            case NONE:
            default:
                       LOG_ERROR("Loading object with no type specified from scene file");
                       break;
        }
        mesh->type = type;

        {
            const Value& a = itr->FindMember("position")->value;
            assert(a.IsArray());
            gameObject->position = glm::vec3(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble());
        }

        {
            const Value& a = itr->FindMember("rotation")->value;
            assert(a.IsArray());
            gameObject->rotation = glm::vec3(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble());
        }

        {
            const Value& a = itr->FindMember("scale")->value;
            assert(a.IsArray());
            gameObject->scale = glm::vec3(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble());
        }

        std::string path(itr->FindMember("texture")->value.GetString());
        Texture texture(path.c_str());
        mesh->texture = texture;

        //ShaderOld* shader = shaderController.Get(std::string(itr->FindMember("shader")->value.GetString()));
        //mesh->shader = shader;

        // TODO find a more manageable way of loading this?
        if (mesh->type == LIGHT)
        {
            //ShaderOld* lightShader = shaderController.Get(std::string(itr->FindMember("shader")->value.GetString()));
            //mesh->shader = lightShader;

            {
                const Value& a = itr->FindMember("color")->value;
                assert(a.IsArray());
                static_cast<Light*>(mesh)->color =
                    glm::vec4(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble(), a[3].GetDouble());
            }

            static_cast<Light*>(mesh)->constant = itr->FindMember("constant")->value.GetDouble();
            static_cast<Light*>(mesh)->linear = itr->FindMember("linear")->value.GetDouble();
            static_cast<Light*>(mesh)->quadratic = itr->FindMember("quadratic")->value.GetDouble();
        }
        //===============================================================================

        //===============================================================================
        // Load rigidBody
        RigidBody* rb = new RigidBody();
        rb->mass = itr->FindMember("mass")->value.GetFloat();
        rb->isDynamic = itr->FindMember("isDynamic")->value.GetBool();
        rb->colShape = ConvertStringToColShape(itr->FindMember("shape")->value.GetString());
        rb->InitTransform(gameObject->position, gameObject->rotation, gameObject->scale);
        //===============================================================================

        gameObject->rigidBody = rb;
        gameObject->glObject = mesh;

        manager.Add(gameObject);
    }
    */
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

    /*
    if (!HasSceneLoaded())
    {
        currentScenePath.assign(path);
        currentSceneFileName = currentScenePath.substr(
                currentScenePath.find_last_of('/')+1
        );
    }
    std::cout << "Saving Scene to " << path << '\n';
    Document doc;
    doc.SetObject();
    Value myArray(kArrayType);
    Document::AllocatorType& allocator = doc.GetAllocator();

    for (const auto object : manager.objectList)
    {
        Value objValue;
        objValue.SetObject();

        // Mesh details
        // Write all fields of GlObject
        GlObject* mesh = object->glObject;

        {
            Value tempValueType(ConvertGeometryToString(mesh->type), allocator);
            objValue.AddMember("type", tempValueType, allocator);
        }

        if (mesh->type == LIGHT)
        {
            // Get the string associated with the shader controller map
            objValue.AddMember("shader", "light", allocator);
        }
        else
        {
            objValue.AddMember("shader", "generic", allocator);
        }

        Value tag(object->name.c_str(), allocator);

        objValue.AddMember("tag", tag, allocator);


        Value position(kArrayType);
        for (int i = 0; i < 3; ++i)
        {
            position.PushBack(Value().SetDouble(object->position[i]), allocator);
        }
        objValue.AddMember("position", position, allocator);


        Value rotation(kArrayType);
        for (int i = 0; i < 3; ++i)
        {
            rotation.PushBack(Value().SetDouble(object->rotation[i]), allocator);
        }
        objValue.AddMember("rotation", rotation, allocator);


        Value scale(kArrayType);
        for (int i = 0; i < 3; ++i)
        {
            scale.PushBack(Value().SetDouble(object->scale[i]), allocator);
        }
        objValue.AddMember("scale", scale, allocator);


        Value texture(mesh->texture.GetName().c_str(), allocator);
        objValue.AddMember("texture", texture, allocator);


        objValue.AddMember("isActive", object->isActive, allocator);

        if (mesh->type == LIGHT)
        {
            Light* light = static_cast<Light*>(mesh);
            Value color(kArrayType);
            for (int i = 0; i < 4; ++i)
            {
                color.PushBack(Value().SetDouble(light->color[i]), allocator);
            }
            objValue.AddMember("color", color, allocator);
            objValue.AddMember("constant" , light->constant , allocator);
            objValue.AddMember("linear"   , light->linear   , allocator);
            objValue.AddMember("quadratic", light->quadratic, allocator);
            //Value lightValue;
            //lightValue.SetObject();
        }

        // RigidBody details
        RigidBody* rb = object->rigidBody;
        if (rb != nullptr)
        {
            objValue.AddMember("mass", rb->mass, allocator);
            objValue.AddMember("isDynamic", rb->isDynamic, allocator);
            Value tempValueType(ConvertColShapeToString(rb->colShape), allocator);
            objValue.AddMember("shape", tempValueType, allocator);
        }

        myArray.PushBack(objValue, allocator);
    }

    doc.AddMember("SceneObjects", myArray, allocator);

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::ofstream out(path);
    out << buffer.GetString();
    out.close();

    LOG_INFO("Scene {0} has been saved", currentSceneFileName);
    */
}

void SceneLoader::DeleteScene()
{

}
