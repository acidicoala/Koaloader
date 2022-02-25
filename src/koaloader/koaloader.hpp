#pragma once

#include <build_config.h>

namespace koaloader {
    using namespace koalabox;

    struct Module {
        String path;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Module, path)
    };

    struct Config {
        bool logging = false;
        bool enabled = true;
        Vector<String> targets;
        Vector<Module> modules;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, logging, enabled, targets, modules)
    };

    extern Config config;

    void init(const HMODULE& self_module);

    void shutdown();
}
