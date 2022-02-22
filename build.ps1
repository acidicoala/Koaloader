# Usage examples:
#   build.ps1 32 version Debug
#   build.ps1 64 xinput9_1_0 Release
#   build.ps1 64 dinput8 Release

$arch = $args[0]
if ($arch -notmatch '^(32|64)$') {
    throw "Invalid architecute. Expected: '32' or '64'. Got: '$arch'"
}
$platform = If ($arch -eq '32') { 'Win32' } Else { 'x64' }

$proxy = $args[1]
if ($proxy -eq '') {
    throw "Proxy DLL cannot be empty."
}

$config = $args[2]
if ($config -notmatch '^(Debug|Release|RelWithDebInfo)$') {
    throw "Invalid architecute. Expected: 'Debug' or 'Release' or 'RelWithDebInfo'. Got: '$config'"
}

Set-Location (Get-Item $PSScriptRoot)

$Env:BUILD_DIR = "build\$arch\$proxy"
if ($config -eq 'Debug') {
    $Env:VERSION_SUFFIX = "-debug"
}

$Env:PROXY_DLL = $proxy

cmake -G "Visual Studio 17 2022" -A $platform -B "$Env:BUILD_DIR"

cmake --build "$Env:BUILD_DIR" --config $config
