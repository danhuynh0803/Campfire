#include "Scripting/Lua/LuaAudioSource.h"
#include "Audio/AudioSource.h"

int LuaAudioSource::Play(lua_State* L)
{
	AudioSource* audioSource = (AudioSource*)lua_touserdata(L, lua_upvalueindex(1));
	audioSource->Play();
	return 0;
}

int LuaAudioSource::Pause(lua_State* L)
{
	AudioSource* audioSource = (AudioSource*)lua_touserdata(L, lua_upvalueindex(1));
	audioSource->Pause();
	return 0;
}

int LuaAudioSource::Stop(lua_State* L)
{
	AudioSource* audioSource = (AudioSource*)lua_touserdata(L, lua_upvalueindex(1));
	audioSource->Stop();
	return 0;
}