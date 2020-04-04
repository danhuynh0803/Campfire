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
#include "ObjectManager.h"

using namespace rapidjson;

extern ObjectManager* objectManagerPtr;

void SceneLoader::LoadScene(ObjectManager& manager, const char* path)
{
    static const char* glObjectFields[] =
    {
        "type"
        "tag",
        "position",
        "rotation",
        "scale",
        "texture",
        "Shader",
        "isActive",
        "isLight"
    };

    FILE* fp = fopen(path, "r");
    if (fp == 0)
    {
        std::cout << "Failed to load file: " << path << '\n';
    }
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    Document document;
    document.ParseStream(is);

    fclose(fp);

    //for (auto& m : document.GetObject())
    //{
    //    //printf("%s\n", m.name.GetString());
    //}
}

void SceneLoader::SaveScene(ObjectManager& manager, const char* path)
{
    Document doc;
    doc.SetObject();
    Value myArray(kArrayType);
    Document::AllocatorType& allocator = doc.GetAllocator();

    for (const auto object : manager.objectList)
    {
        Value objValue;
        objValue.SetObject();
        // Write all fields of GlObject

        objValue.AddMember("type", "CUBE", allocator);

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


        Value texture(object->texture.GetName().c_str(), allocator);
        objValue.AddMember("texture", texture, allocator);

        objValue.AddMember("shader", "generic", allocator);

        objValue.AddMember("isActive", object->isActive, allocator);

        objValue.AddMember("isLight", object->isLight, allocator);

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
