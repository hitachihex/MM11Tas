#include "stdafx.h"
#include "D3D11Hooks.h"
#include "MM11.h"
#include "Game.h"
#include "PlaybackManager.h"
#include <timeapi.h>
#include "BossDecisionInit.h"
#include "RNGHooks.h"


#pragma comment(lib, "winmm.lib")
HOOK_TRACE_INFO MM11_CheckInputState04_HookHandle = { NULL };
unsigned long g_ACLEntries[1];

extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo);

float * g_fGlobalGameSpeed = nullptr;
bool g_bPlaybackSync = false;
bool g_bDidFrameStep = false;
unsigned long long g_llGameLoopRcx = 0x0;

// testing forced speedup through hooks.
unsigned long g_dwBaseTime = 0x0;
unsigned long g_dwTickCount = 0x0;
int64_t g_dwPerformanceCount = 0x0;
unsigned long g_dwGameSpeed = 1;

#ifdef RANDTEST
std::random_device g_RandTest;
#endif


oCheckInputState04 original_CheckInputState04 = (oCheckInputState04)(MPGAME_GETINPUTSTATE04_ADDRESS);
oHandleGameSpeed original_HandleGameSpeed = (oHandleGameSpeed)(HANDLEGAMESPEED_ADDRESS);
oHandleGameSpeed2 original_HandleGameSpeed2 = (oHandleGameSpeed2)(HANDLEGAMESPEED02_ADDRESS);
oComInitOriginal00 original_ComInit00 = (oComInitOriginal00)(COMINIT00_ADDRESS);


oQueryPerformanceCounter original_QueryPerformanceCounter = (oQueryPerformanceCounter)(*(unsigned long long*)QPC_IAT_ADDRESS);
oTimeGetTime             original_TimeGetTime = (oTimeGetTime)(*(unsigned long long*)TIMEGETTIME_IAT_ADDRESS);
oGetTickCount            original_GetTickCount = (oGetTickCount)(*(unsigned long long*)GETTICKCOUNT_IAT_ADDRESS);

HOOK_TRACE_INFO MM11_HandleGameSpeed_HookHandle = { NULL };
HOOK_TRACE_INFO MM11_HandleGameSpeed2_HookHandle = { NULL };

HOOK_TRACE_INFO MM11_MTObjectComInit00_HookHandle = { NULL };
HOOK_TRACE_INFO MM11_MTObjectComInit01_HookHandle = { NULL };
HOOK_TRACE_INFO MM11_MTObjectComInit02_HookHandle = { NULL };
HOOK_TRACE_INFO MM11_MTObjectComInit03_HookHandle = { NULL };


void DoInitRoutine(const char * pcszNameBind, empty_init_Routine init_routine)
{
	DebugOutput("Init %s", pcszNameBind);
	init_routine();
	DebugOutput("Init %s done.", pcszNameBind);
}

void __fastcall InitRNGHooks()
{
	g_dwBaseTime = timeGetTime();
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&g_dwPerformanceCount));

	*(unsigned long long*)(TIMEGETTIME_IAT_ADDRESS) = (unsigned long long)TimeGetTime_Hook;
	*(unsigned long long*)(QPC_IAT_ADDRESS) = (unsigned long long)QueryPerformanceCounter_Hook;
}

void __fastcall InitFastForward()
{
	g_dwTickCount = GetTickCount();
	g_dwBaseTime = timeGetTime();
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&g_dwPerformanceCount));

	*(unsigned long long*)(TIMEGETTIME_IAT_ADDRESS) = (unsigned long long)TimeGetTime_Hook;
	*(unsigned long long*)(QPC_IAT_ADDRESS) = (unsigned long long)QueryPerformanceCounter_Hook;
}

