#pragma once

#include "Boss_ImpactMan.h"

namespace Boss
{

	static void __fastcall InitBossDecisionHooks()
	{
		if (g_pImpactMan)
			g_pImpactMan->Hook();

		// for each one we would just call g_pBossName->Hook();
	}
};
