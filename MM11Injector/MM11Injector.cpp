// MM11Injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <cstring>
#include <TlHelp32.h>
#include <easyhook.h>


#pragma comment(lib, "psapi.lib")

unsigned long MM11_ProcessID = (-1);

DWORD GetGameProcessID()
{
	HANDLE hProcSnap;
	PROCESSENTRY32 pe32;

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcSnap, &pe32))
	{
		std::cout << " Process32First failed " << std::endl;
		CloseHandle(hProcSnap);
		return -1;
	}

	do
	{
		std::wcout << "Found process: " << pe32.szExeFile << std::endl;

		if (lstrcmpiW(L"game.exe", pe32.szExeFile) == 0)
		{
			// procid GET
			CloseHandle(hProcSnap);
			return  pe32.th32ProcessID;
		}
	} while (Process32Next(hProcSnap, &pe32));

	CloseHandle(hProcSnap);
	return (-1);
}

int main(int argc, char ** argv)
{
	MM11_ProcessID = GetGameProcessID();

	if (MM11_ProcessID == -1)
		return 1;


	const WCHAR * DynLib = L"MM11Stuff.dll";


	NTSTATUS ntResult = RhInjectLibrary(MM11_ProcessID,
		0, EASYHOOK_INJECT_DEFAULT, nullptr, (WCHAR*)DynLib,  NULL, 0);

	if (ntResult != 0)
	{
		printf("Failed to inject with error code %d\n", ntResult);
		PWCHAR err = RtlGetLastErrorString();
		std::wcout << err << "\n";
		return -1;
	}

	else
	{
		std::cout << "Injection succesfull.\n";
	}


	return 0;
}

