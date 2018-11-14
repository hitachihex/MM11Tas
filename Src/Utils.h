#pragma once

#include <Windows.h>
#include <stdio.h>

static void DebugOutput(const char * fmt, ...)
{
	char outputBuffer[2048] = { 0 };
	va_list vaList;

	va_start(vaList, fmt);
	vsnprintf(outputBuffer, 2048, fmt, vaList);
	va_end(vaList);

	OutputDebugStringA(outputBuffer);
}

static void DebugOutputW(const wchar_t* fmt, ...)
{
	wchar_t outputBuffer[2048] = { 0 };

	va_list args;
	va_start(args, fmt);
	vswprintf_s(outputBuffer, 2048, fmt, args);
	va_end(args);

	OutputDebugStringW(outputBuffer);
}

static NTSTATUS AddHook(void* pInEntryPoint, void * pHookProc, void * pCallback, TRACED_HOOK_HANDLE traceRecord)
{
	return LhInstallHook(pInEntryPoint, pHookProc, pCallback, traceRecord);
}

static NTSTATUS MakeHookExclusive(unsigned long * pThreadListArray, unsigned long dwThreadCount, TRACED_HOOK_HANDLE traceRecord)
{
	return LhSetExclusiveACL(pThreadListArray, dwThreadCount, traceRecord);
}

#define ExclusiveHook(b) MakeHookExclusive(g_ACLEntries, 1, b)
