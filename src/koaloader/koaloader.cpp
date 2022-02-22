#include "koaloader.hpp"

namespace koaloader {

    Config config = {};

    Vector<HMODULE> injected_modules;

    bool is_loaded_by_target() {
        if (config.targets.empty()) {
            return true;
        }

        const auto process_handle = win_util::get_module_handle(nullptr);
        const auto executable_path = Path(win_util::get_module_file_name(process_handle));
        const auto executable_name = executable_path.filename().string();

        bool target_found = false;
        for (const auto& target: config.targets) {
            if (util::strings_are_equal(target, executable_name)) {
                logger->debug("Target found: '{}'", target);
                target_found = true;
                break;
            }
        }

        return target_found;
    }

    void inject_modules() {
        for (const auto& module: config.modules) {
            const auto path = std::filesystem::absolute(module.path);

            const auto handle = win_util::load_library(path);

            injected_modules.push_back(handle);

            logger->info("💉 Injected module: '{}'", path.string());
        }
    }

    void init(const HMODULE& self_module) {
        DisableThreadLibraryCalls(self_module);

        const auto self_directory = loader::get_module_dir(self_module);

        config = config_parser::parse<Config>(self_directory / PROJECT_NAME".json");

        if (config.logging) {
            logger = file_logger::create(self_directory / PROJECT_NAME".log");
        }

        logger->info("🐨 {} 📥 v{}", PROJECT_NAME, PROJECT_VERSION);

        if (is_loaded_by_target()) {
            inject_modules();
        } else {
            logger->debug("Not loaded by target process. Skipping injections.");
        }

        logger->info("🚀 Initialization complete");
    }

    void shutdown() {
        for (const auto& module: injected_modules) {
            logger->debug("Freeing module with handle: {}", fmt::ptr(module));

            win_util::free_library(module);
        }

        injected_modules.clear();

        logger->info("💀 Shutdown complete");
    }

}
