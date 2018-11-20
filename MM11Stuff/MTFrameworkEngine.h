#pragma once

#define ENGINE_ADDRESS_140A0E098 0x140A0E098
#define ENGINE_PERFORMANCE_FREQUENCY_ADDRESS 0x140C4B778

static double * g_EngineUnknownDouble = (double*)(ENGINE_ADDRESS_140A0E098);
static unsigned long long * g_EnginePerformanceFrequency = (unsigned long long*)(ENGINE_PERFORMANCE_FREQUENCY_ADDRESS);

#pragma pack(push, 1)
class MTFrameworkEngine
{
	// 0x00 - 0x07
	unsigned long long * vtable;


	// 0x40078 - Performance Frequency.
	unsigned long long getPerformanceFrequency()
	{
		return *(unsigned long long*)((unsigned long)this + 0x40078);
	}

	// 0x40048 - Current or Previous Performance Counter?
	unsigned long long getPerformanceCounter()
	{
		return *(unsigned long long*)((unsigned long)this + 0x40048);
	}



	// call Game.GetPerformanceFrequency
	// movsd xmm7, qword ptr ds:[ENGINE_ADDRESS_140A0E098]
	// xorps xmm1, xmm1
	// movaps xmm0, xmm7
	// mov qword ptr ds:[rbx+0x40078], rax
	// cvtsi2sd xmm1, rax
	// divsd xmm0, xmm1
	// movsd qword ptr ds:[rbx+0x40070], xmm0
	// 0x40070 - ?? Something to do with g_EngineUnknownDouble
};
#pragma pack(pop)
