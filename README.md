# 🐨 Koaloader 📥

A collections of DLLs that use search order hijacking to automatically inject specified DLLs.

## 🚀 Usage

Simply place one of the proxy dlls where a process is attempting to load it and setup the config file to load the DLLs that you wish to inject.

### 📚 Supported proxies

- **dinput8.dll**
- **version.dll**
- **xinput9_1_0.dll**

## ⚙ Configuration

Koaloader comes with a configuration file `Koaloader.json`, which will be the same for any proxy dll. The config file
conforms to the standard JSON format. The description of each available option is presented below:

* `logging`: Enables or disables logging into a `Koaloader.log` file. Possible values: `true`, `false` (default).
* `modules`: An array of objects that describe modules that will be loaded in the order they were defined. Each object
  has the following properties:
    * `path`:  A string that specifies absolute or relative path

You can refer to the following config as an example.

> Here we have defined 2 DLLs to load:
>
> * `target.dll` - via a path that is relative to the current working directory of the executable
> * `eucalyptus.dll` - via an absolute path.
>
> ```json
> {
>   "logging": true,
>   "modules": [
>     {
>       "path": "target.dll"
>     },
>     {
>       "path": "C:/users/acidicoala/eucalyptus.dll"
>     }
>   ]
> }
> ```

## 🛠 Development

### 🚥 Prerequisites

* Git v2.13 or newer
* CMake 3.21 or newer
* VS 2019 Build Tools/IDE with Desktop Development with C++ installed.

___
Clone the project with its submodules:

```powershell
git clone --recurse-submodules https://github.com/acidicoala/Koaloader.git
```

Run the build script with desired parameters:

```shell
./build.ps1 $Arch $Proxy $Config
```

* `$Arch` - Program architecture. Valid values:
    * `32`
    * `64`
* `$Proxy` - Proxy DLL to build. Valid values:
    * `dinput8`
    * `version`
    * `xinput9_1_0`
* `$Config` - Build configuration. Valid values:
    * `Debug`
    * `Release`
    * `RelWithDebInfo`

Example:

```shell
./build.ps1 64 xinput9_1_0 Release
```

The final DLL will be located at
`build\$Arch\$Proxy\$Config`

### 🔡 Commands

Update all submodules:

```shell
git submodule foreach git pull
```

### Miscellaneous notes

- Version is defined in [version.txt](./res/version.txt)
- CMake project likely needs to be reloaded after changing files in the [res](./res) directory.
- GitHub actions will build the project on every push to `master`, but will prepare a draft release only if the last
  commit was tagged.

## 👋 Acknowledgements

KoalaBox makes use of the following open source projects:

- [spdlog](https://github.com/gabime/spdlog)
- [JSON for Modern C++](https://github.com/nlohmann/json)

## 📄 License

This software is licensed under [BSD Zero Clause  License], terms of which are available in [LICENSE.txt]

[BSD Zero Clause  License]: https://choosealicense.com/licenses/0bsd/

[LICENSE.txt]: LICENSE.txt
