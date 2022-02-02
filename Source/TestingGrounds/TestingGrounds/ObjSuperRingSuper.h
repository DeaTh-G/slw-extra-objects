#pragma once
#include "Patches.h"

namespace app
{
    class ObjSuperRingSuperInfo : public CObjInfo
    {
    public:
        int m_pModel;

    protected:
        void Initialize(GameDocument& document) override
        {
            printf("Initializing ObjSuperRingSuperInfo\n");
            
            hh::ut::PackFile packFile = ObjUtil::GetPackFile(ObjUtil::GetStagePackName(document));
            TestingGrounds::ObjUtil::GetModelResource(&m_pModel, "cmn_obj_super_ring_super", packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjSuperRingSuperInfo";
        }
    };

    class ObjSuperRingSuper : public CSetObjectListener
    {
    public:
        void AddCallback(GameDocument& document) override
        {
            printf("Created ObjSuperRingSuper\n");

            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            
            ObjSuperRingSuperInfo* info = ObjUtil::GetObjectInfo<ObjSuperRingSuperInfo>(document, "ObjSuperRingSuperInfo");

            fnd::GOComponent::BeginSetup(*this);

            fnd::GOCVisualModel* gocVisual = GetComponent<fnd::GOCVisualModel>();
            if (gocVisual)
            {
                fnd::GOCVisualModel::Description description {};
                description.m_pModel = reinterpret_cast<hh::gfx::res::ResModel*>((((int**)info)[4]));

                gocVisual->Setup(description);
            }

            fnd::GOComponent::EndSetup(*this);
        }
    };
}