unsigned long __fastcall TimeGetTime_Hook()
{
	DoOnceBlock("timeGetTime hook ok");

	auto curGetTime = original_TimeGetTime();
	unsigned long result = 0;
	// ??
#ifdef FASTFORWARD_HOOK_TEST
	result = g_dwBaseTime + ((curGetTime - g_dwBaseTime) * g_dwGameSpeed);
#else
	result = g_dwBaseTime + ((curGetTime - g_dwBaseTime) * 1);
#endif

#ifdef RADICAL_ED
	///result = 0;
#endif
	return result;


}

unsigned long __fastcall GetTickCount_Hook()
{
	DoOnceBlock("GetTickCount hook, !bOnce");

	auto curTickCount = original_GetTickCount();
	return g_dwTickCount + ((curTickCount - g_dwTickCount) * g_dwGameSpeed);
}

bool __fastcall QueryPerformanceCounter_Hook(LARGE_INTEGER * pPerformanceCounter)
{
	DoOnceBlock("QueryPerformanceCounter hook, !bOnce");

	int64_t curPerfCounter;
	if (!original_QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&curPerfCounter)))
		return false;

	int64_t newTime = 0; 
#ifdef FASTFORWARD_HOOK_TEST
	newTime = curPerfCounter + ((curPerfCounter - g_dwPerformanceCount) * g_dwGameSpeed);
#else 
	// This will probably explode everything, but  /shrug
	newTime = curPerfCounter + ((curPerfCounter - g_dwPerformanceCount) * 1000);
#endif

	*pPerformanceCounter = *reinterpret_cast<LARGE_INTEGER*>(&newTime);
	
	return true;
}

/*
LRESULT CALLBACK MainWindowProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	//DebugOutput("MWP.Msg = %u", uMsg);

	if (uMsg == WM_KEYUP)
		DebugOutput("KeyUp  WinProcHook");

	if (uMsg == WM_KEYDOWN)
		DebugOutput("KeyDown WinProcHook");

	if (uMsg == WM_SYSKEYDOWN)
		DebugOutput("SysKeyDown WinProcHook");

	if (uMsg == WM_SYSKEYUP)
		DebugOutput("SysKeyUp WinProcHook");

	if (uMsg == WM_CHAR)
	{
		DebugOutput("WM_CHAR , Key==%X", wParam);
	}

	return original_MainWindowProc(hWnd, uMsg, wParam, lParam);
}

#pragma warning(disable : 4996)
HRESULT __fastcall DirectInputDevice_GetDeviceState_Hook(IDirectInputDevice*pDev, unsigned long cbSize, LPVOID lpvData)
{
	static bool bOnce = false;
	if (!bOnce)
	{
		DebugOutput("hi from DID_GDS_Hook");
		bOnce = true;
	}

	unsigned int nLen = (cbSize / 20);
	DIDEVICEOBJECTDATA *pKeys = (DIDEVICEOBJECTDATA*)lpvData;


	long ret =  original_DirectInputDevice_GetDeviceState(pDev, cbSize, lpvData);
	unsigned char * pData = (unsigned char*)lpvData;

	FILE *fp = fopen("dumpDI8Data.txt", "a");

	static unsigned long long callCount = 0;
	if (fp != nullptr)
	{
		if (cbSize != 0x110)
		{
			fprintf(fp, "CallCount:%u,CbSize=%u,Data:", callCount, cbSize);
			for (unsigned int i = 0; i < cbSize; i++)
				fputc((int)pData[i], fp);

			fputc('\n', fp);
		}
		
	}


	fclose(fp);

	callCount++;

	return 0;
}

ATOM __fastcall RegisterClassExW_Hook(WNDCLASSEXW* pWc)
{

	//DebugOutputW(L"RegClassExW_Hook: ClassName=%s, WndProcAddr=%X", pWc->lpszClassName, pWc->lpfnWndProc);

	std::wstring sClassName = std::wstring(pWc->lpszClassName);

	DebugOutput("ClassName=%s", pWc->lpszClassName);

	if (sClassName.find(L"MT FRAMEWORK") != std::string::npos)
	{
		*(unsigned long long*)(&original_MainWindowProc) = (unsigned long long)pWc->lpfnWndProc;
		pWc->lpfnWndProc = MainWindowProc_Hook;

		DebugOutput("Hooked WindowProc for MT FRAMEWORK window");

	}
	return original_RegisterClassExW(pWc);
}
void HookDirectInputMethods()
{
	static bool haveHooked = false;

	unsigned long long rax = *(unsigned long long*)MPGAME_INPUT_POINTER;
	if (!rax) return;
	unsigned long long rdi = *(unsigned long long*)(rax + 0x0 * 0x8 + 0xBF0);
	if (!rdi) return;

	rax = *(unsigned long long*)rdi;
	//rcx=rdi=__thisptr
	// call qword ptr ds:[rax+0xC8] == ??


	if (!haveHooked)
	{
		DWORD dwOldProt;
		VirtualProtect((LPVOID)&*(unsigned long long*)(rax + 0x48), 0x8, PAGE_EXECUTE_READWRITE, &dwOldProt);

		*(unsigned long long*)(&original_DirectInputDevice_GetDeviceState) = *(unsigned long long*)(rax + 0x48);
		//*(unsigned long long*)(&original_DirectInputDevice_GetDeviceState) = (unsigned long long)DirectInputDevice_GetDeviceState_Hook;

		*(unsigned long long*)(rax + 0x48) = (unsigned long long)DirectInputDevice_GetDeviceState_Hook;
	}

	haveHooked = true;

}

*/

