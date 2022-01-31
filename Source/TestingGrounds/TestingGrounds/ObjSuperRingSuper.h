#pragma once
#include "pch.h"

extern app::CSetObjClass SUPER_RING_SUPER_CLASS;

namespace app
{
    class ObjSuperRingSuperInfo : public CObjInfo
    {
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
        }
    };

    inline static CSetObjectListener* create_ObjSuperRingSuper() { return new ObjSuperRingSuper(); }

    inline static CObjInfo* createObjInfo_ObjSuperRingSuperInfo(csl::fnd::IAllocator* pAllocator)
    {
        return new(pAllocator) ObjSuperRingSuperInfo();
    }
}