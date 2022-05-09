#pragma once

#include <koalabox/koalabox.hpp>

#include <nlohmann/json.hpp>

namespace koaloader {
    using namespace koalabox;

    struct Module {
        String path;
        bool required = true;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Module, path, required)
    };

    struct Config {
        bool logging = false;
        bool enabled = true;
        bool auto_load = true;
        Vector<String> targets;
        Vector<Module> modules;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Config, logging, enabled, auto_load, targets, modules)
    };

    extern Config config;

    void init(const HMODULE& self_module);

    void shutdown();
}