void _FuckYourLimiter()
{
	//0000000148E3270D

	// Forcing this to 1.0 for part of the update loop? Hmm
	// mov dword ptr ds:[rcx+0x80], 0x3F800000

	unsigned char * pMem = (unsigned char*)(0x0000000148E3270D);
	
	for (unsigned int i = 0; i < 10; i++)
		*(pMem + i) = 0x90;

}

// Handles megaman's speed only.
void __fastcall HandleGameSpeed2_Hook(unsigned long long rcx)
{
	DoOnceBlock("HandleGameSpeed2_Hook, !bOnce");

	//float * pSpeedMultiplier = (float*)(rcx + 0x28);
	//*pSpeedMultiplier = 1.0f;
	return original_HandleGameSpeed2(rcx);
}

// I'm not sure what this even does??
unsigned long __fastcall HandleGameSpeed_Hook(unsigned long long rcx, unsigned long long rdx, unsigned long long r8, unsigned long long r9)
{
	DoOnceBlock("HandleGameSpeed_Hook, !bOnce");

	// *rdx = 1.0, 	No matter what!
	//float * pSpeedMultiplier = (float*)rdx;
	//*pSpeedMultiplier = 1.0F;
	return original_HandleGameSpeed(rcx, rdx, r8, r9);
}

void __fastcall MTObjectComInit_Hook00(unsigned long long rcx, unsigned long long rdx)
{
#pragma pack(push, 1)
	struct Foobar
	{

		// 0x00 - 0x03
		unsigned long * vtbl;

		// 0x04 - 0x72
		unsigned char m_ucUnknown04_72[0x70 - 0x04];

		// 0x70 - 0x73
		float m_Scale;

		// 0x74 - 0x10F
		unsigned char m_ucUnknown0074_0113[0x110 - 0x74];

		// 0x110 - 0x113
		float m_BaseFPS;

		// 0x114 - 0x117
		float m_DeltaTimeRate;

		// 0x118 - 0x11B
		unsigned long m_dwUnknown00;

		// 0x11C - 0x11F
		float m_TimeInterpolationRate;

	};
#pragma pack(pop)

	Foobar *pFoobar = (Foobar*)(rcx);

	//pFoobar->m_Scale = 50.0f;
	//pFoobar->m_BaseFPS = 1000.0f;
	//pFoobar->m_TimeInterpolationRate = 1000.0f;
	return original_ComInit00(rcx, rdx);
}

