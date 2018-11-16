#pragma once

#include <Windows.h>
#include <stdio.h>



static NTSTATUS AddHook(void* pInEntryPoint, void * pHookProc, void * pCallback, TRACED_HOOK_HANDLE traceRecord)
{
	return LhInstallHook(pInEntryPoint, pHookProc, pCallback, traceRecord);
}

static NTSTATUS MakeHookExclusive(unsigned long * pThreadListArray, unsigned long dwThreadCount, TRACED_HOOK_HANDLE traceRecord)
{
	return LhSetExclusiveACL(pThreadListArray, dwThreadCount, traceRecord);
}

#define ExclusiveHook(b) MakeHookExclusive(g_ACLEntries, 1, b)
