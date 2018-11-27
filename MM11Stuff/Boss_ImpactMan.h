#pragma once

#include "BossBase.h"
#include "PlaybackManager.h"

namespace Boss
{

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