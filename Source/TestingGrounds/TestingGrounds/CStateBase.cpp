#include "pch.h"
#include "Patches.h"

inline static FUNCTION_PTR(unsigned int, __cdecl, IsDead, ASLR(0x008D8C10), void* stateGOC);
inline static FUNCTION_PTR(int*, __thiscall, GetBlackBoard, ASLR(0x0085B510), void* This);
inline static FUNCTION_PTR(int, __cdecl, AddRingNum, ASLR(0x008D7BA0), void* This, int ringNum);
inline static FUNCTION_PTR(int, __cdecl, SendMissionGetRing, ASLR(0x008D7100), void* This, int ringNum);
inline static FUNCTION_PTR(int, __thiscall, AddStatePlugin, ASLR(0x0085C3F0), void* This, int pluginNum);
inline static FUNCTION_PTR(void*, __thiscall, GetStatePluginPtr, ASLR(0x0085C410), void* This, char* pluginName);
inline static FUNCTION_PTR(int, __thiscall, RequestStart, ASLR(0x008D3B50), void* This, void* stateGOC, int unk1);
inline static FUNCTION_PTR(int, __thiscall, PlaySE, ASLR(0x0085C420), void* This, int* DeviceTag, const char* name, int unk1);

inline static char* const PluginStateInvincibleString = (char*)ASLR(0x00E006C0);


HOOK(int, __fastcall, ProcMsgTakeObjectHook, ASLR(0x008947B0), int* This, void* edx, int* cStateGOC, app::xgame::MsgTakeObject& message)
{
    int result{};
    if (message.m_Type != 19)
        return result = originalProcMsgTakeObjectHook(This, edx, cStateGOC, message);

    if (IsDead(cStateGOC) || (GetBlackBoard(cStateGOC)[6] & 0x4000000))
        return true;

    if (message.IsValidUserID() && message.m_UserID != 6)
        return true;

    AddRingNum(cStateGOC, 100);
    SendMissionGetRing(cStateGOC, 100);
    
    AddStatePlugin(cStateGOC, 10);
    void* pluginPtr = GetStatePluginPtr(cStateGOC, PluginStateInvincibleString);
    RequestStart(pluginPtr, cStateGOC, 0);

    message.m_Taken = 1;
    return 1;
}

void TestingGrounds::CStateBase::InstallPatches()
{
    INSTALL_HOOK(ProcMsgTakeObjectHook);
}