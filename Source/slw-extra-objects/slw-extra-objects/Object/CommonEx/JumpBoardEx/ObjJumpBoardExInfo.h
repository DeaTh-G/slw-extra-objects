#pragma once

namespace app
{
    class ObjJumpBoardExInfo : public CObjInfo
    {
    private:
        inline static const char* ms_ModelNames[] = { "cmn_obj_jumpboard_15S", "cmn_obj_jumpboard_30S", "cmn_obj_jumpboard_30M", "cmn_obj_jumpboard_30L" };
        inline static const char* ms_BeltAnimNames[] = { "cmn_obj_jumpboard_belt-0000", "cmn_obj_jumpboard_belt-0001" };

    public:
        inline static size_t ms_ModelCount = ARRAYSIZE(ms_ModelNames);
        inline static size_t ms_BeltAnimCount = ARRAYSIZE(ms_BeltAnimNames);

        hh::gfx::res::ResModel m_Models[ARRAYSIZE(ms_ModelNames)]{};
        ResPhysicsMesh m_Colliders[ARRAYSIZE(ms_ModelNames)]{};
        hh::gfx::res::ResAnimTexSrt m_BeltAnims[ARRAYSIZE(ms_BeltAnimNames)]{};
        hh::gfx::res::ResAnimTexSrt m_ArrowAnim{};

    protected:
        void Initialize(GameDocument& document) override
        {
            auto packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");
            for (size_t i = 0; i < ms_ModelCount; i++)
            {
                m_Models[i] = ObjUtil::GetModelResource(ms_ModelNames[i], packFile);
                m_Colliders[i] = ObjUtil::GetPhysicsMeshResource(ms_ModelNames[i], packFile);
            }

            for (size_t i = 0; i < ms_BeltAnimCount; i++)
                m_BeltAnims[i] = ObjUtil::GetTexSrtAnimationResource(ms_BeltAnimNames[i], packFile);

            m_ArrowAnim = ObjUtil::GetTexSrtAnimationResource("cmn_obj_jumpboard_arrow", packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjJumpBoardExInfo";
        }
    };
}