void __fastcall _FixIATRehook()
{
	//000000014D0311D1 - denuvo'd
	//000000014092234D - drm free
#define IATREHOOK_ADDRESS 0x000000014092234D
	unsigned char * pMem = (unsigned char*)(0x000000014092234D);
	
	DWORD dwOldProt;
	VirtualProtect((LPVOID)IATREHOOK_ADDRESS, 0x8, PAGE_EXECUTE_READWRITE, &dwOldProt);

	*(pMem + 0x0) = 0x90;
	*(pMem + 0x1) = 0x90;
	*(pMem + 0x2) = 0x90;
	*(pMem + 0x3) = 0x90;

}


static void ChangeGameSpeed(float f, bool reset = false)
{
#ifdef FASTFORWARD_HOOK_TEST
	g_dwGameSpeed += (unsigned long)f;
	DebugOutput("GameSpeed is now: %u", g_dwGameSpeed);
#else
	float * pfTimeStep = (float*)(0x1409C3A34);
	unsigned long long rcx = *(unsigned long long*)(MPENVIRONMENTVARIABLES_ADDRESS);
	if (!rcx) return;

	g_fGlobalGameSpeed = (float*)(rcx + 0x8C);

	if (reset) {
		*g_fGlobalGameSpeed = 1.0;
		return;
	}

	if (*g_fGlobalGameSpeed == 1 && f < 1)
	{
		g_bPaused = true;
		return;
	}

	*g_fGlobalGameSpeed += (f);
#endif


	/*
	if (reset)
	{
		*pfTimeStep = 3.0f;
		return;
	}*/

	//float *pfEngineUpdateRate = (float*)(rcx + 0x80);
	//*pfEngineUpdateRate += (f);

	// So what if we only change the timestep??
	//*pfTimeStep += (f * 3.0f);
	/*
	float *pTest = (float*)(rcx + 0x80);

	// Ok this.. is zoom, or something. Lol
	float *pZoom_Q = (float*)(0x00000001409BE564);

	// The engine timestep!
	float *pTest2 = (float*)(0x1409C3A34);

	if (reset) {
		//*pZoom_Q = 1.0f;
		*pTest = 1.0f;
		*pTest2 = 3.0f;
		*g_fGlobalGameSpeed = 1.0;
		return;
	}


	// Just do multiples of 3??
	//*pTest2 += (f * 3.0f);
	//*pZoom_Q += (f);
	*pTest += (f);
	*g_fGlobalGameSpeed += (f);
	// so.. change teh fps to 120??
	float *pFps = (float*)(rcx + 0x50);
	*pFps = 120.0;*/


}

/*
void ThreadProc()
{
	//DebugOutput("hi from threadProc");

	while (true)
	{
		if (GetAsyncKeyState(VK_ADD) & 1)
		{
			ChangeGameSpeed(1.0f);
		}


		if (GetAsyncKeyState(VK_SUBTRACT) & 1)
		{
			ChangeGameSpeed(-1.0f);

		}
		Sleep(10);
	}
}*/


void ForceGameOver(unsigned long long argRcx)
{
	//unsigned long long *_RBX = (unsigned long long*)(&*(unsigned long long*)((unsigned long)argRcx + 0x7048));
	//unsigned long long _RCX = *_RBX;

#define CallGameOver(e) ((void(__thiscall*)(unsigned long long))0x140075E28)(e);

	CallGameOver(argRcx);
}

