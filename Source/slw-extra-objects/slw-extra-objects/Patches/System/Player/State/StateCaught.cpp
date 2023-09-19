#include "pch.h"
#include "StateCaught.h"

inline static const char* StandAnimation = "STAND";
inline static const char* UpreelAnimation = "UPREEL";
inline static void* ReturnAddress = (void*)ASLR(0x008A56C5);

__declspec(naked) void OnEnterMidAsmHook()
{
    __asm
    {
        cmp eax, 15
        jne default
        mov ebx, UpreelAnimation
        jmp [ReturnAddress]

        default:
        mov ebx, StandAnimation
        jmp [ReturnAddress]
    }
}

void slw_extra_objects::Player::CStateCaught::InstallHooks()
{
    WRITE_JUMP(ASLR(0x008A586B), &OnEnterMidAsmHook);
}