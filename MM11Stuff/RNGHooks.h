#pragma once

#include "Boss_ImpactMan.h"

// Tinker tailor, soldier sailor

#define TINKER_TAILOR_SOLDIER_SAILOR 1

#define IMPACTMAN_SHIFTRNG_RETURNADDRESS 0x00000001400EBA02
namespace RNG
{
	extern HOOK_TRACE_INFO ShiftRNG_HookHandle;
	extern const unsigned long long g_OriginalShiftRNG;
	typedef unsigned long long(__cdecl * ___asm)();

	extern ___asm asm_GetRBX;        //= (___asm)(0x0);
	extern ___asm asm_GetRetRet;     //= (___asm)(0x0);

	extern void InitHook();

	// r8  = rcx
#pragma pack(push, 1)
	typedef struct t_ActionTimeValues
	{
		// 0x00 - 0x03
		unsigned long A;

		// 0x04 - 0x7
		unsigned long B;
		
		// 0x08 - 0x0B
		unsigned long C;

		// 0x0C - 0x0F
		unsigned long D;
	} ActionTimeValues;
#pragma pack(pop)

	// Impact Man decides his next action based on this returning even or odd - (and ebx, 0x01) @ 0x1401972F7
	// So an odd number means he dashes, and an even number means he jumps.
	/*__declspec(noinline)*/ extern unsigned long __fastcall Shift_Hook(ActionTimeValues*);
	/*
	{

		// We sub rsp, 0x20 for _ImpactManInstance currently.
		// Need to change this when we add more.

		//48 83 C4 28 - add rsp, 0x28
		//48 83 EC 28 - sub rsp, 0x28

		// add rsp, 0x28
		// mov rax, dword ptr ss:[rsp]
		// sub rsp, 0x28
		// ret
		const char * p = "\x48\x83\xC4\x28\x48\x8B\x04\x24\x48\x83\xEC\x28\xC3";
		
		// mov rax, rbx
		// ret
		const char * GetRBX = "\x48\x8B\xC3\xC3";


		*(unsigned long long*)(&asm_GetRetRet) = (unsigned long long)p;
		*(unsigned long long*)(&asm_GetRBX) = (unsigned long long)GetRBX;

		// Get our return address.
		unsigned long long RetRetRet = asm_GetRetRet();//((unsigned long long(__cdecl*)())p)();

		if (RetRetRet == 0x00000001400EBA02)
		{
			// Need RBX as the ImpactMan object instance.
			unsigned long long RBX = asm_GetRBX();//((unsigned long long(__cdecl*)())GetRBX)();
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

		// Temp
		return 0x0;

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

		return eax;
	}
	*/
};