//#pragma optimize("", off)
void __fastcall GameLoop_Hook(unsigned long long ecx, unsigned long long edx)
{
	MTFramework::MainGame * _this = (MTFramework::MainGame*)(ecx);
	DoOnceBlock("GameLoop_Hook, !bOnce");

	g_llGameLoopRcx = ecx;
	if (g_pPlaybackManager)
	{
		auto pGameState = MTFramework::GetGameState();
		if (pGameState)
		{
			//g_pPlaybackManager->m_bLoading = MTFramework::GetGameState();

			if (!strcmpi(MTFramework::GetGameState()->m_szMenuState, "aBriefing"))
				g_pPlaybackManager->m_bLoading = true;
			else
				g_pPlaybackManager->m_bLoading = false;

		}

	}


	if (GetAsyncKeyState(VK_F1) & 1 && !g_bPaused)
	{
		// Don't let original update get called on the frame we pressed the bind.
		g_bPaused = true;
		DebugOutput("Paused.");
		return;
	}

	if (GetAsyncKeyState(VK_F4) & 1)
	{
		if (g_pPlaybackManager->IsPlayingBack())
		{
			memset(&g_pPlaybackManager->m_szCurrentManagerState[0], 0, 120);
			g_pPlaybackManager->InitPlayback(true);
			g_bPlaybackSync = false;
		}
		else
		{
			memset(&g_pPlaybackManager->m_szCurrentManagerState[0], 0, 120);
			DebugOutput("Starting playback.");
			g_pPlaybackManager->InitPlayback(false);
			DebugOutput("InitPlayback success.");
			g_bPlaybackSync = true;

			DebugOutput("Toggled g_bPlaybackSync.");
		}
	}
	
#ifdef RADICAL_ED

	if (GetAsyncKeyState(VK_F5) & 1)
	{
		RNG::ActionTimeValues * pValues = new RNG::ActionTimeValues();
		pValues->A = rand();
		pValues->B = rand();
		pValues->C = rand();
		pValues->D = rand();

		unsigned long result = RNG::Shift(pValues);

		DebugOutput("RNG::Shift result = %u", result);
		/*
		auto pGameState = MTFramework::GetGameState();
		if (pGameState)
		{
			DebugOutput("GameState string: %s", pGameState->m_szMenuState);
		}*/


		/*
		auto pColMgr = &_this->m_pCollision->m_CollManager;
		for (unsigned int i = 0; i < pColMgr->m_NumObjects; i++)
		{
			auto obj = &*pColMgr->m_pCollObjects[i];
			auto data = obj->m_pObjInstance->m_pObjectData;

			if (data && obj->m_pObjInstance->m_bUnk0D)
			{
				DebugOutput("Obj %u is at X %f", i, data->m_PositionX);
			}
		}*/


		/*
		auto m_pCamera = _this->m_pCamera;
		for (unsigned int i = 0; i < 9; i++)
		{
			auto pCamera = &m_pCamera->m_Cameras[i];

			if (pCamera->m_pScreenStuff)
			{
				pCamera->m_DisplayType = MTFramework::eCameraDisplayType::CDT_BOTTOM_STRETCHED;
			}
		}*/

		/*
		auto m_pUnit = _this->m_pUnit;

		auto ptr = *(unsigned long long*)(MTFramework::Unit::_UnitManagerPointer);
		unsigned long objectCount = *(unsigned long*)(ptr + MTFramework::Unit::NumObjectsToIterateOffset);

		for (auto i = 0; i < objectCount; i++)
		{
			auto pObject = &m_pUnit->m_pObjects[i];
			DebugOutput("Vtable Addr of m_pUnit->m_pObjects[%u] is %llx\n", pObject->m_Vtable);
		}*/

	}
#endif


	// Game speed hotkey, increase
	if (GetAsyncKeyState(VK_ADD) & 1)
	{
		ChangeGameSpeed(1.0f);
	}

	// Game speed hotkey, decrease
	if (GetAsyncKeyState(VK_SUBTRACT) & 1)
	{
		ChangeGameSpeed(-1.0f);
	}

	// Back to normal speed
	if (GetAsyncKeyState(VK_DIVIDE) & 1)
	{
		ChangeGameSpeed(0.0f, true);
	}

	// Only check  unpause or framestep while paused
	if (g_bPaused)
	{
		g_bDidFrameStep = false;
		// Handle unpause case.
		if (GetAsyncKeyState(VK_F1) & 1)
		{
			DebugOutput("Unpaused.");
			g_bPaused = false;
			g_bPlaybackSync = true;
			((void(__thiscall *)(unsigned long long))g_dwOriginalGameLoopAddress)(ecx);
			return;

		}

		// Handle framestepping case.
		if (GetAsyncKeyState(VK_OEM_6) & 1)
		{
			g_bDidFrameStep = true;
			DebugOutput("Stepping.");
			g_bPlaybackSync = true;
			((void(__thiscall *)(unsigned long long))g_dwOriginalGameLoopAddress)(ecx);
			return;
		}

		return;
	}

	if (g_pPlaybackManager->IsPlayingBack())
		g_bPlaybackSync = true;
	else
		g_bPlaybackSync = false;


	((void(__thiscall *)(unsigned long long))g_dwOriginalGameLoopAddress)(ecx);


	if (g_pPlaybackManager->IsPlayingBack())
	{
		InputRecord * pCurrentInput = g_pPlaybackManager->GetCurrentInput();
		if (pCurrentInput->m_Frames == 1 || (pCurrentInput->m_Done == pCurrentInput->m_Frames))
		{
			// Null the gamepad state if we we were on a 1 frame input, or the last frame of an input.

			if (g_pPlaybackManager->GetXInputState() == nullptr)
			{
				DebugOutput("First frame of input file stepped over.");
			}
			else
			{
				if(g_pPlaybackManager->GetXInputState() != nullptr)
					memset(&g_pPlaybackManager->GetXInputState()->Gamepad, 0, sizeof(XINPUT_GAMEPAD));
			}

		}


	}

}
//#pragma optimize("", on)

