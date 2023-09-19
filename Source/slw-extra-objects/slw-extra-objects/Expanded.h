#pragma once

namespace slw_extra_objects
{
	inline static FUNCTION_PTR(void, __cdecl, AddParamFloat, ASLR(0x00843CB0), app::SetEd::CResClass rClass, const char* name, const char* description, unsigned location, float defaultValue, float a1, float a2, float a3);
	inline static FUNCTION_PTR(void, __cdecl, AddParamBool, ASLR(0x00843AD0), app::SetEd::CResClass rClass, const char* name, const char* description, unsigned location, float defaultValue);
	inline static FUNCTION_PTR(void, __cdecl, AddParamEnum, ASLR(0x00843D10), app::SetEd::CResClass rClass, const char* name, const char* description, unsigned int location, const char** a1, unsigned int a2, unsigned char a3);
	inline static FUNCTION_PTR(void, __cdecl, AddParamTarget, ASLR(0x00843D60), app::SetEd::CResClass rClass, const char* name, const char* description, unsigned int location);
}

#include "Expanded/System/Player/Info/PlayerVisualSonicExInfo.h"

#include "Expanded/Object/Objects.h"
