cmake_minimum_required(VERSION 3.21)

if (NOT DEFINED PROXY_DLL)
    set(PROXY_DLL $ENV{PROXY_DLL})
endif ()

if ("${PROXY_DLL}" STREQUAL "")
    message(FATAL_ERROR "PROXY_DLL must be provided either as a parameter or environment variable")
endif ()

include(KoalaBox/KoalaBox.cmake)

configure_globals(KoalaBox)

project(Koaloader VERSION 2.0.0)

configure_version_resource("https://github.com/acidicoala/Koaloader")

configure_exports_generator()
configure_linker_exports(
    "C:/Windows/System32/${PROXY_DLL}"
    "C:/Windows/System32/${PROXY_DLL}.dll"
    ${SRC_DIR}
)

configure_build_config()

configure_library(
    SHARED

    ${SRC_DIR}/koaloader/koaloader.cpp
    ${SRC_DIR}/main.cpp

    ${KOALABOX_SRC_DIR}/koalabox/loader/loader.cpp
    ${KOALABOX_SRC_DIR}/koalabox/file_logger/file_logger.cpp
    ${KOALABOX_SRC_DIR}/koalabox/util/util.cpp
    ${KOALABOX_SRC_DIR}/koalabox/win_util/win_util.cpp

    ${LINKER_EXPORTS}
)

configure_precompile_headers(${CMAKE_PROJECT_NAME} ${SRC_DIR}/pch.hpp)

configure_output_name(${PROXY_DLL})

configure_include_directories()

configure_dependencies(${CMAKE_PROJECT_NAME} spdlog nlohmann_json)
