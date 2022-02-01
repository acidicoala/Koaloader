#include "koaloader.hpp"
#include "config/config.hpp"
#include "logger/logger.hpp"
#include "win_util/win_util.hpp"

using namespace koalabox;

HMODULE koaloader::original_module = nullptr;

Vector<HMODULE> injected_modules;

Path get_system_module_path(const String& name) {
    TCHAR path[MAX_PATH];
    GetSystemDirectory(path, MAX_PATH);
    return std::filesystem::absolute(path) / name;
}

void koaloader::init(HMODULE self_module) {
    DisableThreadLibraryCalls(self_module);

    auto config = config::read("Koaloader.json");

    if (config.logging) {
        logger::init("Koaloader.log");
    }

    logger::info("🐨 Koaloader 📥 v{}", PROJECT_VERSION);

    auto original_module_path = get_system_module_path(DLL_NAME".dll");
    original_module = win_util::load_library(original_module_path);

    for (const auto& module: config.modules) {
        auto path = std::filesystem::absolute(module.path);

        auto handle = win_util::load_library(path);

        injected_modules.push_back(handle);

        logger::info("💉 Injected module: '{}'", path.string());
    }

    logger::info("🚀 Initialization complete");
}

void koaloader::shutdown() {
    for (const auto& module: injected_modules) {
        logger::debug("Freeing module with handle: {}", fmt::ptr(module));

        win_util::free_library(module);
    }

    injected_modules.clear();

    win_util::free_library(original_module);

    logger::info("💀 Shutdown complete");
}
