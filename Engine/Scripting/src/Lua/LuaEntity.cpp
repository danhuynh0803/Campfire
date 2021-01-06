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
#include "Core/ResourceManager.h"
#include "Scene/Component.h"

void LuaEntity::LuaPushEntity(lua_State* L, Entity entity)
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
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetVelocity, "SetVelocity");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::AddVelocity, "AddVelocity");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetVelocity, "GetVelocity");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetMass, "SetMass");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetMass, "GetMass");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetDrag, "SetDrag");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetDrag, "GetDrag");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetUseGravity, "SetUseGravity");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetUseGravity, "GetUseGravity");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetFreezePosition, "SetFreezePosition");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetFreezePosition, "GetFreezePosition");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetFreezeRotation, "SetFreezeRotation");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetFreezePosition, "GetFreezeRotation");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::SetBodyType, "SetBodyType");
            LuaPushCFunctionWithRigidbody(L, entity, LuaRigidbody::GetBodyType, "GetBodyType");
        }
        lua_setfield(L, -2, "Rigidbody");
    }

    if (entity.HasComponent<AudioComponent>())
    {
        {
            lua_newtable(L);
            LuaPushCFunctionWithAudioSource(L, entity, LuaAudioSource::Play, "Play");
            LuaPushCFunctionWithAudioSource(L, entity, LuaAudioSource::Pause, "Pause");
            LuaPushCFunctionWithAudioSource(L, entity, LuaAudioSource::Stop, "Stop");
        }
        lua_setfield(L, -2, "AudioSource");
    }

    if (entity.HasComponent<CameraComponent>())
    {
        {
            lua_newtable(L);
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetClearFlags, "SetClearFlags");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetClearFlags, "GetClearFlags");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetBackgroundColor, "SetBackgroundColor");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetBackgroundColor, "GetBackgroundColor");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetIsPerspective, "SetIsPerspective");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetIsPerspective, "SetClearFlags");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetVerticalFOV, "SetVerticalFOV");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetVerticalFOV, "GetVerticalFOV");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetNearPlane, "SetNearPlane");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetNearPlane, "GetNearPlane");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetFarPlane, "GetFarPlane");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetFarPlane, "SetFarPlane");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetDepth, "SetDepth");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetDepth, "GetDepth");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetViewPortRectX, "SetViewPortRectX");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetViewPortRectX, "GetViewPortRectX");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetViewPortRectY, "SetViewPortRectY");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetViewPortRectY, "GetViewPortRectY");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetViewPortRectWidth, "SetViewPortRectWidth");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::GetViewPortRectWidth, "SetViewPortRectWidth");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetViewPortRectHeight, "SetViewPortRectHeight");
            LuaPushCFunctionWithCamera(L, entity, LuaCamera::SetViewPortRectHeight, "SetViewPortRectHeight");
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

void LuaEntity::LuaPushCFunctionWithAudioSource(lua_State* L, Entity entity, const lua_CFunction& f, const char* name)
{
    SharedPtr<AudioSource> audioSource = entity.GetComponent<AudioComponent>().audioSource;
    lua_pushlightuserdata(L, audioSource.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaEntity::LuaPushCFunctionWithCamera(lua_State* L, Entity entity, const lua_CFunction& f, const char* name)
{
    SharedPtr<Camera> camera = entity.GetComponent<CameraComponent>().camera;
    lua_pushlightuserdata(L, camera.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaEntity::LuaPushCFunctionWithRigidbody(lua_State* L, Entity entity, const lua_CFunction& f, const char* name)
{
    SharedPtr<Rigidbody> rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
    lua_pushlightuserdata(L, rigidbody.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

int LuaEntity::Instantiate(lua_State* L)
{
    int topIndex = lua_gettop(L);
    std::string prefabPath = luaL_checkstring(L, topIndex-3);
    lua_Number x = luaL_checknumber(L, topIndex-2);
    lua_Number y = luaL_checknumber(L, topIndex-1);
    lua_Number z = luaL_checknumber(L, topIndex);
    LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(1));

    // Deserialize the requested prefab
    Entity newEntity = script->Instantiate(prefabPath, glm::vec3(x, y, z));
    if (!newEntity)
    {
        lua_pushstring(L, "Instantiate invalid Entity");
        lua_error(L);
    }
    if (newEntity)
    {
        lua_newtable(L);
        {
            lua_newtable(L);
            {
                lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().position));
                lua_pushcclosure(L, LuaTransfrom::SetEntityPosition, 1);
                lua_setfield(L, -2, "SetPosition");

                lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().euler));
                lua_pushcclosure(L, LuaTransfrom::SetEntityRotation, 1);
                lua_setfield(L, -2, "SetRotation");

                lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().scale));
                lua_pushcclosure(L, LuaTransfrom::SetEntityScale, 1);
                lua_setfield(L, -2, "SetScale");

                lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().position));
                lua_pushcclosure(L, LuaTransfrom::Translate, 1);
                lua_setfield(L, -2, "Translate");

                lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().position));
                lua_pushcclosure(L, LuaTransfrom::GetEntityPosition, 1);
                lua_setfield(L, -2, "GetPosition");

                lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().rotation));
                lua_pushcclosure(L, LuaTransfrom::GetEntityRotation, 1);
                lua_setfield(L, -2, "GetRotation");

                lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().scale));
                lua_pushcclosure(L, LuaTransfrom::GetEntityScale, 1);
                lua_setfield(L, -2, "GetScale");
            }
            lua_setfield(L, -2, "Transform");
        }
    }
    
    return 1;
}

