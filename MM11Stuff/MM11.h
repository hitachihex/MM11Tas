#pragma once

#include "Addresses.h"
#include <easyhook.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <Xinput.h>
#include <dinput.h>
#include "MTFramework.h"
#include "Utils.h"
#include "DebugUtils.h"
#include <random>


#define RADICAL_ED 1

#define ComInitOriginalDef(num) typedef void(__fastcall * oComInitOriginal##num(unsigned long long, unsigned long long) \
                                extern oComInitOriginal##num original_ComInit##num;

extern unsigned long g_ACLEntries[1];

extern IDirectInputDevice *g_pDirectInputDevice;

/*typedef bool(__cdecl * oEventLoop)(void*, void*, void*, void*, void*, void*);
oEventLoop originalEventLoop = (oEventLoop)(EventLoop_Address);*/
typedef void(__fastcall * oCheckInputState04)(unsigned long, unsigned long);
extern oCheckInputState04 original_CheckInputState04;

typedef unsigned long(__fastcall* oHandleGameSpeed)(unsigned long long, unsigned long long, unsigned long long, unsigned long long);
extern oHandleGameSpeed original_HandleGameSpeed;
typedef void(__fastcall * oHandleGameSpeed2)(unsigned long long);
extern oHandleGameSpeed2 original_HandleGameSpeed2;

typedef void(__fastcall *oComInitOriginal00)(unsigned long long, unsigned long long);
extern oComInitOriginal00 original_ComInit00;


typedef bool(__fastcall *oQueryPerformanceCounter)(LARGE_INTEGER*);
extern oQueryPerformanceCounter original_QueryPerformanceCounter;

typedef unsigned long(__fastcall* oTimeGetTime)();
extern oTimeGetTime original_TimeGetTime;

typedef unsigned long(_fastcall * oGetTickCount)();
extern oGetTickCount original_GetTickCount;

//typedef ATOM(__fastcall * oRegisterClassExW)(WNDCLASSEXW*);
//oRegisterClassExW original_RegisterClassExW = (oRegisterClassExW)(*(unsigned long long*)REGISTERCLASSEXW_IAT_ADDRESS);
//ecx, edx, R8, R9
//typedef HRESULT(__fastcall * oDirectInputDevice_GetDeviceState)(IDirectInputDevice*, unsigned long, LPVOID);
//extern oDirectInputDevice_GetDeviceState original_DirectInputDevice_GetDeviceState;

//typedef LRESULT(CALLBACK * oMainWindowProc)(HWND, UINT, WPARAM, LPARAM);
//extern oMainWindowProc original_MainWindowProc = (oMainWindowProc)0x0;

//extern HRESULT __fastcall DirectInputDevice_GetDeviceState_Hook(IDirectInputDevice*, unsigned long, LPVOID);

extern HOOK_TRACE_INFO MM11_CheckInputState04_HookHandle;
extern HOOK_TRACE_INFO MM11_HandleGameSpeed_HookHandle;
extern HOOK_TRACE_INFO MM11_HandleGameSpeed2_HookHandle;

extern HOOK_TRACE_INFO MM11_MTObjectComInit00_HookHandle;
//extern HOOK_TRACE_INFO MM11_MTObjectComInit01_HookHandle;
//extern HOOK_TRACE_INFO MM11_MTObjectComInit02_HookHandle;
//extern HOOK_TRACE_INFO MM11_MTObjectComInit03_HookHandle;

extern unsigned long __fastcall HandleGameSpeed_Hook(unsigned long long, unsigned long long, unsigned long long, unsigned long long);
extern void __fastcall HandleGameSpeed2_Hook(unsigned long long);
extern void __fastcall CheckInputState04_Hook(unsigned long, unsigned long);

extern void __fastcall MTObjectComInit_Hook00(unsigned long long, unsigned long long);

typedef unsigned long(WINAPI * oXInputGetState)(unsigned long, XINPUT_STATE*);
extern oXInputGetState original_XInputGetState;

extern unsigned long WINAPI XInputGetState_Hook(unsigned long, XINPUT_STATE*);
extern unsigned long WINAPI XInputGetCapabilities_Hook(unsigned long, unsigned long, XINPUT_CAPABILITIES*);

extern unsigned long long * g_pXInputGetState_IAT;

//extern void HookDirectInputMethods();

//extern ATOM __fastcall RegisterClassExW_Hook(WNDCLASSEXW*);

//extern LRESULT CALLBACK MainWindowProc_Hook(HWND, UINT, WPARAM, LPARAM);

extern void _FixIATRehook();
extern void _FuckYourLimiter();

extern void DumpPointersForExternalOSD();

extern bool IsLoading(unsigned long long);

extern void InitFastForward();

//extern void ForceGameOver(unsigned long long);

extern unsigned long __fastcall TimeGetTime_Hook();
extern bool __fastcall QueryPerformanceCounter_Hook(LARGE_INTEGER*);
extern unsigned long __fastcall GetTickCount_Hook();
extern unsigned long g_dwBaseTime;
extern unsigned long g_dwTickCount;
extern int64_t g_dwPerformanceCount;
extern unsigned long g_dwGameSpeed;

extern float * g_fGlobalGameSpeed;
extern bool g_bPlaybackSync;
extern bool g_bDidFrameStep;
extern unsigned long long g_llGameLoopRcx;

#ifdef RANDTEST
extern std::random_device g_RandTest;
#endif

