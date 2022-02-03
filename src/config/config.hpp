#pragma once

#include "koalabox/koalabox.hpp"

#include <nlohmann/json.hpp>

namespace config {

    using namespace koalabox;

    struct Module {
        String path;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Module, path)
    };

    struct Config {
        bool logging = false;
        Vector<String> targets;
        Vector<Module> modules;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, logging, targets, modules)
    };

    Config read(Path path);

}
