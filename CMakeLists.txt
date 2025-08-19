cmake_minimum_required(VERSION 3.24)

project(Koaloader VERSION 3.0.3)

include(KoalaBox/cmake/KoalaBox.cmake)

add_subdirectory(KoalaBox EXCLUDE_FROM_ALL)

# Get module variable
SET(MODULE "" CACHE STRING "Some user-specified option")
if (NOT MODULE)
    message(FATAL_ERROR "Option 'MODULE' must be set")
endif ()

configure_version_resource("https://github.com/acidicoala/Koaloader")

configure_build_config()

set(
    KOALOADER_SOURCES
    src/koaloader/koaloader.cpp
    src/koaloader/koaloader.hpp
    src/main.cpp
)

add_library(Koaloader SHARED ${KOALOADER_SOURCES} ${GENERATED_LINKER_EXPORTS} ${VERSION_RESOURCE})

configure_linker_exports(
    TARGET Koaloader
    FORWARDED_DLL "C:/Windows/System32/${MODULE}"
    INPUT_SOURCES_DIR ""
    DLL_FILES_GLOB "C:/Windows/System32/${MODULE}.dll"
)

configure_output_name(${MODULE})

configure_include_directories()
target_precompile_headers(Koaloader REUSE_FROM KoalaBox)

target_link_libraries(Koaloader PRIVATE KoalaBox)
