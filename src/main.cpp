#include <koaloader/koaloader.hpp>

[[maybe_unused]]
__declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        koaloader::init(instance);
    } else if (reason == DLL_PROCESS_DETACH) {
        koaloader::shutdown();
    }

    return TRUE;
}
