#pragma once

namespace SLW_EXTRA_OBJECTS
{
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

	namespace Player
	{
		class SonicInfo
		{
		public:
			static void InstallPatches();
		};
	}
}