void __fastcall CheckInputState04_Hook(unsigned long ecx, unsigned long edx)
{
	// ;
	static bool bHooked = false;
	if (*(unsigned long long*)MPGAME_PTR != NULL && !bHooked)
	{
		bHooked = true;
		g_pVtable = (unsigned long long*)(MPGAME_PTR);

		g_dwOriginalGameLoopAddress = g_pVtable[6];

		DWORD dwOldProt;
		VirtualProtect((LPVOID)&g_pVtable[2], 0x8, PAGE_EXECUTE_READWRITE, &dwOldProt);

		g_pVtable[6] = (unsigned long long)GameLoop_Hook;

		DebugOutput("Hooked GameLoop via vtable.");

		*(unsigned long long*)(&original_XInputGetState) = (unsigned long long) GetProcAddress(GetModuleHandle(L"xinput1_3.dll"), "XInputGetState");

		DebugOutput("Original XInputGetState = %llx", (unsigned long long)original_XInputGetState);

	}

	return original_CheckInputState04(ecx, edx);
}

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
	NTSTATUS result = AddHook((void*)MPGAME_GETINPUTSTATE04_ADDRESS, CheckInputState04_Hook, NULL, &MM11_CheckInputState04_HookHandle);

	if (FAILED(result))
	{
		//wchar_t outputBuffer[]
		auto err = RtlGetLastErrorString();
		DebugOutputW(L"Failed to hook GetInputState04 , lastErr=%s", err);

	}
	else
	{
		DebugOutput("GetInputState04 hooked ");
	}
	ExclusiveHook(&MM11_CheckInputState04_HookHandle);

	
