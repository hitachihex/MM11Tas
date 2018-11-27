#pragma once


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

	__declspec(noinline) static unsigned long __fastcall Shift(ActionTimeValues * pAtv)
	{
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
};