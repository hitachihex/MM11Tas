#pragma once

#include "BossBase.h"
#include "PlaybackManager.h"

namespace Boss
{
#pragma pack(push, 1)
	struct _ImpactManInstance
	{
		// 0x0000 - 0x32B1
		unsigned char m_ucUnknown[0x32B2 - 0x0];

		// 0x32B2
		bool m_bUnk01;

		// 0x32B3
		bool m_bUnk02;

		// 0x32B4 - 0x32B7
		unsigned long m_dwUnk00;

		// 0x32B8 - 0x32BB
		unsigned int m_RepeatedActionCount; // -- He can't do more than 2 of the same actions in a row (unless we change this, and that would be cheating.)

		// 0x32BC - 0x32BF
		// If his m_CurrentActionStep == 4 , then  he goes into stake mode, or powerdrive is his HP is <= 50%.
		unsigned int m_CurrentActionStep; // -- where we are in his cycle

	};
#pragma pack(pop)

	class Impact_Man : public Boss::BossBase
	{
	public:
		Impact_Man(unsigned long long, BossDecisionHandler, std::wstring);
		bool Hook();
	protected:
	private:
		std::wstring m_HookInfo;

	};

	extern Impact_Man * g_pImpactMan;
	extern void __fastcall ImpactMan_DecisionHandler_Hook(void*, unsigned long long);
};