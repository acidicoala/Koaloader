#pragma once

#include <string>

#include <nlohmann/json.hpp>

namespace koaloader {

    struct Module {
        std::string path;
        bool required = true;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Module, path, required)
    };

    struct Config {
        bool logging = false;
        bool enabled = true;
        bool auto_load = true;
        std::vector<std::string> targets;
        std::vector<Module> modules;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
            Config, logging, enabled, auto_load, targets, modules
        )
    };

    extern Config config;

    void init(HMODULE self_module);

    void shutdown();

}
