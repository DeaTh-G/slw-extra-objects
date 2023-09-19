#pragma once

namespace slw_extra_objects
{
	class objectObjectReflections
	{
	public:
		inline static app::CSetObjClass SUPER_RING_SUPER_CLASS{ app::CSetObjClass::Create<slw_extra_objects::ObjSuperRingSuper, slw_extra_objects::ObjSuperRingSuperInfo>("SuperRingSuper", "OBJECT", "Item") };
		inline static app::CSetObjClass DASH_RING_CLASS{ app::CSetObjClass::Create<slw_extra_objects::ObjDashRing, slw_extra_objects::ObjDashRingInfo>("DashRing", "OBJECT", "Gimmick", slw_extra_objects::paramMap_DashRing) };
		inline static app::CSetObjClass JUMP_PANEL_CLASS{ app::CSetObjClass::Create<slw_extra_objects::ObjJumpPanel, slw_extra_objects::ObjJumpPanelInfo>("JumpPanel", "OBJECT", "Gimmick", slw_extra_objects::paramMap_JumpPanel) };
		inline static app::CSetObjClass JUMP_BOARD_EX_CLASS{ app::CSetObjClass::Create<slw_extra_objects::ObjJumpBoardEx, slw_extra_objects::ObjJumpBoardExInfo>("JumpBoardEx", "OBJECT", "Gimmick", slw_extra_objects::paramMap_JumpBoardEx) };
		inline static app::CSetObjClass WIDE_SPRING_CLASS{ app::CSetObjClass::Create<slw_extra_objects::ObjWideSpring, slw_extra_objects::ObjWideSpringInfo>("WideSpring", "OBJECT", "Gimmick", slw_extra_objects::paramMap_WideSpring) };
		inline static app::CSetObjClass UP_REEL_CLASS{ app::CSetObjClass::Create<slw_extra_objects::ObjUpReel, slw_extra_objects::ObjUpReelInfo>("UpReel", "OBJECT", "Gimmick") };
	};
}