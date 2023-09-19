#pragma once

namespace app
{
    class ObjSuperRingSuperInfo : public CObjInfo
    {
    private:
        inline static const char* ms_pPackfileName = "CommonObjectEx.pac";
        inline static const char* ms_pModelName = "cmn_obj_super_ring_super";

    public:
        inline static const char* ms_pName = "ObjSuperRingSuperInfo";

        hh::gfx::res::ResModel Model{};

    protected:
        void Initialize(GameDocument& in_rDocument) override
        {
            hh::ut::PackFile packFile = ObjUtil::GetPackFile(ms_pPackfileName);
            Model = ObjUtil::GetModelResource(ms_pModelName, packFile);
        }

        const char* GetInfoName() override
        {
            return ms_pName;
        }
    };
}