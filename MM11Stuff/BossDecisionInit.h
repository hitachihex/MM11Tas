#pragma once

#include "Boss_ImpactMan.h"
#include "RNGHooks.h"

namespace Boss
{

	static void __fastcall InitBossDecisionHooks()
	{
#ifdef BOSS_PATTERN_HOOKS
		// for each one we would just call g_pBossName->Hook();
		if (g_pImpactMan)
			g_pImpactMan->Hook();
#elif defined SEED_TWIST_HOOKS
		RNG::InitHook();
#else
		DebugOutput("InitBossDecisionHooks - neither BOSS_PATTERN_HOOKS or SEED_TWIST_HOOKS are defined.");
#endif
	}
};
