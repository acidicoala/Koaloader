#include <iostream>
#include <Windows.h>

extern "C" BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
//            init();
            break;
        case DLL_PROCESS_DETACH:
//            shutdown();
            break;
        default:
            break;
    }

    return TRUE;
}
