#include "pch.h"
#include "PlayerStateBase.h"

HOOK(int, __fastcall, ProcMsgTakeObjectHook, ASLR(0x008947B0), app::Player::CStateBase* in_pThis, void* edx, app::Player::CStateGOC& in_rStateGoc, app::xgame::MsgTakeObject& in_rMessage)
{
    if (in_rMessage.m_Type != 19)
        return originalProcMsgTakeObjectHook(in_pThis, edx, in_rStateGoc, in_rMessage);

    if (app::Player::StateUtil::IsDead(in_rStateGoc) || in_rStateGoc.GetBlackBoard()->m_Unk1[3].test(26))
        return true;

    if (in_rMessage.IsValidUserID() && in_rMessage.m_UserID != 6)
        return true;

    app::Player::StateUtil::AddRingNum(in_rStateGoc, 100);
    app::Player::StateUtil::SendMissionGetRing(in_rStateGoc, 100);

    in_rStateGoc.AddStatePlugin(10);
    auto* pPlugin = in_rStateGoc.GetStatePlugin<app::Player::PluginStateInvincible>();
    pPlugin->RequestStart(in_rStateGoc, 0);

    in_rMessage.m_Taken = 1;
    return 1;
}

void slw_extra_objects::Player::CStateBase::InstallHooks()
{
    INSTALL_HOOK(ProcMsgTakeObjectHook);
}

inline static void* ReturnAddress = (void*)ASLR(0x0089792D);

__declspec(naked) void SetCatchPlayerMidAsmHook()
{
    __asm
    {
        cmp eax, 21
        jne default
        mov ebx, 16
        mov ecx, [esi + 8]
        jmp [ReturnAddress]

        default:
        xor ebx, ebx
        mov ecx, [esi + 8]
        jmp [ReturnAddress]
    }
}

void slw_extra_objects::Player::CActionStateBase::InstallHooks()
{
    WRITE_JUMP(ASLR(0x00897928), &SetCatchPlayerMidAsmHook);
}