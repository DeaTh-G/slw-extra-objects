#pragma once

namespace TestingGrounds
{
	class FileLoader
	{
	public:
		inline static FUNCTION_PTR(void, __thiscall, LoadFile, ASLR(0x00490D90), app::fnd::FileLoader* This, int* a2, const char* fileName, app::fnd::FileLoaderParam* loaderParam);
	};

	class CSetObjectFactory
	{
	public:
		static void InstallPatches();
	};

	class StageDataInitializer
	{
	public:
		static void InstallPatches();
	};

	class CStateBase
	{
	public:
		static void InstallPatches();
	};

	class MsgExtendPlayer : public app::fnd::Message
	{
		inline const static unsigned int MessageID = 0x300F;
	
	public:
		int field_18{};
		int field_1C{};

		MsgExtendPlayer() : Message(MessageID)
		{

		}

		MsgExtendPlayer(int a1, int a2) : MsgExtendPlayer()
		{
			field_18 = a1;
			field_1C = a2;
		}
	};
}