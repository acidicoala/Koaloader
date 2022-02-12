#pragma once

#include <Windows.h>

#include <build_config.h>

namespace koaloader {

    void init(HMODULE self_module);

    void shutdown();
}
