#include "stdafx.h"
#include "Boss_ImpactMan.h"

namespace Boss
{
	Boss::Impact_Man::Impact_Man(unsigned long long orig, BossDecisionHandler Fun, std::wstring hkInfo) : BossBase(orig, Fun)
	{
		DebugOutput("Boss::Impact_Man::Impact_Man : orig = %llx, fun = %llx", orig, (unsigned long long)Fun);

		//std::string_view at earliest convenience pls
		this->m_HookInfo = hkInfo;
	}

	bool Boss::Impact_Man::Hook()
	{
		bool baseResult = BossBase::Hook();

		if (!baseResult)
		{
			auto err = RtlGetLastErrorString();
			DebugOutputW(L"%s encountered an error from Base.Hook -> %s", this->m_HookInfo.c_str(), err);
			return false;
		}
		
		DebugOutputW(L"Succesfully hooked %s.", this->m_HookInfo.c_str());
		return true;
	}

	Impact_Man * g_pImpactMan = new Impact_Man(0x0000000140197250, ImpactMan_DecisionHandler_Hook, L"ImpactMan_DecisionHandler_Hook");

	void __fastcall ImpactMan_DecisionHandler_Hook(void* RCX, unsigned long long RDX)
	{

		// This should be impossible
		if (g_pImpactMan == nullptr)
			return;

		// movzx ebp, dl
		unsigned char ebp = (unsigned char)RDX;

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

		_ImpactManInstance * pInstance = (_ImpactManInstance*)(RCX);

		std::string iniFile = g_pPlaybackManager->m_CWD;
		iniFile += "ImpactMan.ini";

		unsigned int actionOne = GetPrivateProfileIntA("ImpactMan", "ActionOne",     1, iniFile.c_str());
		unsigned int actionTwo = GetPrivateProfileIntA("ImpactMan", "ActionTwo",     1, iniFile.c_str());
		unsigned int actionThree = GetPrivateProfileIntA("ImpactMan", "ActionThree", 0, iniFile.c_str());
		unsigned int actionFour = GetPrivateProfileIntA("ImpactMan", "ActionFour",   1, iniFile.c_str());

		//cwd += "ImpactMan.ini";
		DebugOutput("ImpactMan::DecideAction -> a1=%u, a2=%u, a3=%u, a4=%u", actionOne, actionTwo, actionThree, actionFour);

		// Ok:
		// mov eax, action @ 1401972EA
		// and nop:
		// 1401972F7 for 3 bytes.

		// MOV EAX, ..
		*(unsigned char*)(0x1401972EA) = 0xB8;

		switch (pInstance->m_CurrentActionStep)
		{
		case 0x00:
			*(unsigned char*)(0x1401972EB) = actionOne;
			break;
		case 0x01:
			*(unsigned char*)(0x1401972EB) = actionTwo;
			break;
		case 0x02:
			*(unsigned char*)(0x1401972EB) = actionThree;
			break;
		case 0x03:
			*(unsigned char*)(0x1401972EB) = actionFour;
			break;
		default:
			break;
		}

		// The other 3 bytes need to 0.
		*(unsigned char*)(0x1401972EC) = 0x00;
		*(unsigned char*)(0x1401972ED) = 0x00;
		*(unsigned char*)(0x1401972EE) = 0x00;

		// and ebx, 0x01 - nop this crap.
		*(unsigned char*)(0x1401972F7) = 0x90;
		*(unsigned char*)(0x1401972F8) = 0x90;
		*(unsigned char*)(0x1401972F9) = 0x90;


		// Call original.
		g_pImpactMan->m_Original(RCX, RDX);
	}
};