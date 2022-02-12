#pragma once

#include <Windows.h>

#include <build_config.h>

namespace koaloader {

    extern HMODULE original_module;

    void init(HMODULE self_module);

    void shutdown();
}
