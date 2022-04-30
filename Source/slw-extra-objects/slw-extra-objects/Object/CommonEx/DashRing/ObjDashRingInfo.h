#pragma once

namespace app
{
    class ObjDashRingInfo : public CObjInfo
    {
    private:
        inline static const char* ms_ModelNames[] = { "cmn_obj_dashring", "cmn_obj_rainbowring" };
        inline static const char* ms_AnimNames[] = {
            "cmn_obj_dashring-0000", "cmn_obj_dashring-0001", "cmn_obj_dashring-0002", "cmn_obj_dashring-0003",
            "cmn_obj_rainbowring-0000", "cmn_obj_rainbowring-0001", "cmn_obj_rainbowring-0002", "cmn_obj_rainbowring-0003"
        };

    public:
        inline static size_t ms_ModelCount = ARRAYSIZE(ms_ModelNames);
        inline static size_t ms_AnimCount = ARRAYSIZE(ms_AnimNames);

        hh::gfx::res::ResModel m_Models[ARRAYSIZE(ms_ModelNames)]{};
        hh::gfx::res::ResAnimTexSrt m_TexAnims[ARRAYSIZE(ms_ModelNames)]{};
        hh::gfx::res::ResAnimMaterial m_MatAnims[ARRAYSIZE(ms_ModelNames)]{};

    protected:
        void Initialize(GameDocument& document) override
        {
            auto packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");

            for (size_t i = 0; i < ms_ModelCount; i++)
            {
                m_Models[i] = ObjUtil::GetModelResource(ms_ModelNames[i], packFile);
                m_MatAnims[i] = ObjUtil::GetMaterialAnimationResource(ms_ModelNames[i], packFile);
            }

            for (size_t i = 0; i < ms_AnimCount; i++)
                m_TexAnims[i] = ObjUtil::GetTexSrtAnimationResource(ms_AnimNames[i], packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjDashRingInfo";
        }
    };
}