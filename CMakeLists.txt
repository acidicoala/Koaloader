cmake_minimum_required(VERSION 3.21)

include(KoalaBox/KoalaBox.cmake)

configure_globals(KoalaBox)

project(Koaloader VERSION "2.0.0")

set(PROXY_DLL $ENV{PROXY_DLL})

configure_build_config(extra_build_config)

configure_version_resource("https://github.com/acidicoala/Koaloader")

configure_exports_generator()
configure_linker_exports(
    "C:/Windows/System32/${PROXY_DLL}"
    "C:/Windows/System32/${PROXY_DLL}.dll"
    ${SRC_DIR}
)

configure_library(
    SHARED
    ${SRC_DIR}/config/config.cpp
    ${SRC_DIR}/koaloader/koaloader.cpp
    ${SRC_DIR}/main.cpp
)

configure_output_name(${PROXY_DLL})

configure_include_directories()

configure_dependencies(spdlog nlohmann_json)
