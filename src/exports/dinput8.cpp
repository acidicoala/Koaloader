#include "koaloader/koaloader.hpp"

using namespace koaloader;

#ifdef PROXY_DLL_DINPUT8

struct DIDATAFORMAT;

FUNC_IMPL(HRESULT, DirectInput8Create, ARGS(HINSTANCE, DWORD, REFIID, LPVOID * , LPUNKNOWN))
FUNC_IMPL(HRESULT, DllCanUnloadNow, NO_ARGS)
FUNC_IMPL(HRESULT, DllGetClassObject, ARGS(REFCLSID, REFIID, LPVOID))
FUNC_IMPL(HRESULT, DllRegisterServer, NO_ARGS)
FUNC_IMPL(HRESULT, DllUnregisterServer, NO_ARGS)
FUNC_IMPL(DIDATAFORMAT*, GetdfDIJoystick, NO_ARGS)

#endif
