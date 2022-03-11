#pragma once

namespace TestingGrounds
{
	namespace ObjUtil
	{
		inline static FUNCTION_PTR(void, __cdecl, GetSetObjectTransform, ASLR(0x0073E040), app::GameDocument* gameDocument, const app::CSetObjectID& objectID, csl::math::Vector3* position, csl::math::Quaternion* rotation);
	}

	class GOCSound
	{
	public:
		inline static FUNCTION_PTR(void, __thiscall, Play3D, ASLR(0x004CD060), app::game::GOCSound* This, int* DeviceTag, const char* CueName, int a4);
	};

	inline static FUNCTION_PTR(void, __cdecl, AddParamFloat, ASLR(0x00843CB0), app::SetEd::CResClass rClass, const char* name, const char* description, unsigned location, float defaultValue, float a1, float a2, float a3);
	inline static FUNCTION_PTR(void, __cdecl, AddParamEnum, ASLR(0x00843D10), app::SetEd::CResClass rClass, const char* name, const char* description, unsigned int location, const char** a1, unsigned int a2, unsigned char a3);
	inline static FUNCTION_PTR(void, __cdecl, AddParamTarget, ASLR(0x00843D60), app::SetEd::CResClass rClass, const char* name, const char* description, unsigned int location);
	
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