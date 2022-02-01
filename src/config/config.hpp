#pragma once

#include "koalabox.hpp"

#include <nlohmann/json.hpp>

namespace config {

    using namespace koalabox;

    struct Module {
        String path;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Module, path)
    };

    struct Config {
        bool logging = false;
        Vector<Module> modules;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, logging, modules)
    };

    Config read(Path path);

}
