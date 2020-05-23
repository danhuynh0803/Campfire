#include <iostream>
#include <cstdio>
#include <fstream>

#include "SceneLoader.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"

#include "GlObject.h"
#include "Cube.h"
#include "Quad.h"
#include "Light.h"
#include "ObjectManager.h"
#include "ShaderController.h"
#include "Shared.h"

using namespace rapidjson;

extern ShaderController shaderController;

char* ConvertTypeToString(Geometry geom);
Geometry ConvertStringToType(const char* type);

char* ConvertTypeToString(Geometry geom)
{
    switch (geom)
    {
        case CUBE:
            return "CUBE";
            break;
        case QUAD:
            return "QUAD";
            break;
        case SPHERE:
            return "SPHERE";
            break;
        case LIGHT:
            return "LIGHT";
            break;
        case MODEL:
            return "MODEL";
            break;
        case NONE:
            std::cout << "ERROR: Saving object with no type specified\n";
            return "NONE";
            break;
    }
}

Geometry ConvertStringToType(const char* type)
{
    if (strcmp(type, "CUBE") == 0)
    {
        return CUBE;
    }
    else if (strcmp(type, "QUAD") == 0)
    {
        return QUAD;
    }
    else if (strcmp(type, "SPHERE") == 0)
    {
        return SPHERE;
    }
    else if (strcmp(type, "LIGHT") == 0)
    {
        return LIGHT;
    }
    else if (strcmp(type, "MODEL") == 0)
    {
        return MODEL;
    }

    return NONE;
}

bool IsValidField(std::string field)
{
    return true;
}

void SceneLoader::LoadNewScene(ObjectManager& manager)
{
    std::cout << "Clearing scene\n";
    for (auto objectPtr : manager.objectList)
    {
        delete objectPtr;
    }
    manager.objectList.clear();

    currentScenePath.clear();
    currentSceneFileName.clear();
}

void SceneLoader::LoadScene(ObjectManager& manager, const char* path)
{
    FILE* fp = fopen(path, "r");
    if (fp == 0)
    {
        std::cout << "ERROR: Failed to load SCENE file: " << path << '\n';
    }

    currentScenePath.assign(path);
    currentSceneFileName = currentScenePath.substr(
            currentScenePath.find_last_of('/')+1
    );

    std::cout << "Clearing scene\n";
    for (auto objectPtr : manager.objectList)
    {
        delete objectPtr;
    }
    manager.objectList.clear();

    std::cout << "Loading SCENE file: " << currentScenePath << '\n';
    std::cout << "Loading SCENE file: " << currentSceneFileName << '\n';
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
        GlObject* mesh;
        Geometry type = ConvertStringToType(itr->FindMember("type")->value.GetString());
        switch (type)
        {
            case CUBE: mesh = new Cube(); break;
            case QUAD: mesh = new Quad(); break;
            //case SPHERE: object = new Sphere(); break;
            case LIGHT:
                       mesh = new Light(); break;
            case NONE:
                       std::cout << "ERROR: Loading object with no type specified\n";
                       break;
            default:
                       std::cout << "ERROR (from default): Loading object with no type specified\n";
                       break;
        }
        mesh->type = type;

        gameObject->name = std::string(itr->FindMember("tag")->value.GetString());

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

        Texture texture(itr->FindMember("texture")->value.GetString());
        mesh->texture = texture;

        Shader* shader = shaderController.Get(std::string(itr->FindMember("shader")->value.GetString()));
        mesh->shader = shader;

        gameObject->isActive = itr->FindMember("isActive")->value.GetBool();

        // TODO find a more manageable way of loading this?
        if (mesh->type == LIGHT)
        {
            Shader* lightShader = shaderController.Get(std::string(itr->FindMember("shader")->value.GetString()));
            mesh->shader = lightShader;

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

        gameObject->glObject = mesh;

        manager.Add(gameObject);
    }
}

void SceneLoader::SaveCurrentScene(ObjectManager& manager)
{
    SaveScene(manager, currentScenePath.c_str());
}

void SceneLoader::SaveScene(ObjectManager& manager, const char* path)
{
    // If no scene was currently loaded,
    // then designate this new path as the currently loaded scene
    // This is for cases where users create a new scene
    // and will save that scene
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

        // Write all fields of GlObject
        GlObject* mesh = object->glObject;
        Value tempValueType(ConvertTypeToString(mesh->type), allocator);
        objValue.AddMember("type", tempValueType, allocator);

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
        myArray.PushBack(objValue, allocator);
    }

    doc.AddMember("SceneObjects", myArray, allocator);

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::ofstream out(path);
    out << buffer.GetString();
    out.close();
}

void SceneLoader::DeleteScene()
{

}
