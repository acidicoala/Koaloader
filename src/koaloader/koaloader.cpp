#include "koaloader.hpp"
#include "config/config.hpp"
#include "logger/logger.hpp"
#include "win_util/win_util.hpp"
#include "util/util.hpp"

using namespace koalabox;

HMODULE koaloader::original_module = nullptr;

Vector<HMODULE> injected_modules;

void koaloader::init(HMODULE self_module) {
    DisableThreadLibraryCalls(self_module);

    const auto self_directory = util::get_module_dir(self_module);

    const auto config = config::read(self_directory / "Koaloader.json");

    if (config.logging) {
        logger::init(self_directory / "Koaloader.log");
    }

    logger::info("🐨 Koaloader 📥 v{}", PROJECT_VERSION);

    const auto original_module_path = win_util::get_system_directory() / DLL_NAME".dll";
    original_module = win_util::load_library(original_module_path);

    for (const auto& module: config.modules) {
        const auto path = std::filesystem::absolute(module.path);

        const auto handle = win_util::load_library(path);

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
