#include  <type_traits>

#include "Scene/ScriptableEntity.h"
#include "Scripting/LuaScript.h"
#include "Scripting/Lua/LuaTransform.h"
#include "Scripting/Lua/LuaTag.h"
#include "Scripting/Lua/LuaEntity.h"
#include "Scripting/Lua/LuaInput.h"
#include "Scripting/Lua/LuaRigidbody.h"
#include "Scripting/Lua/LuaAudioSource.h"
#include "Scripting/Lua/LuaVector.h"
#include "Scripting/Lua/LuaCollider.h"
#include "Scripting/Lua/LuaEntity.h"
#include "Core/Log.h"
#include "Physics/Collider.h"


#define LUA_DESTROY_ENTITY "Destroy Entity"

int LuaScriptCallBack::LuaCallback(lua_State* L)
{
    const char* message = lua_tostring(L, 1);
    if (message && strcmp(message, LUA_DESTROY_ENTITY) != 0)
    {
        //replace and push an error message that includes stack trace
        luaL_traceback(L, L, message, 1);
        return 1;
    }
    return 0;
}

static int Log(lua_State* L)
{
    luaL_checkstring(L, -1);
    const char* msg = lua_tostring(L, -1);
    LOG_INFO(msg);
    return 0;
}

void LuaScript::LoadStandardLibries()
{
    luaL_requiref(L, LUA_GNAME, luaopen_base, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_LOADLIBNAME, luaopen_package, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_COLIBNAME, luaopen_coroutine, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_IOLIBNAME, luaopen_io, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_OSLIBNAME, luaopen_os, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_UTF8LIBNAME, luaopen_utf8, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_UTF8LIBNAME, luaopen_debug, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_DBLIBNAME, luaopen_base, 1);
    lua_pop(L, 1);
}

