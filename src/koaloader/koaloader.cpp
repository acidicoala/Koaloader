#include "koaloader.hpp"
#include "config/config.hpp"
#include "koalabox/logger/logger.hpp"
#include "koalabox/win_util/win_util.hpp"
#include "koalabox/util/util.hpp"

using namespace koalabox;

HMODULE koaloader::original_module = nullptr;

Vector<HMODULE> injected_modules;

bool is_loaded_by_target(const config::Config& config) {
    if (config.targets.empty()) {
        return true;
    } else {
        const auto process_handle = win_util::get_current_process_handle();
        const auto executable_path = Path(win_util::get_module_file_name(process_handle));
        const auto executable_name = executable_path.filename().string();

        bool target_found = false;
        for (const auto& target: config.targets) {
            if (util::strings_are_equal(target, executable_name)) {
                logger::debug("Target found: '{}'", target);
                target_found = true;
                break;
            }
        }

        return target_found;
    }
}

void inject_modules(const config::Config& config) {
    for (const auto& module: config.modules) {
        const auto path = std::filesystem::absolute(module.path);

        const auto handle = win_util::load_library(path);

        injected_modules.push_back(handle);

        logger::info("💉 Injected module: '{}'", path.string());
    }
}

void koaloader::init(HMODULE self_module) {
    DisableThreadLibraryCalls(self_module);

    koalabox::project_name = "Koaloader";

    const auto self_directory = util::get_module_dir(self_module);

    const auto config = config::read(self_directory / "Koaloader.json");

    if (config.logging) {
        logger::init(self_directory / "Koaloader.log");
    }

    logger::info("🐨 Koaloader 📥 v{}", PROJECT_VERSION);

    util::panic("Test title", "Test message {}", 123);

    const auto original_module_path = win_util::get_system_directory() / DLL_NAME".dll";
    koaloader::original_module = win_util::load_library(original_module_path);
    logger::info("📚 Loaded original library from: '{}'", original_module_path.string());

    if (is_loaded_by_target(config)) {
        inject_modules(config);
    } else {
        logger::debug("Not loaded by target process. Skipping injections.");
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
