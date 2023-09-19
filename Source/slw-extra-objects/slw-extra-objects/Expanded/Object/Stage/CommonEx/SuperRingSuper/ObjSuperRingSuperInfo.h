#pragma once

namespace slw_extra_objects
{
    class ObjSuperRingSuperInfo : public app::CObjInfo
    {
    private:
        inline static const char* ms_pPackfileName = "CommonObjectEx.pac";
        inline static const char* ms_pModelName = "cmn_obj_super_ring_super";

    public:
        inline static const char* ms_pName = "ObjSuperRingSuperInfo";

        hh::gfx::res::ResModel Model{};

    protected:
        void Initialize(app::GameDocument& in_rDocument) override;
        const char* GetInfoName() override;
    };
}