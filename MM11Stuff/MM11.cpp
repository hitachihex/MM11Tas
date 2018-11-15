#include "stdafx.h"
#include "MM11.h"
#include "Game.h"
#include "PlaybackManager.h"


HOOK_TRACE_INFO MM11_CheckInputState04_HookHandle = { NULL };
unsigned long g_ACLEntries[1];

extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo);

float * g_fGlobalGameSpeed = nullptr;
bool g_bPlaybackSync = false;

oCheckInputState04 original_CheckInputState04 = (oCheckInputState04)(MPGAME_GETINPUTSTATE04_ADDRESS);


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

void _FixIATRehook()
{
	//000000014D0311D1
	unsigned char * pMem = (unsigned char*)(0x000000014D0311D1);

	*(pMem + 0x0) = 0x90;
	*(pMem + 0x1) = 0x90;
	*(pMem + 0x2) = 0x90;
	*(pMem + 0x3) = 0x90;
}

/*
unsigned long __fastcall TimeGetTime_Hook()
{
	static bool bOnce = false;

	if (!bOnce) {
		bOnce = true;
		DebugOutput("timeGetTime hook ok ");
	}
	// ??
	return ULONG_MAX;
}*/

static void ChangeGameSpeed(float f, bool reset = false)
{
	unsigned long long rcx = *(unsigned long long*)(MPENVIRONMENTVARIABLES_ADDRESS);
	if (!rcx) return;

	if (reset) {
		*g_fGlobalGameSpeed = 1.0;
		return;
	}

	g_fGlobalGameSpeed = (float*)(rcx + 0x8C);
	if (*g_fGlobalGameSpeed == 1 && f < 1)
	{
		g_bPaused = true;
		return;
	}

	*g_fGlobalGameSpeed += (f);

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

void __fastcall KeyboardUpdate_Hook(unsigned long long rcx, unsigned long long rdx)
{
	static bool _bOnce = false;

	if (!_bOnce) {
		DebugOutput("KeyboardUpdate hook - OKThumb.");
		_bOnce = true;
	}
	
	struct KeyTable
	{
		unsigned long Keys[0xFF];
	};

	// lea rbx, qword ptr ds:[rcx+0x138]
	KeyTable *pKeyTable = (KeyTable*)(&*(u64*)((u32)rcx + 0x138));

	// mov dword ptr ds:[rbx+0x04], 0x40;
	//pKeyTable->Keys[1] = 0x40;

	// mov dword ptr ds:[rbx+0x24], 0x40;
	//pKeyTable->Keys[9] = 0x40;


	((void(__thiscall*)(unsigned long long))g_dwOriginalKeyboardUpdateAddress)(rcx);
}

void __fastcall GameLoop_Hook(unsigned long long ecx, unsigned long long edx)
{
	static bool _bHookKeyboardOnce = false;

	if (GetAsyncKeyState(VK_F1) & 1 && !g_bPaused)
	{
		// Don't let original update get called on the frame we pressed the bind.
		g_bPaused = true;
		DebugOutput("Paused.");
		return;
	}


	if (GetAsyncKeyState(VK_F4) & 1)
	{
		if (g_pPlaybackManager)
		{
			DebugOutput("Starting playback.");
			if (g_pPlaybackManager->IsPlayingBack())
			{
				g_bPlaybackSync = false;
				g_pPlaybackManager->InitPlayback(true);
			}
			else
			{
				g_pPlaybackManager->InitPlayback(false);
				g_bPlaybackSync = true;
			}
		}
	}

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

	if (!_bHookKeyboardOnce)
	{
		unsigned long long _rcx = *(unsigned long long*)(ecx + 0x40208);
		unsigned long long _rax = *(unsigned long long*)(_rcx);
		// rax  + 0x30
	
		g_dwOriginalKeyboardUpdateAddress = *(unsigned long long*)(_rax + 0x30);
		*(unsigned long long*)(_rax + 0x30) = (unsigned long long)KeyboardUpdate_Hook;

		DebugOutput("Hooked KeyboardUpdate.");
		_bHookKeyboardOnce = true;
	}

	// Only check  unpause or framestep while paused
	if (g_bPaused)
	{
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
			memset(&g_pPlaybackManager->GetXInputState()->Gamepad, 0, sizeof(XINPUT_GAMEPAD));
		}


	}

}

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
		VirtualProtect((LPVOID)g_pVtable[6], 0x8, PAGE_EXECUTE_READWRITE, &dwOldProt);

		g_pVtable[6] = (unsigned long long)GameLoop_Hook;

		DebugOutput("Hooked GameLoop via vtable.");

		*(unsigned long long*)(&original_XInputGetState) = (unsigned long long) GetProcAddress(GetModuleHandle(L"xinput1_3.dll"), "XInputGetState");

		DebugOutput("Original XInputGetState = %llx", (unsigned long long)original_XInputGetState);
	}

	//HookDirectInputMethods();

	return original_CheckInputState04(ecx, edx);
}

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
	//DebugOutput("NativeInjectionEntryPoint");

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

	DWORD dwOldProt;
	VirtualProtect((LPVOID)XINPUT_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);
	VirtualProtect((LPVOID)XINPUT_GETCAPS_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);
	//VirtualProtect((LPVOID)REGISTERCLASSEXW_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);
	//VirtualProtect((LPVOID)TIMEGETTIME_IAT_ADDRESS, 8, PAGE_EXECUTE_READWRITE, &dwOldProt);

	*(unsigned long long*)(XINPUT_IAT_ADDRESS) = (unsigned long long)XInputGetState_Hook;
	*(unsigned long long*)(XINPUT_GETCAPS_IAT_ADDRESS) = (unsigned long long)XInputGetCapabilities_Hook;

	//*(unsigned long long*)(REGISTERCLASSEXW_IAT_ADDRESS) = (unsigned long long)RegisterClassExW_Hook;
	//*(unsigned long long*)(TIMEGETTIME_IAT_ADDRESS) = (unsigned long long)TimeGetTime_Hook;

	_FixIATRehook();

	g_pPlaybackManager = new PlaybackManager("megaman.rec");

}