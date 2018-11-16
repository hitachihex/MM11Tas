#pragma once

#include "Addresses.h"
#include <easyhook.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <Xinput.h>
#include <dinput.h>
#include "Utils.h"
#include "DebugUtils.h"

extern unsigned long g_ACLEntries[1];

extern IDirectInputDevice *g_pDirectInputDevice;

/*typedef bool(__cdecl * oEventLoop)(void*, void*, void*, void*, void*, void*);
oEventLoop originalEventLoop = (oEventLoop)(EventLoop_Address);*/
typedef void(__fastcall * oCheckInputState04)(unsigned long, unsigned long);
extern oCheckInputState04 original_CheckInputState04;

//typedef ATOM(__fastcall * oRegisterClassExW)(WNDCLASSEXW*);
//oRegisterClassExW original_RegisterClassExW = (oRegisterClassExW)(*(unsigned long long*)REGISTERCLASSEXW_IAT_ADDRESS);
//ecx, edx, R8, R9
//typedef HRESULT(__fastcall * oDirectInputDevice_GetDeviceState)(IDirectInputDevice*, unsigned long, LPVOID);
//extern oDirectInputDevice_GetDeviceState original_DirectInputDevice_GetDeviceState;

//typedef LRESULT(CALLBACK * oMainWindowProc)(HWND, UINT, WPARAM, LPARAM);
//extern oMainWindowProc original_MainWindowProc = (oMainWindowProc)0x0;

//extern HRESULT __fastcall DirectInputDevice_GetDeviceState_Hook(IDirectInputDevice*, unsigned long, LPVOID);

extern HOOK_TRACE_INFO MM11_CheckInputState04_HookHandle;

extern void __fastcall CheckInputState04_Hook(unsigned long, unsigned long);

typedef unsigned long(WINAPI * oXInputGetState)(unsigned long, XINPUT_STATE*);
extern oXInputGetState original_XInputGetState;

extern unsigned long WINAPI XInputGetState_Hook(unsigned long, XINPUT_STATE*);
extern unsigned long WINAPI XInputGetCapabilities_Hook(unsigned long, unsigned long, XINPUT_CAPABILITIES*);

extern unsigned long long * g_pXInputGetState_IAT;

//extern void HookDirectInputMethods();

//extern ATOM __fastcall RegisterClassExW_Hook(WNDCLASSEXW*);

//extern LRESULT CALLBACK MainWindowProc_Hook(HWND, UINT, WPARAM, LPARAM);

extern void _FixIATRehook();

extern void DumpPointersForExternalOSD();

//extern unsigned long __fastcall TimeGetTime_Hook();

extern float * g_fGlobalGameSpeed;
extern bool g_bPlaybackSync;
extern bool g_bDidFrameStep;

