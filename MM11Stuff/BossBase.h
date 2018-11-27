#pragma once

#include "MM11.h"


namespace Boss
{
#pragma pack(push, 1)

	class BossBase;

	// RCX, RDX
	typedef void(__fastcall *  BossDecisionHandler)(void*, unsigned long long);

	class BossBase
	{
	public:

		BossBase() = delete;
		BossBase(unsigned long long, BossDecisionHandler);
		virtual bool Hook();


		BossDecisionHandler m_DecisionHandler;

		BossDecisionHandler m_Original;

		unsigned long long m_OriginalAddress;

		HOOK_TRACE_INFO m_HookHandle;
	protected:

	private:
		
	};


#pragma pack(pop)
};


