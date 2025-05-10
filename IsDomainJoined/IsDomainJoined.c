#include <windows.h>
#include <lm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib, "netapi32.lib")

int main()
{
    // Initialize
    LPWSTR domainNameBuffer = NULL;
    NETSETUP_JOIN_STATUS domainStatus;
    NET_API_STATUS nStatus;
    
    // Check
    nStatus = NetGetJoinInformation(NULL, &domainNameBuffer, &domainStatus);

    // Output
    wchar_t outMessage[256];

    if (nStatus == NERR_Success) {
        switch (domainStatus) {
            case NetSetupUnknownStatus:
                wcscpy_s(outMessage, 256, L"Domain Status Unknown");
                break;
            case NetSetupUnjoined:
                wcscpy_s(outMessage, 256, L"Computer not joined to a domain");
                break;
            case NetSetupWorkgroupName:
                _snwprintf_s(outMessage, 256, _TRUNCATE, L"Computer joined to workgroup: %s", domainNameBuffer);
                break;
            case NetSetupDomainName:
                _snwprintf_s(outMessage, 256, _TRUNCATE, L"Computer joined to domain: %s", domainNameBuffer);
                break;
        }

        MessageBox(NULL, outMessage, L"Info", MB_ICONINFORMATION);
    }
    else {
        //_snwprintf_s(outMessage, 256, _TRUNCATE,);
        wcscpy_s(outMessage, 256, L"Domain Check Failed");
        MessageBox(NULL, outMessage, L"Error", MB_ICONERROR);
    }
    
    NetApiBufferFree(domainNameBuffer);
    return 0;
}