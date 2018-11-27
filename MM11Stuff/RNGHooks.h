#pragma once

#include "Boss_ImpactMan.h"

// Tinker tailor, soldier sailor
namespace RNG
{
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
	__declspec(noinline) static unsigned long __fastcall Shift(ActionTimeValues * pAtv)
	{

		// We sub rsp, 0x20 for _ImpactManInstance currently.
		// Need to change this when we add more.

		//48 83 C4 20 - add rsp, 0x20
		//48 83 EC 20 - sub rsp, 0x20

		// add rsp, 0x20
		// mov rax, dword ptr ss:[rsp]
		// sub rsp, 0x20
		// ret
		const char * p = "\x48\x83\xC4\x20\x48\x8B\x04\x24\x48\x83\xEC\x20\xC3";
		
		// mov rax, rbx
		// ret
		const char * GetRBX = "\x48\x8B\xC3\xC3";

		// Get our return address.
		unsigned long long RetRetRet = ((unsigned long long(__fastcall*)())p)();

		if (RetRetRet == 0x00000001400EBA02)
		{
			// Need RBX as the ImpactMan object instance.
			unsigned long long RBX = ((unsigned long long(__fastcall*)())GetRBX)();
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