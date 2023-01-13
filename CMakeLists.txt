cmake_minimum_required(VERSION 3.24)

project(Koaloader VERSION 3.0.0)

include(KoalaBox/cmake/KoalaBox.cmake)

add_subdirectory(KoalaBox EXCLUDE_FROM_ALL)

# Get module variable
SET(MODULE "" CACHE STRING "Some user-specified option")
if (NOT MODULE)
    message(FATAL_ERROR "Option 'MODULE' must be set")
endif ()

configure_version_resource("https://github.com/acidicoala/Koaloader")

# Setup linker exports

set(
    KOALOADER_SOURCES
    src/koaloader/koaloader.cpp
    src/koaloader/koaloader.hpp
    src/main.cpp
)

configure_linker_exports(
    FORWARDED_DLL "C:/Windows/System32/${MODULE}"
    INPUT_SOURCES_DIR ""
    INPUT_DLLS "C:/Windows/System32/${MODULE}.dll"
    DEP_SOURCES "${KOALOADER_SOURCES}"
)

configure_build_config()

add_library(Koaloader SHARED ${KOALOADER_SOURCES} ${GENERATED_LINKER_EXPORTS} ${VERSION_RESOURCE})

configure_output_name(${MODULE})

configure_include_directories()

target_link_libraries(Koaloader PRIVATE KoalaBox)
