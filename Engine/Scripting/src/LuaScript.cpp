#include "Scene/ScriptableEntity.h"
#include "Core/Log.h"
#include "Scripting/LuaScript.h"
#include "Scripting/Lua/LuaTransform.h"
#include "Scripting/Lua/LuaTag.h"
#include "Scripting/Lua/LuaEntity.h"
#include "Scripting/Lua/LuaInput.h"
#include "Scripting/Lua/LuaRigidbody.h"

void LuaScript::Start()
{
    L = luaL_newstate();
    luaL_openlibs(L); //opens all standard Lua libraries

    /*
    char* transformComponetMetaTableName = "TransComMT";
    {luaL_newmetatable(L, transformComponetMetaTableName);
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, LuaTransfrom::LuaTransformTableIndex);//indexing method for the Transfrom table above
    }lua_settable(L, -3); //sets the (meta)table and pop above
    */

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
            lua_pushcclosure(L, LuaTransfrom::GetEntityPosition, 1);
            lua_setfield(L, -2, "GetRotation");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::GetEntityPosition, 1);
            lua_setfield(L, -2, "GetScale");
        }
        lua_setglobal(L, "Transform");
    }

    if(HasComponent<RigidbodyComponent>())
    {
        lua_newtable(L);
        {
            Rigidbody* rb = GetComponent<RigidbodyComponent>().rigidbody.get();
            lua_pushlightuserdata(L, rb);
            lua_pushcclosure(L, LuaRigidbody::AddVelocity, 1);
            lua_setfield(L, -2, "AddVelocity");

            lua_pushlightuserdata(L, rb);
            lua_pushcclosure(L, LuaRigidbody::SetVelocity, 1);
            lua_setfield(L, -2, "SetVelocity");

            lua_pushlightuserdata(L, rb);
            lua_pushcclosure(L, LuaRigidbody::GetVelocity, 1);
            lua_setfield(L, -2, "GetVelocity");
        }
        lua_setglobal(L, "Rigidbody");
    }

    //if(HasComponent<RigidbodyComponent>())
    //{
    //    lua_newtable(L);
    //    lua_pushcfunction_with_entity(LuaRigidbody::AddVelocity,"AddVelocity");
    //    lua_pushcfunction_with_entity(LuaRigidbody::GetVelocity, "GetVelocity");
    //    lua_pushcfunction_with_entity(LuaRigidbody::SetVelocity, "SetVelocity");
    //    lua_pushcfunction_with_entity(LuaRigidbody::GetDrag, "GetMass");
    //    lua_pushcfunction_with_entity(LuaRigidbody::SetMass, "SetMass");
    //    lua_pushcfunction_with_entity(LuaRigidbody::GetDrag, "GetDrag");
    //    lua_pushcfunction_with_entity(LuaRigidbody::SetDrag, "SetDrag");
    //    lua_pushcfunction_with_entity(LuaRigidbody::GetAngularDrag, "GetAngularDrag");
    //    lua_pushcfunction_with_entity(LuaRigidbody::SetAngularDrag, "SetAngularDrag");
    //    lua_setglobal(L, "Rigidbody");
    //}

    //entity:AddComponent("Rigidbody")

    {lua_newtable(L);
        //lua_getglobal(L, "Transform");
        //lua_setfield(L, -1, "Transform");
    }lua_setglobal(L, "entity"); //name the table entity

    {lua_newtable(L);
        luaL_setfuncs(L, LuaTag::tagLib, 0);
    }lua_setglobal(L, "Tag");//name the table Tag

    {lua_newtable(L);
        luaL_setfuncs(L, LuaInput::inputLib, 0);
    }lua_setglobal(L, "Input");//name the table Input

    luaL_dofile(L, filepath.c_str());
    lua_getglobal(L, "Start");
    if (lua_pcall(L, 0, 0, 0) != 0)
    {
        LOG_ERROR("Cannot run Start() within {0}", filepath);
        LOG_ERROR("Error {0}", lua_tostring(L, -1));
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
        LOG_ERROR("Cannot run Update() within {0}", filepath);
        LOG_ERROR("Error {0}", lua_tostring(L, -1));
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
