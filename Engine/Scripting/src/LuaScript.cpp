#include "Scene/ScriptableEntity.h"
#include "Core/Log.h"
#include "Scripting/LuaScript.h"
#include "Scripting/Lua/LuaTransform.h"
#include "Scripting/Lua/LuaTag.h"
#include "Scripting/Lua/LuaEntity.h"
#include "Scripting/Lua/LuaInput.h"
#include "Scripting/Lua/LuaRigidbody.h"
#include "Scripting/Lua/LuaAudioSource.h"
#include "Scripting/Lua/LuaVector.h"

void LuaScript::Start()
{
    L = luaL_newstate();
    luaL_openlibs(L); //opens all standard Lua libraries

    /* for objects managed by lua stack(gc and all that)
    char* transformComponetMetaTableName = "TransformComponetMetaTable";
    {luaL_newmetatable(L, transformComponetMetaTableName);
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, LuaTransfrom::LuaTransformTableIndex);//indexing method for the Transfrom table above
    }lua_settable(L, -3); //sets the (meta)table and pop above
    */
    lua_newtable(L);
    {
        lua_pushcfunction(L, LuaVector::LuaVec2);
        lua_setfield(L, -2, "New");
    }
    lua_setglobal(L, "Vec2");

    lua_newtable(L);
    {
        lua_pushcfunction(L, LuaVector::LuaVec3);
        lua_setfield(L, -2, "New");
    }
    lua_setglobal(L, "Vec3");

    lua_newtable(L);
    {
        lua_pushcfunction(L, LuaVector::LuaVec4);
        lua_setfield(L, -2, "New");
    }
    lua_setglobal(L, "Vec4");

    char* vec2MetaTableName = "LuaVec2MetaTable";
    luaL_newmetatable(L, vec2MetaTableName);
    lua_pushstring(L, "__add");
    lua_pushcfunction(L, LuaVector::LuaVec2Add);
    lua_settable(L, -3);
    lua_pushstring(L, "__sub");
    lua_pushcfunction(L, LuaVector::LuaVec2Sub);
    lua_settable(L, -3);

    char* vec3MetaTableName = "LuaVec3MetaTable";
    luaL_newmetatable(L, vec3MetaTableName);
    lua_pushstring(L, "__add");
    lua_pushcfunction(L, LuaVector::LuaVec3Add);
    lua_settable(L, -3);
    lua_pushstring(L, "__sub");
    lua_pushcfunction(L, LuaVector::LuaVec3Sub);
    lua_settable(L, -3);

    char* vec4MetaTableName = "LuaVec4MetaTable";
    luaL_newmetatable(L, vec4MetaTableName);
    lua_pushstring(L, "__add");
    lua_pushcfunction(L, LuaVector::LuaVec4Add);
    lua_settable(L, -3);
    lua_pushstring(L, "__sub");
    lua_pushcfunction(L, LuaVector::LuaVec4Sub);
    lua_settable(L, -3); //sets the (meta)table and pop above

    if(HasComponent<TransformComponent>())
    {
        lua_newtable(L);
        {
            //luaL_setfuncs(L, LuaTransfrom::transformLib, 0);

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::SetEntityPosition, 1);
            lua_setfield(L, -2, "SetPosition");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().euler));
            lua_pushcclosure(L, LuaTransfrom::SetEntityRotation, 1);
            lua_setfield(L, -2, "SetRotation");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::SetEntityScale, 1);
            lua_setfield(L, -2, "SetScale");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::Translate, 1);
            lua_setfield(L, -2, "Translate");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::GetEntityPosition, 1);
            lua_setfield(L, -2, "GetPosition");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().rotation));
            lua_pushcclosure(L, LuaTransfrom::GetEntityRotation, 1);
            lua_setfield(L, -2, "GetRotation");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::GetEntityScale, 1);
            lua_setfield(L, -2, "GetScale");
        }
        lua_setglobal(L, "Transform");
    }

    if(HasComponent<RigidbodyComponent>())
    {
        lua_newtable(L);
        {
            lua_pushcfunction_with_rigidbody(LuaRigidbody::AddVelocity, "AddVelocity");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::GetVelocity, "GetVelocity");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::SetVelocity, "SetVelocity");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::GetMass, "GetMass");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::SetMass, "SetMass");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::GetDrag, "GetDrag");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::SetDrag, "SetDrag");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::UseGravity, "UseGravity");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::FreezePosition, "FreezePosition");
            lua_pushcfunction_with_rigidbody(LuaRigidbody::FreezeRotation, "FreezeRotation");
        }
        lua_setglobal(L, "Rigidbody");
    }

    lua_newtable(L);
    {

    }
    lua_setglobal(L, "Entity"); //name the table entity
    
    lua_newtable(L);
    {
        lua_pushcfunction_with_tag(LuaTag::GetTag,"GetTag");
    }
    lua_setglobal(L, "Tag");//name the table Tag

    lua_newtable(L);
    {
        luaL_setfuncs(L, LuaInput::inputLib, 0);
    }
    lua_setglobal(L, "Input");//name the table Input

    if (HasComponent<AudioComponent>())
    {
        lua_newtable(L);
        {
            lua_pushcfunction_with_audioSource(LuaAudioSource::Play, "Play");
            lua_pushcfunction_with_audioSource(LuaAudioSource::Pause, "Pause");
            lua_pushcfunction_with_audioSource(LuaAudioSource::Stop, "Stop");
        }
        lua_setglobal(L, "AudioSource");
    }


    luaL_dofile(L, filepath.c_str());
    lua_getglobal(L, "Start");
    if (lua_pcall(L, 0, 0, 0) != 0)
    {
        LOG_ERROR("Cannot run Update() within {0}. Error: {1}", filepath, lua_tostring(L, -1));;
        lua_pop(L, 1);
    }
}

void LuaScript::Update(float dt)
{
    luaL_dofile(L, filepath.c_str());
    lua_getglobal(L, "Update");
    lua_pushnumber(L, dt);
    if (lua_pcall(L, 1, 0, 0) != 0)
    {
        LOG_ERROR("Cannot run Update() within {0}. Error: {1}", filepath, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

void LuaScript::Destroy()
{
    lua_close(L);
}

void LuaScript::lua_pushcfunction_with_entity(const lua_CFunction& f, const char* name)
{
    lua_pushlightuserdata(L, &entity);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::lua_pushcfunction_with_rigidbody(const lua_CFunction& f, const char* name)
{
    SharedPtr<Rigidbody> rigidbody = GetComponent<RigidbodyComponent>().rigidbody;
    lua_pushlightuserdata(L, rigidbody.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::lua_pushcfunction_with_tag(const lua_CFunction& f, const char* name)
{
    const char* tag = GetComponent<TagComponent>().tag.c_str();
    lua_pushstring(L, tag);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::lua_pushcfunction_with_audioSource(const lua_CFunction& f, const char* name)
{
    SharedPtr<AudioSource> audioSource = GetComponent<AudioComponent>().audioSource;
    lua_pushlightuserdata(L, audioSource.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}
