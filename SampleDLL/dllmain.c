// dllmain.c : Defines the entry point for the DLL application.
#include <Windows.h>

// Our Export
extern __declspec(dllexport) void HelloWorld(void){
    MessageBoxA(NULL, "Hello, World!", "DLL Message",MB_ICONASTERISK);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}