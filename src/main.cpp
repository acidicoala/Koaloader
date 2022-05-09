#include <koaloader/koaloader.hpp>
#include <linker_exports.h>

#define DLL_EXPORT(TYPE) extern "C" __declspec(dllexport) TYPE WINAPI

DLL_EXPORT(BOOL) DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        koaloader::init(instance);
    } else if (reason == DLL_PROCESS_DETACH) {
        koaloader::shutdown();
    }

    return TRUE;
}
