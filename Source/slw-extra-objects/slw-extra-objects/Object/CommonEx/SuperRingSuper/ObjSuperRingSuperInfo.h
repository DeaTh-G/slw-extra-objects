#pragma once

namespace app
{
    class ObjSuperRingSuperInfo : public CObjInfo
    {
    public:
        hh::gfx::res::ResModel m_Model{};

    protected:
        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");
            m_Model = ObjUtil::GetModelResource("cmn_obj_super_ring_super", packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjSuperRingSuperInfo";
        }
    };
}