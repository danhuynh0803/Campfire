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
#include "Scripting/LuaCall.h"
#include "Core/Log.h"
#include "Physics/Collider.h"

static int Log(lua_State* L)
{
    luaL_checkstring(L, -1);
    const char* msg = lua_tostring(L, -1);
    LOG_INFO(msg);
    return 0;
}

void LuaScript::Start()
{
    L = luaL_newstate();
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
        LuaPushCFunctionWithTag(L,LuaTag::GetTag,"GetTag");
    }
    lua_setglobal(L, "Tag");//name the table Tag

    lua_newtable(L);
    {
        luaL_setfuncs(L, LuaInput::inputLib, 0);
    }
    lua_setglobal(L, "Input");//name the table Input

    StartCoroutine = lua_newthread(L);
    lua_pushstring(StartCoroutine, filepath.c_str());
    lua_pushlightuserdata(StartCoroutine, this);
    lua_pushcclosure(StartCoroutine, LuaCall::Start, 2);
    
    int nresults = 0;
    lua_resume(StartCoroutine, 0, 0, &nresults);
}

void LuaScript::Update(float dt)
{
    if (!L) return;
    //lua_pushnumber(L, dt);
    //lua_setglobal(L, "deltatime");
    //UpdateCoroutine = lua_newthread(L);
    //lua_pushstring(UpdateCoroutine, filepath.c_str());
    //lua_pushcclosure(UpdateCoroutine, LuaCall::Update, 1);
    //int nresults = 0;
    //lua_resume(UpdateCoroutine, 0, 0, &nresults);
}

void LuaScript::Destroy()
{
    lua_close(L);
}

void LuaScript::OnTriggerEnter(Entity other)
{
    if (!L) return;
    lua_State* OnTriggerEnterCoroutine = lua_newthread(L);
    lua_pushstring(OnTriggerEnterCoroutine, filepath.c_str());
    lua_pushlightuserdata(OnTriggerEnterCoroutine, this);
    lua_pushlightuserdata(OnTriggerEnterCoroutine, &other);
    lua_pushcclosure(OnTriggerEnterCoroutine, LuaCall::OnTriggerEnter,3);
    int nresults = 0;
    lua_resume(OnTriggerEnterCoroutine, 0, 0, &nresults);
}

void LuaScript::OnTriggerStay(Entity other)
{
    //if (!L) return;
    //lua_State* OnTriggerStayThread = lua_newthread(L);
    //lua_pushstring(OnTriggerStayThread, filepath.c_str());
    //lua_pushlightuserdata(OnTriggerStayThread, this);
    //lua_pushlightuserdata(OnTriggerStayThread, &other);
    //lua_pushcclosure(OnTriggerStayThread, LuaCall::OnTriggerStay, 3);
    //int nresults = 0;
    //lua_resume(OnTriggerStayThread, 0, 0, &nresults);
}

void LuaScript::OnTriggerExit(Entity other)
{
    //if (!L) return;
    //lua_State* OnTriggerExitThread = lua_newthread(L);
    //lua_pushstring(OnTriggerExitThread, filepath.c_str());
    //lua_pushlightuserdata(OnTriggerExitThread, this);
    //lua_pushlightuserdata(OnTriggerExitThread, &other);
    //lua_pushcclosure(OnTriggerExitThread, LuaCall::OnTriggerExit, 3);
    //int nresults = 0;
    //lua_resume(OnTriggerExitThread, 0, 0, &nresults);
}

void LuaScript::LuaPushCFunctionWithEntity(lua_State* L, const lua_CFunction& f, const char* name)
{
    lua_pushlightuserdata(L, &entity);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithRigidbody(lua_State* L, Entity entity, const lua_CFunction& f, const char* name)
{
    SharedPtr<Rigidbody> rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
    lua_pushlightuserdata(L, rigidbody.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithRigidbody(lua_State* L, const lua_CFunction& f, const char* name)
{
    SharedPtr<Rigidbody> rigidbody = GetComponent<RigidbodyComponent>().rigidbody;
    lua_pushlightuserdata(L, rigidbody.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithTag(lua_State* L,const lua_CFunction& f, const char* name)
{
    const char* tag = GetComponent<TagComponent>().tag.c_str(); lua_pushstring(L, tag);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithAudioSource(lua_State* L,const lua_CFunction& f, const char* name)
{
    SharedPtr<AudioSource> audioSource = GetComponent<AudioComponent>().audioSource;
    lua_pushlightuserdata(L, audioSource.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushEntity(Entity entity, lua_State* L)
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
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::AddVelocity, "AddVelocity");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetVelocity, "GetVelocity");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetVelocity, "SetVelocity");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetMass, "GetMass");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetMass, "SetMass");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetDrag, "GetDrag");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetDrag, "SetDrag");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::UseGravity, "UseGravity");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::FreezePosition, "FreezePosition");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::FreezeRotation, "FreezeRotation");
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
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::AddVelocity, "AddVelocity");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::GetVelocity, "GetVelocity");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::SetVelocity, "SetVelocity");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::GetMass, "GetMass");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::SetMass, "SetMass");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::GetDrag, "GetDrag");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::SetDrag, "SetDrag");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::UseGravity, "UseGravity");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::FreezePosition, "FreezePosition");
            LuaPushCFunctionWithRigidbody(L, LuaRigidbody::FreezeRotation, "FreezeRotation");
            lua_setglobal(L, "Rigidbody");
        }
        else if (std::is_same<T, AudioComponent>::value)
        {
            lua_newtable(L);
            LuaPushCFunctionWithAudioSource(L, LuaAudioSource::Play, "Play");
            LuaPushCFunctionWithAudioSource(L, LuaAudioSource::Pause, "Pause");
            LuaPushCFunctionWithAudioSource(L, LuaAudioSource::Stop, "Stop");
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
