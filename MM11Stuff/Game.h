#pragma once

#pragma pack(push, 1)

unsigned long long * g_pVtable;

unsigned long long g_dwOriginalGameLoopAddress;

unsigned long long g_dwOriginalKeyboardUpdateAddress;

#pragma pack(pop)