#include "stdafx.h"
#include "RNGHooks.h"

namespace RNG
{
	___asm asm_GetRBX = (___asm)(0x0);
	___asm asm_GetRetRet = (___asm)(0x0);

	const unsigned long long g_OriginalShiftRNG = 0x000000014032DE50;
	HOOK_TRACE_INFO ShiftRNG_HookHandle = { NULL };


	void InitHook()
	{
		NTSTATUS result = AddHook((void*)g_OriginalShiftRNG, Shift_Hook, NULL, &ShiftRNG_HookHandle);

		if (FAILED(result))
		{
			auto err = RtlGetLastErrorString();
			DebugOutputW(L"RNG::InitHook encountered an error -> %s", err);
			return;
		}
		ExclusiveHook(&ShiftRNG_HookHandle);
	}

	unsigned long  __fastcall Shift_Hook(ActionTimeValues * pAtv)
	{
		DoOnceBlock("Shift_Hook, !bOnce");

		// We sub rsp, 0x28 in our current hook method. 
		// Need to change this when we add more local vars, etc.

		//48 83 C4 28 - add rsp, 0x28
		//48 83 EC 28 - sub rsp, 0x28

		// add rsp, 0x28
		// mov rax, dword ptr ss:[rsp]
		// sub rsp, 0x28
		// ret
#ifndef TINKER_TAILOR_SOLDIER_SAILOR
		const char * p = "\x48\x83\xC4\x28\x48\x8B\x04\x24\x48\x83\xEC\x28\xC3";

		// mov rax, rbx
		// ret
		const char * GetRBX = "\x48\x8B\xC3\xC3";


		*(unsigned long long*)(&asm_GetRetRet) = (unsigned long long)p;
		*(unsigned long long*)(&asm_GetRBX) = (unsigned long long)GetRBX;

		// Get our return address.
		unsigned long long RetRet = asm_GetRetRet();
		//((unsigned long long(__cdecl*)())p)();

		if (RetRet == IMPACTMAN_SHIFTRNG_RETURNADDRESS)
		{
			// Need RBX as the ImpactMan object instance.
			unsigned long long RBX = asm_GetRBX();
			//((unsigned long long(__cdecl*)())GetRBX)();

			Boss::_ImpactManInstance * pInstance = (Boss::_ImpactManInstance*)(RBX);

			switch (pInstance->m_CurrentActionStep)
			{
			case 0x00:
				break;
			case 0x01:
				break;
			case 0x02:
				break;
			case 0x03:
				break;
			default:
				break;
			}
		}


		return ((unsigned long(__fastcall*)(ActionTimeValues*))g_OriginalShiftRNG)(pAtv);
#endif
		if (g_pPlaybackManager && g_pPlaybackManager->IsPlayingBack() && !g_pPlaybackManager->m_bLoading)
		{
			pAtv->A = g_pPlaybackManager->m_CurrentFrame; 
			
			// B??
			pAtv->B = g_pPlaybackManager->m_CurrentFrame; //  left 16 bits of input state ?
			// C??
			pAtv->C = g_pPlaybackManager->m_CurrentFrame; // right 16 bits of input state ?

			pAtv->D = g_pPlaybackManager->GetTotalFrameCount();
		}

		return ((unsigned long(__fastcall*)(ActionTimeValues*))g_OriginalShiftRNG)(pAtv);
		
		/*
		// mov eax, dword ptr ds:[rcx+0x4]
		unsigned long eax = pAtv->B;

		// mov r8, rcx
		ActionTimeValues *r8 = pAtv;

		// mov edx, dword ptr ds:[rcx]
		unsigned long edx = pAtv->A;

		// shl edx, 0x0f
		edx <<= 0x0F;

		// xor edx, dword ptr ds:[rcx]
		edx ^= pAtv->A;

		// mov dword ptr ds:[rcx], eax
		pAtv->A = eax;

		// mov eax, dword ptr ds:[rcx+0x08]
		eax = pAtv->C;

		// mov dword ptr ds:[rcx+0x4], eax
		pAtv->B = eax;

		// mov ecx, dword ptr ds:[rcx+0x0C]
		unsigned long ecx = pAtv->D;

		// mov eax, ecx
		eax = ecx;

		// shr eax, 0x11
		eax >>= 0x11;

		// xor eax, edx
		eax ^= edx;

		// mov dword ptr ds:[r8+0x08], ecx
		r8->C = ecx;

		// shr eax, 0x04
		eax >>= 0x04;

		// xor eax, ecx
		eax ^= ecx;

		// xor eax, edx
		eax ^= edx;

		// mov dword ptr ds:[r8+0x0C], eax
		r8->D = eax;

		return eax;*/
	}
};