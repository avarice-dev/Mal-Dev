#include <windows.h>

INT __declspec(dllexport) PopMessageBox(VOID) {

    MessageBoxW(NULL, L'Test Message', L'Test Title', MB_OK | MB_ICONINFORMATION);

}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        PopMessageBox();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    
    return TRUE;
}

