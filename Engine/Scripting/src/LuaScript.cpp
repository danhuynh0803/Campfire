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
#include "Scripting/Lua/LuaGlobal.h"
#include "Scripting/Lua/LuaCamera.h"
#include "Scripting/LuaUtility.h"
#include "Scripting/LuaManager.h"
#include "Core/Log.h"
#include "Physics/Collider.h"

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

    //lua_pushglobaltable(L);
    //LOG_INFO("Non-Nil Global Variables found in {0}", filepath);
    //LuaParseTableOnTop("");

    luaL_openlibs(L); //opens all standard Lua libraries

    // Setup logs
    lua_pushcfunction(L, LuaUtility::Log);
    lua_setglobal(L, "Log");

    {
        lua_newtable(L);
        lua_pushcfunction(L, LuaVector::LuaVec2);
        lua_setfield(L, -2, "New");
        lua_setglobal(L, "Vec2");
    }

    {
        lua_newtable(L);
        lua_pushcfunction(L, LuaVector::LuaVec3);
        lua_setfield(L, -2, "New");
        lua_setglobal(L, "Vec3");
    }

    {
        lua_newtable(L);
        lua_pushcfunction(L, LuaVector::LuaVec4);
        lua_setfield(L, -2, "New");
        lua_setglobal(L, "Vec4");
    }
    
    {
        char* vec2MetaTableName = "LuaVec2MetaTable";
        luaL_newmetatable(L, vec2MetaTableName);
        lua_pushstring(L, "__add");
        lua_pushcfunction(L, LuaVector::LuaVec2Add);
        lua_settable(L, -3);
        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, LuaVector::LuaVec2Sub);
        lua_settable(L, -3);
        lua_pop(L, 1);//metatable can be popped after initialization
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
        lua_pop(L, 1);
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
        lua_pop(L, 1);
    }

    {
        LuaPushComponetTable<TransformComponent>();
        LuaPushComponetTable<RigidbodyComponent>();
        LuaPushComponetTable<AudioSource>();
        LuaPushComponetTable<BoxCollider>();
        LuaPushComponetTable<SphereCollider>();
        LuaPushComponetTable<CapsuleCollider>();
    }

    {
        lua_newtable(L);
        lua_pushlightuserdata(L, this);
        lua_pushcclosure(L, LuaEntity::Instantiate, 1);
        lua_setfield(L, -2, "Instantiate");
        lua_pushlightuserdata(L, &entity);
        lua_pushcclosure(L, LuaEntity::EntityDestroy, 1);
        lua_setfield(L, -2, "Destroy");
        lua_setglobal(L, "Entity"); //name the table entity
    }
   
    {
        lua_newtable(L);
        lua_pushcfunction(L, LuaGlobal::GetBoolean);
        lua_setfield(L, -2, "GetBoolean");
        lua_pushcfunction(L, LuaGlobal::GetInteger);
        lua_setfield(L, -2, "GetInteger");
        lua_pushcfunction(L, LuaGlobal::GetNumber);
        lua_setfield(L, -2, "GetNumber");
        lua_pushcfunction(L, LuaGlobal::GetString);
        lua_setfield(L, -2, "GetString");
        lua_pushcfunction(L, LuaGlobal::GetTable);
        lua_setfield(L, -2, "GetTable");
        lua_pushcfunction(L, LuaGlobal::SetBoolean);
        lua_setfield(L, -2, "SetBoolean");
        lua_pushcfunction(L, LuaGlobal::SetInteger);
        lua_setfield(L, -2, "SetInteger");
        lua_pushcfunction(L, LuaGlobal::SetNumber);
        lua_setfield(L, -2, "SetNumber");
        lua_pushcfunction(L, LuaGlobal::SetString);
        lua_setfield(L, -2, "SetString");
        lua_pushcfunction(L, LuaGlobal::SetTable);
        lua_setfield(L, -2, "SetTable");
        lua_setglobal(L, "Global"); 
    }

    {
        lua_newtable(L);
        LuaPushCFunctionWithTag(LuaTag::GetTag,"GetTag");
        lua_setglobal(L, "Tag");
    }

    {
        lua_newtable(L);
        luaL_setfuncs(L, LuaInput::inputLib, 0);
        lua_setglobal(L, "Input");
    }

    {
        lua_newtable(L);
        lua_pushcfunction(L, LuaUtility::DeseralizeLuaTableX);
        lua_setfield(L, -2, "DeseralizeLuaTableX");
        lua_setglobal(L, "Utility");
    }

    lua_pushcfunction(L, LuaUtility::LuaCallback);
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
    lua_setglobal(L, "dt");
    lua_pushcfunction(L, LuaUtility::LuaCallback);
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
    lua_pushcfunction(L, LuaUtility::LuaCallback);
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
    lua_pushcfunction(L, LuaUtility::LuaCallback);
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
    lua_pushcfunction(L, LuaUtility::LuaCallback);
    if (lua_getglobal(L, "OnTriggerExit"))
    {
        LuaPushEntity(other);
        if (runOnTriggerExit)
        {
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

//Push Functions

void LuaScript::LuaPushCFunctionWithAudioSource(const lua_CFunction& f, const char* name)
{
    SharedPtr<AudioSource> audioSource = GetComponent<AudioComponent>().audioSource;
    lua_pushlightuserdata(L, audioSource.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithEntity(const lua_CFunction& f, const char* name)
{
    lua_pushlightuserdata(L, &entity);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithCamera(const lua_CFunction& f, const char* name)
{
    SharedPtr<Camera> camera = GetComponent<CameraComponent>().camera;
    lua_pushlightuserdata(L, camera.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithCamera(Entity entity, const lua_CFunction& f, const char* name)
{
    SharedPtr<Camera> camera = entity.GetComponent<CameraComponent>().camera;
    lua_pushlightuserdata(L, camera.get());
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

void LuaScript::LuaPushCFunctionWithRigidbody(Entity entity, const lua_CFunction& f, const char* name)
{
    SharedPtr<Rigidbody> rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
    lua_pushlightuserdata(L, rigidbody.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithTag(const lua_CFunction& f, const char* name)
{
    lua_pushlightuserdata(L, &(GetComponent<TagComponent>().tag));
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushEntity(Entity entity)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, &entity);
    lua_pushcclosure(L, LuaEntity::OtherEntityDestroy, 1);
    lua_setfield(L, -2, "Destroy");

    if (entity.HasComponent<TagComponent>())
    {
        {
            lua_newtable(L);
            lua_pushlightuserdata(L, &(entity.GetComponent<TagComponent>().tag));
            lua_pushcclosure(L, LuaTag::GetTag, 1);
            lua_setfield(L, -2, "GetTag");
        }
        lua_setfield(L, -2, "Tag");
    }

    if (entity.HasComponent<TransformComponent>())
    {
        {
            lua_newtable(L);
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
        {
            lua_newtable(L);
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetVelocity, "SetVelocity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::AddVelocity, "AddVelocity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetVelocity, "GetVelocity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetMass, "SetMass");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetMass, "GetMass");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetDrag, "SetDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetDrag, "GetDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetUseGravity, "SetUseGravity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetUseGravity, "GetUseGravity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetFreezePosition, "SetFreezePosition");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetFreezePosition, "GetFreezePosition");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetFreezeRotation, "SetFreezeRotation");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetFreezePosition, "GetFreezeRotation");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetBodyType, "SetBodyType");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetBodyType, "GetBodyType");
        }
        lua_setfield(L, -2, "Rigidbody");
    }

    if (entity.HasComponent<AudioComponent>())
    {
        {
            lua_newtable(L);
            LuaPushCFunctionWithAudioSource(LuaAudioSource::Play, "Play");
            LuaPushCFunctionWithAudioSource(LuaAudioSource::Pause, "Pause");
            LuaPushCFunctionWithAudioSource(LuaAudioSource::Stop, "Stop");
        }
        lua_setfield(L, -2, "AudioSource");
    }

    if (entity.HasComponent<CameraComponent>())
    {
        {
            lua_newtable(L);
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetClearFlags, "SetClearFlags");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetClearFlags, "GetClearFlags");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetBackgroundColor, "SetBackgroundColor");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetBackgroundColor, "GetBackgroundColor");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetIsPerspective, "SetIsPerspective");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetIsPerspective, "SetClearFlags");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetVerticalFOV, "SetVerticalFOV");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetVerticalFOV, "GetVerticalFOV");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetNearPlane, "SetNearPlane");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetNearPlane, "GetNearPlane");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetFarPlane, "GetFarPlane");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetFarPlane, "SetFarPlane");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetDepth, "SetDepth");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetDepth, "GetDepth");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetViewPortRectX, "SetViewPortRectX");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetViewPortRectX, "GetViewPortRectX");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetViewPortRectY, "SetViewPortRectY");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetViewPortRectY, "GetViewPortRectY");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetViewPortRectWidth, "SetViewPortRectWidth");
            LuaPushCFunctionWithCamera(entity, LuaCamera::GetViewPortRectWidth, "SetViewPortRectWidth");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetViewPortRectHeight, "SetViewPortRectHeight");
            LuaPushCFunctionWithCamera(entity, LuaCamera::SetViewPortRectHeight, "SetViewPortRectHeight");
        }
        lua_setfield(L, -2, "Camera");
    }
    //if (entity.HasComponent<MeshComponent>())
    //{
    //}
    //if (entity.HasComponent<SpriteComponent>())
    //{
    //}
    //if (entity.HasComponent<LightComponent>())
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
        if (std::is_same<T, TagComponent>::value)
        {
            lua_newtable(L);
            LuaPushCFunctionWithTag(LuaTag::GetTag, "GetTag");
            lua_setglobal(L, "Tag");
        }
        else if (std::is_same<T, TransformComponent>::value)
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

            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetVelocity, "SetVelocity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::AddVelocity, "AddVelocity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetVelocity, "GetVelocity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetMass, "SetMass");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetMass, "GetMass");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetDrag, "SetDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetDrag, "GetDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetUseGravity, "SetUseGravity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetUseGravity, "GetUseGravity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetFreezePosition, "SetFreezePosition");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetFreezePosition, "GetFreezePosition");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetFreezeRotation, "SetFreezeRotation");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetFreezeRotation, "GetFreezeRotation");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetBodyType, "SetBodyType");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetBodyType, "GetBodyType");
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
        else if (std::is_same<T, CameraComponent>::value)
        {
            lua_newtable(L);
            LuaPushCFunctionWithCamera(LuaCamera::SetClearFlags, "SetClearFlags");
            LuaPushCFunctionWithCamera(LuaCamera::GetClearFlags, "GetClearFlags");
            LuaPushCFunctionWithCamera(LuaCamera::SetBackgroundColor, "SetBackgroundColor");
            LuaPushCFunctionWithCamera(LuaCamera::GetBackgroundColor, "GetBackgroundColor");
            LuaPushCFunctionWithCamera(LuaCamera::SetIsPerspective, "SetIsPerspective");
            LuaPushCFunctionWithCamera(LuaCamera::GetIsPerspective, "SetClearFlags");
            LuaPushCFunctionWithCamera(LuaCamera::SetVerticalFOV, "SetVerticalFOV");
            LuaPushCFunctionWithCamera(LuaCamera::GetVerticalFOV, "GetVerticalFOV");
            LuaPushCFunctionWithCamera(LuaCamera::SetNearPlane, "SetNearPlane");
            LuaPushCFunctionWithCamera(LuaCamera::GetNearPlane, "GetNearPlane");
            LuaPushCFunctionWithCamera(LuaCamera::GetFarPlane, "GetFarPlane");
            LuaPushCFunctionWithCamera(LuaCamera::SetFarPlane, "SetFarPlane");
            LuaPushCFunctionWithCamera(LuaCamera::SetDepth, "SetDepth");
            LuaPushCFunctionWithCamera(LuaCamera::GetDepth, "GetDepth");
            LuaPushCFunctionWithCamera(LuaCamera::SetViewPortRectX, "SetViewPortRectX");
            LuaPushCFunctionWithCamera(LuaCamera::GetViewPortRectX, "GetViewPortRectX");
            LuaPushCFunctionWithCamera(LuaCamera::SetViewPortRectY, "SetViewPortRectY");
            LuaPushCFunctionWithCamera(LuaCamera::GetViewPortRectY, "GetViewPortRectY");
            LuaPushCFunctionWithCamera(LuaCamera::SetViewPortRectWidth, "SetViewPortRectWidth");
            LuaPushCFunctionWithCamera(LuaCamera::GetViewPortRectWidth, "SetViewPortRectWidth");
            LuaPushCFunctionWithCamera(LuaCamera::SetViewPortRectHeight, "SetViewPortRectHeight");
            LuaPushCFunctionWithCamera(LuaCamera::SetViewPortRectHeight, "SetViewPortRectHeight");
            lua_setglobal(L, "Camera");
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
    #ifdef _DEBUG
        CORE_WARN("Missing {0} component", typeid(T).name());
    #endif // _DEBUG
    }
}