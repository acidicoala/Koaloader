#include "koaloader/koaloader.hpp"

using namespace koaloader;

// Documentation: https://docs.microsoft.com/en-us/windows/win32/api/_xinput/

#ifdef PROXY_DLL_XINPUT9_1_0

struct XINPUT_CAPABILITIES;
struct XINPUT_STATE;
struct XINPUT_VIBRATION;

FUNC_IMPL(DWORD, XInputGetCapabilities, ARGS(DWORD, DWORD, XINPUT_CAPABILITIES * ))
FUNC_IMPL(DWORD, XInputGetDSoundAudioDeviceGuids, ARGS(DWORD, GUID * , GUID * ))
FUNC_IMPL(DWORD, XInputGetState, ARGS(DWORD, XINPUT_STATE * ))
FUNC_IMPL(DWORD, XInputSetState, ARGS(DWORD, XINPUT_VIBRATION * ))

#endif