void LuaScript::Start()
{
    L = luaL_newstate();

    if (luaL_dofile(L, filepath.c_str()) != LUA_OK)//check syntax error
    {
        LOG_ERROR("Lua Synatx error within {0}", lua_tostring(L, -1));
        lua_pop(L, 1);
        lua_close(L);
        hasSynataxError = true;
        return;
    }

    lua_pushglobaltable(L);
    LOG_INFO("Non-Nil Global Variables found in {0}", filepath);
    LuaParseTableOnTop("");

    luaL_openlibs(L); //opens all standard Lua libraries

    // Setup logs
    lua_pushcfunction(L, Log);
    lua_setglobal(L, "Log");

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
    
    {
        char* vec2MetaTableName = "LuaVec2MetaTable";
        luaL_newmetatable(L, vec2MetaTableName);
        lua_pushstring(L, "__add");
        lua_pushcfunction(L, LuaVector::LuaVec2Add);
        lua_settable(L, -3);
        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, LuaVector::LuaVec2Sub);
        lua_settable(L, -3);
    }

    {
        char* vec3MetaTableName = "LuaVec3MetaTable";
        luaL_newmetatable(L, vec3MetaTableName);
        lua_pushstring(L, "__add");
        lua_pushcfunction(L, LuaVector::LuaVec3Add);
        lua_settable(L, -3);
        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, LuaVector::LuaVec3Sub);
        lua_settable(L, -3);
    }
    
    {
        char* vec4MetaTableName = "LuaVec4MetaTable";
        luaL_newmetatable(L, vec4MetaTableName);
        lua_pushstring(L, "__add");
        lua_pushcfunction(L, LuaVector::LuaVec4Add);
        lua_settable(L, -3);
        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, LuaVector::LuaVec4Sub);
        lua_settable(L, -3); //sets the (meta)table and pop above
    }
    
    LuaPushComponetTable<TransformComponent>();
    LuaPushComponetTable<RigidbodyComponent>();
    LuaPushComponetTable<AudioSource>();
    LuaPushComponetTable<Colliders>();

    lua_newtable(L);
    {
        lua_pushlightuserdata(L, this);
        lua_pushcclosure(L, LuaEntity::Instantiate, 1);
        lua_setfield(L, -2, "Instantiate");

        lua_pushlightuserdata(L, this);
        lua_pushlightuserdata(L, &entity);
        lua_pushcclosure(L, LuaEntity::EntityDestroy, 2);
        lua_setfield(L, -2, "Destroy");
    }
    lua_setglobal(L, "Entity"); //name the table entity

    lua_newtable(L);
    {
        LuaPushCFunctionWithTag(LuaTag::GetTag,"GetTag");
    }
    lua_setglobal(L, "Tag");//name the table Tag

    lua_newtable(L);
    {
        luaL_setfuncs(L, LuaInput::inputLib, 0);
    }
    lua_setglobal(L, "Input");//name the table Input

    lua_pushcfunction(L, LuaScriptCallBack::LuaCallback);
    if (lua_getglobal(L, "Start"))
    {
        if (lua_pcall(L, 0, 0, -2) != LUA_OK)
        {
            LOG_ERROR("Cannot run Start() within {0}. Error: {1}", filepath, lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

void LuaScript::Update(float dt)
{
    lua_pushnumber(L, dt);
    lua_setglobal(L, "deltatime");
    lua_pushcfunction(L, LuaScriptCallBack::LuaCallback);
    if (lua_getglobal(L, "Update"))
    {
        if (runUpdate)
        {
            if (lua_pcall(L, 0, 0, -2) != LUA_OK)
            {
                LOG_ERROR("Cannot run Update() within {0}. Error: {1}", filepath, lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }
    else
    {
        entity.GetComponent<ScriptComponent>().runUpdate = false;
    }
    lua_pop(L, 1);
}

void LuaScript::Destroy()
{
    lua_close(L);
}

void LuaScript::OnTriggerEnter(Entity other)
{
    if (!other.IsValid()) return;
    lua_pushcfunction(L, LuaScriptCallBack::LuaCallback);
    if (lua_getglobal(L, "OnTriggerEnter"))
    {
        if (runOnTriggerEnter)
        {
            LuaPushEntity(other);
            if (lua_pcall(L, 1, 0, -3) != LUA_OK)
            {
                LOG_ERROR("Cannot run OnTriggerEnter() within {0}. Error: {1}", filepath, lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

void LuaScript::OnTriggerStay(Entity other)
{
    if (!other.IsValid()) return;
    lua_pushcfunction(L, LuaScriptCallBack::LuaCallback);
    if(lua_getglobal(L, "OnTriggerStay"))
    {
        LuaPushEntity(other);
        if (runOnTriggerStay)
        {
            LuaPushEntity(other);
            if (lua_pcall(L, 1, 0, -3) != LUA_OK)
            {
                LOG_ERROR("Cannot run OnTriggerStay() within {0}. Error: {1}", filepath, lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

void LuaScript::OnTriggerExit(Entity other)
{
    if (!other.IsValid()) return;
    lua_pushcfunction(L, LuaScriptCallBack::LuaCallback);
    if (lua_getglobal(L, "OnTriggerExit"))
    {
        LuaPushEntity(other);
        if (runOnTriggerExit)
        {
            LuaPushEntity(other);
            if (lua_pcall(L, 1, 0, -3) != LUA_OK)
            {
                LOG_ERROR("Cannot run OnTriggerExit() within {0}. Error: {1}", filepath, lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

void LuaScript::LuaPushCFunctionWithEntity(const lua_CFunction& f, const char* name)
{
    lua_pushlightuserdata(L, &entity);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithRigidbody(Entity entity, const lua_CFunction& f, const char* name)
{
    SharedPtr<Rigidbody> rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
    lua_pushlightuserdata(L, rigidbody.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithRigidbody(const lua_CFunction& f, const char* name)
{
    SharedPtr<Rigidbody> rigidbody = GetComponent<RigidbodyComponent>().rigidbody;
    lua_pushlightuserdata(L, rigidbody.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithTag(const lua_CFunction& f, const char* name)
{
    const char* tag = GetComponent<TagComponent>().tag.c_str(); lua_pushstring(L, tag);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithAudioSource(const lua_CFunction& f, const char* name)
{
    SharedPtr<AudioSource> audioSource = GetComponent<AudioComponent>().audioSource;
    lua_pushlightuserdata(L, audioSource.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushEntity(Entity entity)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, this);
    lua_pushlightuserdata(L, &entity);
    lua_pushcclosure(L, LuaEntity::OtherEntityDestroy, 2);
    lua_setfield(L, -2, "Destroy");

    if (entity.HasComponent<TagComponent>())
    {
        lua_pushstring(L, entity.GetComponent<TagComponent>().tag.c_str());
        lua_setfield(L, -2, "tag");
    }

    if (entity.HasComponent<TransformComponent>())
    {
        lua_newtable(L);
        {
            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::SetEntityPosition, 1);
            lua_setfield(L, -2, "SetPosition");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().euler));
            lua_pushcclosure(L, LuaTransfrom::SetEntityRotation, 1);
            lua_setfield(L, -2, "SetRotation");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::SetEntityScale, 1);
            lua_setfield(L, -2, "SetScale");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::Translate, 1);
            lua_setfield(L, -2, "Translate");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::GetEntityPosition, 1);
            lua_setfield(L, -2, "GetPosition");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().rotation));
            lua_pushcclosure(L, LuaTransfrom::GetEntityRotation, 1);
            lua_setfield(L, -2, "GetRotation");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::GetEntityScale, 1);
            lua_setfield(L, -2, "GetScale");
        }
        lua_setfield(L, -2, "Transform");
    }

    if (entity.HasComponent<RigidbodyComponent>())
    {
        lua_newtable(L);
        {
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::AddVelocity, "AddVelocity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetVelocity, "GetVelocity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetVelocity, "SetVelocity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetMass, "GetMass");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetMass, "SetMass");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetDrag, "GetDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetDrag, "SetDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::UseGravity, "UseGravity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::FreezePosition, "FreezePosition");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::FreezeRotation, "FreezeRotation");
        }
        lua_setfield(L, -2, "Rigidbody");
    }

    if (entity.HasComponent<AudioComponent>())
    {

    }

    //if (entity.HasComponent<CameraComponent>())
    //{
    //}
    //if (entity.HasComponent<MeshComponent>())
    //{
    //}
    //if (entity.HasComponent<SpriteComponent>())
    //{
    //}
    //if (entity.HasComponent<LightComponent>())
    //{
    //}
    //if (entity.HasComponent<Colliders>())
    //{
    //}
    //if (entity.HasComponent<TextComponent>())
    //{
    //}
    //if (entity.HasComponent<ScriptComponent>())
    //{
    //}
}

template <class T>
void LuaScript::LuaPushComponetTable()
{
    if (HasComponent<T>())
    {
        if (std::is_same<T, TransformComponent>::value)
        {
            lua_newtable(L);
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
            lua_setglobal(L, "Transform");
        }
        else if (std::is_same<T, RigidbodyComponent>::value)
        {
            lua_newtable(L);
            LuaPushCFunctionWithRigidbody(LuaRigidbody::AddVelocity, "AddVelocity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetVelocity, "GetVelocity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetVelocity, "SetVelocity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetMass, "GetMass");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetMass, "SetMass");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetDrag, "GetDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetDrag, "SetDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::UseGravity, "UseGravity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::FreezePosition, "FreezePosition");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::FreezeRotation, "FreezeRotation");
            lua_setglobal(L, "Rigidbody");
        }
        else if (std::is_same<T, AudioComponent>::value)
        {
            lua_newtable(L);
            LuaPushCFunctionWithAudioSource(LuaAudioSource::Play, "Play");
            LuaPushCFunctionWithAudioSource(LuaAudioSource::Pause, "Pause");
            LuaPushCFunctionWithAudioSource(LuaAudioSource::Stop, "Stop");
            lua_setglobal(L, "AudioSource");
        }
        else if (std::is_same<T, BoxCollider>::value)
        {
            lua_newtable(L);
            lua_pushlightuserdata(L, &(GetComponent<BoxCollider>().center));
            lua_pushcclosure(L, LuaCollider::Box::GetCenter, 1);
            lua_setfield(L, -2, "GetCenter");
            lua_setglobal(L, "BoxCollider");
        }
        else if (std::is_same<T, SphereCollider>::value)
        {
            lua_newtable(L);
            lua_pushlightuserdata(L, &(GetComponent<SphereCollider>().center));
            lua_pushcclosure(L, LuaCollider::Sphere::GetCenter, 1);
            lua_setfield(L, -2, "GetCenter");
            lua_setglobal(L, "SphereCollider");
        }
        else if (std::is_same<T, CapsuleCollider>::value)
        {
            lua_newtable(L);
            lua_pushlightuserdata(L, &(GetComponent<CapsuleCollider>().center));
            lua_pushcclosure(L, LuaCollider::Capsule::GetCenter, 1);
            lua_setfield(L, -2, "GetCenter");
            lua_setglobal(L, "CapsuleCollider");
        }
        else
        {
            CORE_WARN("{0} was found, but the Lua Table is not implemented yet", typeid(T).name());
        }
    }
    else
    {
        CORE_WARN("Missing {0} component", typeid(T).name());
    }
}

void LuaScript::LuaParseTableOnTop(const char* tabs)
{
    if (lua_istable(L, -1))
    {
        std::stringstream tabStream;
        LOG_INFO("{0}{1}", tabs, "{");
        tabStream << tabs << "\t";
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            switch (lua_type(L, -1))//value type
            {
                case LUA_TNONE:
                    CORE_INFO("What is the case for it to be NONE?");
                    break;
                case LUA_TNIL:
                    switch (lua_type(L, -2))//key type
                    {
                        case LUA_TNUMBER:
                            LOG_INFO("{0}({1})[{2}]: nil", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                            break;
                        case LUA_TSTRING:
                            LOG_INFO("{0}({1})[\"{2}\"]: nil", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TBOOLEAN:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            LOG_INFO("{0}({1})[{2}]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2), lua_toboolean(L, -1));
                            break;
                        case LUA_TSTRING:
                            LOG_INFO("{0}({1})[\"{2}\"]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2), lua_toboolean(L, -1));
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TLIGHTUSERDATA:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                            break;
                        case LUA_TSTRING:
                            LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TNUMBER:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            LOG_INFO("{0}({1})[{2}]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2), lua_tonumber(L, -1));
                            break;
                        case LUA_TSTRING:
                            LOG_INFO("{0}({1})[\"{2}\"]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2), lua_tonumber(L, -1));
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TSTRING:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            LOG_INFO("{0}({1})[{2}]: \"{3}\"", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2), lua_tostring(L, -1));
                            break;
                        case LUA_TSTRING:
                            LOG_INFO("{0}({1})[\"{2}\"]: \"{3}\"", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2), lua_tostring(L, -1));
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TTABLE:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                            LuaParseTableOnTop(tabStream.str().c_str());
                            break;
                        case LUA_TSTRING:
                            LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                            LuaParseTableOnTop(tabStream.str().c_str());
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TFUNCTION:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                            break;
                        case LUA_TSTRING:
                            LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TUSERDATA:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                            break;
                        case LUA_TSTRING:
                            LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TTHREAD:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                            break;
                        case LUA_TSTRING:
                            LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                default:
                    CORE_INFO("What is the case for this");
                    break;
            }
            lua_pop(L, 1);
        }
        LOG_INFO("{0}{1}", tabs,"}");
    }
}