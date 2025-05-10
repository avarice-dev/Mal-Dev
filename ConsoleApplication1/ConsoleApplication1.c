// ConsoleApplication1.c : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>

// Function pointer type def... allows the compiler to know how to use a function pointer
typedef void (*fnHelloWorld)();


int main()
{
    // Load our Custom DLL
    // Test if present before loading
    HMODULE hModule = GetModuleHandleA("sampleDLL.dll");

    if (hModule == NULL) {
        hModule = LoadLibraryA("sampleDLL.dll");
    }

    // Get pointer to our desired function
    PVOID pHelloWorld = GetProcAddress(hModule, "HelloWorld");

    // Declare function pointer, recast our FARPROC pointer as that type
    fnHelloWorld HelloWorld = (fnHelloWorld)pHelloWorld;

    // Now we can actually use it as though it were a locally defined function
    HelloWorld();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
