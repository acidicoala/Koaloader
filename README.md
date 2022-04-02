# 🐨 Koaloader 📥

A collection of DLLs that use search order hijacking to automatically inject specified DLLs.

[Official forum topic](https://cs.rin.ru/forum/viewtopic.php?p=2536753#p2536753)

## 🚀 Usage

Simply place one of the proxy dlls where a process is attempting to load it and setup the config file to load the DLLs that you wish to inject.

### 📚 Supported proxies

<details><summary>Click to see all proxies</summary>

- [x] **audioses.dll**
- [x] **d3d9.dll**
- [x] **d3d10.dll**
- [x] **d3d11.dll**
- [x] **dinput8.dll**
- [x] **dwmapi.dll**
- [x] **dxgi.dll**
- [x] **glu32.dll**
- [x] **hid.dll**
- [x] **iphlpapi.dll**
- [x] **msasn1.dll**
- [x] **mswsock.dll**
- [x] **opengl32.dll**
- [x] **profapi.dll**
- [x] **propsys.dll**
- [x] **textshaping.dll**
- [x] **version.dll**
- [x] **winhttp.dll**
- [x] **wldp.dll**
- [x] **winmm.dll**
- [x] **xinput9_1_0.dll**

</details>

> ➕ If you wish to see another proxy DLL supported, feel free to create a new issue, specifying the DLL name and describing its use case.

## ⚙ Configuration

Koaloader comes with a configuration file `Koaloader.json`, which will be the same for any proxy dll. The config file conforms to the standard JSON format. The description of each available option is presented below:

* `logging`: Enables or disables logging into a `Koaloader.log` file. Possible values: `true`, `false` (default).
* `enabled`: Entirely enables or disables Koaloader injection. Can be used to quickly disable Koaloader without modifying files on disk. Possible values: `true` (default), `false`.
* `auto_load`: Enables or disables automatic loading of well-known DLLs. This can be used to automatically inject DLLs without `Koaloader.json` config file. When enabled, Koaloader will first try to find a well-known DLL in parent directories. If it failed to do so, it will recursively go through all files in current working directory (which may be different from executable or Koaloader directory) and search for files with well-known file names. Default: `true`. A list of well-known file names (Names ending in 32 and 64 are loaded only by 32-bit and 64-bit binaries respectively):
  * `Unlocker.dll`, `Unlocker32.dll`, `Unlocker64.dll`
  * `Lyptus.dll`, `Lyptus32.dll`, `Lyptus64.dll`
  * `ScreamAPI.dll`,`ScreamAPI32.dll`,`ScreamAPI64.dll`
  * `UplayR1Unlocker.dll`, `UplayR1Unlocker32.dll`, `UplayR1Unlocker64.dll`
  * `UplayR2Unlocker.dll`, `UplayR2Unlocker32.dll`, `UplayR2Unlocker64.dll`
* `targets`: A list of strings that specify targeted executables. This can be used to prevent unintended loading by irrelevant executables. Koaloader will inject modules if, and only if:
  * The list of targets is empty, **or**
  * The list of targets includes the executable that has loaded Koaloader.
* `modules`: A list of objects that describe modules that will be loaded in the order they were defined. Each object has the following properties:
  * `path`:  A string that specifies absolute or relative path to a DLL. The relative path is with respect to the working directory, which may be different from the executable directory.
  * `required`: A boolean that marks the module as required for loading. This can be used to specify behaviour when Koaloader fails to load a given module. Failure to load required modules will result in a crash with message box, whereas in not required modules Koaloader will simply print the error in the log file. Default: `true`.

You can refer to the following config as an example.

> Here we have defined 2 DLLs to load:
>
> * `target.dll` - via a path that is relative to the current working directory of the executable
> * `eucalyptus.dll` - via an absolute path.
>
> ```json
> {
>   "logging": true,
>   "enabled": true,
>   "auto_load": false,
>   "targets": [ "program32.exe", "program64.exe" ],
>   "modules": [
>     {
>       "path": "target.dll",
>       "required": true
>     },
>     {
>       "path": "C:/users/acidicoala/eucalyptus.dll",
>       "required": false
>     }
>   ]
> }
> ```

## 🛠 Development

### 🚥 Prerequisites

* Git v2.13 or newer
* CMake 3.21 or newer
* Visual Studio 2022 Build Tools/IDE with "**Desktop Development with C++**" installed.

___
Clone the project with its submodules:

```powershell
git clone --recurse-submodules https://github.com/acidicoala/Koaloader.git
```

Run the build script with desired parameters:

```shell
./build.ps1 $Arch $Config $Proxy
```

* `$Arch` - Program architecture. Valid values:
  * `32`
  * `64`
* `$Config` - Build configuration. Valid values:
  * `Debug`
  * `Release`
  * `RelWithDebInfo`
* `$Proxy` - Proxy DLL to build. Any DLL from `C:/Windows/System32 `is valid.

Example:

```shell
./build.ps1 64 Release d3d11
```

The final DLL will be located at
`build\$Arch\$Proxy\$Config`

### Potential improvements

- [ ] DLLs with unnamed exports (by ordinal)
- [ ] DLLs that are missing from System32 directory

### Miscellaneous notes

- Version is defined in [CMakeLists.txt](./CMakeLists.txt)
- CMake project likely needs to be reloaded after changing files in the [res](./res) directory.
- GitHub actions will build the project on every push to `master`, but will prepare a draft release only if the last commit was tagged.
- Proxy dll need to be defined in [ci.yml](.github/workflows/ci.yml)

## 👋 Acknowledgements

This project makes use of the following open source projects:

- [JSON for Modern C++](https://github.com/nlohmann/json)
- [spdlog](https://github.com/gabime/spdlog)

## 📄 License

This software is licensed under [BSD Zero Clause  License], terms of which are available in [LICENSE.txt]

[BSD Zero Clause  License]: https://choosealicense.com/licenses/0bsd/

[LICENSE.txt]: LICENSE.txt