int LuaEntity::EntityDestroy(lua_State* L)
{
    int topIndex = lua_gettop(L);
    LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(1));
    Entity entity = script->GetEntity();
    entity.AddComponent<Destroy>();
    lua_pushstring(L, "Destroy Entity");
    lua_error(L);
    return 0;
}

int LuaEntity::OtherEntityDestroy(lua_State* L)
{
    Entity* entity = (Entity*)lua_touserdata(L, lua_upvalueindex(1));
    if (!entity)
    {
        lua_pushstring(L, "Cannot destory a invalid Entity");
        lua_error(L);
    }
    else
    {
        if (entity->IsValid())
        {
            entity->AddComponent<Destroy>();
        }
        else
        {
            lua_pushstring(L, "Cannot destory a invalid Entity");
            lua_error(L);
        }
    }
    return 0;
}

int LuaEntity::AddComponent(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    Entity* entity = (Entity*)lua_touserdata(L, lua_upvalueindex(1));
    if (!entity)
    {
        lua_pushstring(L, "Cannot add Component to a invalid Entity.");
        lua_error(L);
    }
    if (strcmp(name, "Tag") == 0)
    {
        if (entity->IsValid())
        {
            if (entity->HasComponent<TagComponent>())
            {
                lua_pushstring(L, "The Entity already have a TagComponent.");
                lua_error(L);
            }
            else
            {
                entity->AddComponent<TagComponent>();
                lua_newtable(L);
                {
                    lua_newtable(L);
                    lua_pushlightuserdata(L, &entity->GetComponent<TagComponent>().tag);
                    lua_pushcclosure(L, LuaTag::GetTag, 1);
                    lua_setfield(L, -2, "GetTag");
                }
                lua_setfield(L, -2, "Tag");
            }
        }
        else
        {
            lua_pushstring(L, "Cannot add Component to a invalid Entity.");
        }
    }
    else if (strcmp(name, "Transform") == 0)
    {
        lua_pushstring(L, "Cannot add a TransformComponent.");
        lua_error(L);
    }
    else if (strcmp(name, "Rigidbody") == 0)
    {
        if (entity->IsValid())
        {
            if (entity->HasComponent<RigidbodyComponent>())
            {
                lua_pushstring(L, "The Entity already have a RigidbodyComponent.");
                lua_error(L);
            }
            else
            {
                entity->AddComponent<RigidbodyComponent>();
                lua_newtable(L);
                {
                    lua_newtable(L);
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::SetVelocity, "SetVelocity");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::AddVelocity, "AddVelocity");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::GetVelocity, "GetVelocity");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::SetMass, "SetMass");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::GetMass, "GetMass");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::SetDrag, "SetDrag");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::GetDrag, "GetDrag");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::SetAngularDrag, "SetAngularDrag");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::GetAngularDrag, "GetAngularDrag");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::SetUseGravity, "SetUseGravity");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::GetUseGravity, "GetUseGravity");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::SetFreezePosition, "SetFreezePosition");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::GetFreezePosition, "GetFreezePosition");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::SetFreezeRotation, "SetFreezeRotation");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::GetFreezePosition, "GetFreezeRotation");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::SetBodyType, "SetBodyType");
                    LuaPushCFunctionWithRigidbody(L, *entity, LuaRigidbody::GetBodyType, "GetBodyType");
                }
                lua_setfield(L, -2, "Tag");
            }
        }
        else
        {
            lua_pushstring(L, "Cannot add Component to a invalid Entity.");
        }
    }
    else if (strcmp(name, "Camera") == 0)
    {
        if (entity->IsValid())
        {
            if (entity->HasComponent<CameraComponent>())
            {
                lua_pushstring(L, "The Entity already have a CameraComponent.");
                lua_error(L);
            }
            else
            {
                lua_newtable(L);
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetClearFlags, "SetClearFlags");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetClearFlags, "GetClearFlags");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetBackgroundColor, "SetBackgroundColor");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetBackgroundColor, "GetBackgroundColor");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetIsPerspective, "SetIsPerspective");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetIsPerspective, "SetClearFlags");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetVerticalFOV, "SetVerticalFOV");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetVerticalFOV, "GetVerticalFOV");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetNearPlane, "SetNearPlane");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetNearPlane, "GetNearPlane");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetFarPlane, "GetFarPlane");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetFarPlane, "SetFarPlane");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetDepth, "SetDepth");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetDepth, "GetDepth");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetViewPortRectX, "SetViewPortRectX");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetViewPortRectX, "GetViewPortRectX");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetViewPortRectY, "SetViewPortRectY");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetViewPortRectY, "GetViewPortRectY");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetViewPortRectWidth, "SetViewPortRectWidth");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::GetViewPortRectWidth, "SetViewPortRectWidth");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetViewPortRectHeight, "SetViewPortRectHeight");
                LuaPushCFunctionWithCamera(L, *entity, LuaCamera::SetViewPortRectHeight, "SetViewPortRectHeight");
            }
            
        }
        else
        {
            lua_pushstring(L, "Cannot add Component to a invalid Entity.");
        }
    }
    else
    {
        lua_pushstring(L, "Cannot add an invalid component");
        lua_error(L);
    }
    return 0;
}

