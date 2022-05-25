#include <koaloader/koaloader.hpp>

#include <koalabox/win_util.hpp>
#include <koalabox/util.hpp>
#include <koalabox/loader.hpp>
#include <koalabox/config_parser.hpp>
#include <koalabox/file_logger.hpp>

#include <build_config.h>

namespace koaloader {

    Config config = {};

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

    Set<String> generate_well_known_modules() {
        const Set<String> well_known_names{
            "Unlocker",
            "Lyptus",
            "ScreamAPI",
            "SmokeAPI",
            "UplayR1Unlocker",
            "UplayR2Unlocker",
        };

        Set<String> well_known_modules;

        for (const auto& name: well_known_names) {
            well_known_modules.insert(name + ".dll");
            well_known_modules.insert(name + (util::is_x64() ? "64" : "32") + ".dll");
        }

        return well_known_modules;
    }

    void inject_module(const Path& path, bool required) {
        try {
            win_util::load_library_or_throw(path);

            logger->info("💉 Injected module: \"{}\"", path.string());
        } catch (const Exception& ex) {
            const auto message = fmt::format(
                "Failed to inject module \"{}\":\n\t{}", path.string(), ex.what()
            );

            if (required) {
                util::panic(message);
            } else {
                logger->warn(message);
            }
        }
    }

    /**
     * Since `directory_iterator` and `recursive_directory_iterator`
     * don't share a common interface, we have to use these dirty tricks
     * to keep the code DRY.
     */
    enum class ControlOperation {
        NO_OP,
        CONTINUE_OP,
        RETURN_OP
    };

#define PROCESS_CONTROL_OPERATION(RESULT) \
    switch (RESULT) {                     \
        case ControlOperation::CONTINUE_OP: continue; \
        case ControlOperation::RETURN_OP: return; \
        case ControlOperation::NO_OP: {}; \
    }

    ControlOperation process_file(const std::filesystem::directory_entry& entry) {
        // Skip directories
        if (entry.is_directory()) {
            return ControlOperation::CONTINUE_OP;
        }

        const auto& path = entry.path();

        // Skip files without filename
        if (not path.has_filename()) {
            return ControlOperation::CONTINUE_OP;
        }

        // Skip non-DLLs
        if (not util::strings_are_equal(path.extension().string(), ".dll")) {
            return ControlOperation::CONTINUE_OP;
        }

        const auto filename = path.filename().string();

        const static auto well_known_modules = generate_well_known_modules();

        for (const auto& dll: well_known_modules) {
            if (util::strings_are_equal(filename, dll)) {
                inject_module(path, true);

                return ControlOperation::RETURN_OP;
            }
        }

        return ControlOperation::NO_OP;
    }

    void inject_modules() {
        using namespace std::filesystem;

        if (config.auto_load) {
            logger->info("🤖 Entering auto-loading mode");

            const auto working_dir = absolute(".");

            const static directory_options dir_options = (
                directory_options::follow_directory_symlink |
                directory_options::skip_permission_denied
            );

            // First try searching in parent directories
            logger->debug("Searching in parent directories");

            auto current = working_dir;
            Path previous;
            do {
                for (const auto& entry: directory_iterator(current, dir_options)) {
                    PROCESS_CONTROL_OPERATION(process_file(entry))
                }

                previous = current;
                current = current.parent_path();
            } while (not equivalent(current, previous));

            // Then recursively go over all files in current working directory
            logger->debug("Searching in subdirectories");

            for (const auto& entry: recursive_directory_iterator(working_dir, dir_options)) {
                PROCESS_CONTROL_OPERATION(process_file(entry))
            }
        } else {
            for (const auto& module: config.modules) {
                const auto path = absolute(module.path);

                inject_module(path, module.required);
            }
        }
    }

    void init(const HMODULE& self_module) {
        try {
            DisableThreadLibraryCalls(self_module);

            const auto self_directory = loader::get_module_dir(self_module);

            config = config_parser::parse<Config>(self_directory / PROJECT_NAME".json");

            if (config.logging) {
                logger = file_logger::create(self_directory / PROJECT_NAME".log");
            }

            logger->info("🐨 {} 📥 v{}", PROJECT_NAME, PROJECT_VERSION);

            const auto exe_path = Path(win_util::get_module_file_name_or_throw(nullptr));
            const auto exe_bitness = util::is_x64() ? 64 : 32;
            logger->debug(R"(Executable path: "{}" [{}-bit])", exe_path.string(), exe_bitness);
            logger->debug(R"(Current working directory: "{}")", std::filesystem::current_path().string());

            if (config.enabled) {
                if (is_loaded_by_target()) {
                    inject_modules();
                } else {
                    logger->debug("Not loaded by target process. Skipping injections.");
                }
            } else {
                logger->debug("Koaloader is not enabled in config");
            }

            logger->info("🚀 Initialization complete");
        } catch (const Exception& ex) {
            util::panic(fmt::format("Initialization error: {}", ex.what()));
        }
    }

    void shutdown() {
        logger->info("💀 Shutdown complete");
    }
}
