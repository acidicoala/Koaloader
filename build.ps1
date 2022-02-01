# Usage examples:
#   build.ps1 32 Debug version
#   build.ps1 64 Release xinput9_1_0

$arch = $args[0]
if ($arch -notmatch '^(32|64)$') {
    throw "Invalid architecute. Expected: '32' or '64'. Got: '$arch'"
}
$platform = If ($arch -eq '32') { 'Win32' } Else { 'x64' }

$proxy = $args[1]
if ($proxy -notmatch '^(version|xinput9_1_0)$') {
    throw "Invalid proxy dll. Consult README for list of valid modules."
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

cmake -G "Visual Studio 16 2019" -A $platform -B "$Env:BUILD_DIR"

cmake --build "$Env:BUILD_DIR" --config $config