int LuaEntity::RemoveComponent(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    Entity* entity = (Entity*)lua_touserdata(L, lua_upvalueindex(1));
    if (!entity)
    {
        lua_pushstring(L, "Cannot add Component to a invalid Entity.");
        lua_error(L);
    }
    if (strcmp(name, "Tag") == 0)
    {
        if (entity->IsValid())
        {
            if (!entity->HasComponent<TagComponent>())
            {
                lua_pushstring(L, "The Entity does not have a TagComponent.");
                lua_error(L);
            }
            else
            {
                entity->RemoveComponent<TagComponent>();
            }
        }
        else
        {
            lua_pushstring(L, "Cannot remove a Component from a invalid Entity.");
        }
    }
    else if (strcmp(name, "Transform") == 0)
    {
        lua_pushstring(L, "Cannot remove a TransformComponent.");
        lua_error(L);
    }
    else if (strcmp(name, "Rigidbody") == 0)
    {
        if (entity->IsValid())
        {
            if (!entity->HasComponent<RigidbodyComponent>())
            {
                lua_pushstring(L, "The Entity remove a RigidbodyComponent.");
                lua_error(L);
            }
            else
            {
                entity->RemoveComponent<RigidbodyComponent>();
            }
        }
        else
        {
            lua_pushstring(L, "Cannot remove a Component from a invalid Entity.");
        }
    }
    else if (strcmp(name, "Camera") == 0)
    {
        if (entity->IsValid())
        {
            if (!entity->HasComponent<CameraComponent>())
            {
                lua_pushstring(L, "The Entity does not have a CameraComponent.");
                lua_error(L);
            }
            else
            {
                entity->RemoveComponent<CameraComponent>();
            }
        }
        else
        {
            lua_pushstring(L, "Cannot remove a Component from a invalid Entity.");
        }
    }
    return 0;
}

const luaL_Reg LuaEntity::entityLib[] =
{
    { NULL, NULL }
};