#ifdef DEBUG_TESTS
	result = AddHook((void*)HANDLEGAMESPEED_ADDRESS, HandleGameSpeed_Hook, NULL, &MM11_HandleGameSpeed_HookHandle);
	if (FAILED(result))
	{
		auto err = RtlGetLastErrorString();
		DebugOutputW(L"Failed to hook HandleGameSpeed, lastErr = %s", err);
	}
	else
	{
		DebugOutput("HandleGameSpeed hooked.");
	}
	ExclusiveHook(&MM11_HandleGameSpeed_HookHandle);

	result = AddHook((void*)HANDLEGAMESPEED02_ADDRESS, HandleGameSpeed2_Hook, NULL, &MM11_HandleGameSpeed2_HookHandle);
	if (FAILED(result))
	{
		auto err = RtlGetLastErrorString();
		DebugOutputW(L"Failed to hook HandleGameSpeed2, lastErr =  %s", err);
	}
	else
	{
		DebugOutput("HandleGameSpeed2 hooked.");
	}
	ExclusiveHook(&MM11_HandleGameSpeed2_HookHandle);

	result = AddHook((void*)COMINIT00_ADDRESS, MTObjectComInit_Hook00, NULL, &MM11_MTObjectComInit00_HookHandle);
	if (FAILED(result))
	{
		auto err = RtlGetLastErrorString();
		DebugOutputW(L"Failed to hook MTObjectComInit_Hook00, lastErr = %s", err);
	}
	else
	{
		DebugOutput("MTObjectComInit00 hooked.");
	}
	ExclusiveHook(&MM11_MTObjectComInit00_HookHandle);
#endif


	DWORD dwOldProt;
	VirtualProtect((LPVOID)XINPUT_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);
	VirtualProtect((LPVOID)XINPUT_GETCAPS_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);
	//VirtualProtect((LPVOID)REGISTERCLASSEXW_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);
	//VirtualProtect((LPVOID)TIMEGETTIME_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);
	*(unsigned long long*)(XINPUT_IAT_ADDRESS) = (unsigned long long)XInputGetState_Hook;
	*(unsigned long long*)(XINPUT_GETCAPS_IAT_ADDRESS) = (unsigned long long)XInputGetCapabilities_Hook;

	VirtualProtect((LPVOID)TIMEGETTIME_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);
	VirtualProtect((LPVOID)QPC_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);

#ifdef FASTFORWARD_HOOK_TEST
	DoInitRoutine("FastForward Hooks", InitFastForward);
#endif

#ifdef RADICAL_ED
	//DoInitRoutine("RNG Hooks", InitRNGHooks);
#endif


	DoInitRoutine("Fix IAT rehook", _FixIATRehook);

	DoInitRoutine("InitBossDecision Hooks", Boss::InitBossDecisionHooks);
	//DoInitRoutine("FuckYourLimiter",_FuckYourLimiter());

	g_pPlaybackManager = new PlaybackManager("megaman.rec");

	DumpPointersForExternalOSD();

#ifdef RANDTEST
	DebugOutput("Init Random Test");
	std::default_random_engine reng(g_RandTest());
	std::uniform_int_distribution<int> uniform_dist(1, 6);
#endif
}

// shut up c4996
#pragma warning(disable : 4996)
void DumpPointersForExternalOSD()
{
	// Just reference them and %X print to file?
	FILE * pOutFile = nullptr;

	pOutFile = fopen("pointer_ref.txt", "w");

	if (!pOutFile)
	{
		DebugOutput("Couldn't open file handle to dump pointers?");
		return;
	}

	unsigned long long ** pPlaybackManager = (unsigned long long**)&g_pPlaybackManager;
	unsigned long long ** isPlayingBack = (unsigned long long**)&g_pPlaybackManager->m_bPlayingBack;
	unsigned long long ** pManagerStateString = (unsigned long long**)&g_pPlaybackManager->m_szCurrentManagerState[0];
	unsigned long long ** mbLoading = (unsigned long long**)&g_pPlaybackManager->m_bLoading;
	unsigned long long ** gbPaused = (unsigned long long**)&g_bPaused;

	fprintf(pOutFile, "%llx,%llx,%llx,%llx,%llx", pPlaybackManager, isPlayingBack, pManagerStateString, mbLoading, gbPaused);

	fclose(pOutFile);
}