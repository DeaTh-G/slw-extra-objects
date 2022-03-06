#pragma once

namespace TestingGrounds
{
	namespace math
	{
		inline static FUNCTION_PTR(csl::math::Quaternion*, __cdecl, Vector3Rotate, ASLR(0x00533000), csl::math::Vector3* a1, csl::math::Quaternion* a2, csl::math::Vector3* a3);
	}	
	
	class GOCSound
	{
	public:
		inline static FUNCTION_PTR(void, __thiscall, Play, ASLR(0x004CD180), app::game::GOCSound* gocSound, int* DeviceTag, const char* CueName, int a4);
	};

	inline static FUNCTION_PTR(void, __cdecl, AddParamFloat, ASLR(0x00843CB0), app::SetEd::CResClass rClass, const char* name, const char* description, unsigned location, float defaultValue, float a1, float a2, float a3);
	inline static FUNCTION_PTR(void, __cdecl, AddParamEnum, ASLR(0x00843D10), app::SetEd::CResClass rClass, const char* name, const char* description, unsigned int location, const char** a1, unsigned int a2, unsigned char a3);
	
	class GOCVisualTransformed
	{
	public:
		inline static FUNCTION_PTR(void, __thiscall, SetLocalScale, ASLR(0x00496240), app::fnd::GOCVisualTransformed* This, csl::math::Vector3* scale);
	};

	class CSetObjectFactory
	{
	public:
		static void InstallPatches();
	};

	class StageDataInitializer
	{
	public:
		static void InstallPatches();
	};

	class CStateBase
	{
	public:
		static void InstallPatches();
	};
}