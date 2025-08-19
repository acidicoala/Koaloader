#include <koalabox/globals.hpp>
#include <koalabox/io.hpp>
#include <koalabox/loader.hpp>
#include <koalabox/logger.hpp>
#include <koalabox/str.hpp>
#include <koalabox/util.hpp>
#include <koalabox/win_util.hpp>

#include <build_config.h>

#include <koaloader/koaloader.hpp>

namespace koaloader {
    Config config = {};

    const auto bitness = std::to_string(8 * sizeof(uintptr_t));

    Path self_directory;

    bool loaded = false;

    bool is_loaded_by_target() {
        if (config.targets.empty()) {
            return true;
        }

        const auto process_handle = koalabox::win_util::get_module_handle(nullptr);
        const auto executable_path = Path(koalabox::win_util::get_module_file_name(process_handle));
        const auto executable_name = executable_path.filename().string();

        bool target_found = false;
        for (const auto& target : config.targets) {
            if (koalabox::str::eq(target, executable_name)) {
                LOG_DEBUG("Target found: '{}'", target);
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

        for (const auto& name : well_known_names) {
            well_known_modules.insert(name + ".dll");
            well_known_modules.insert(name + bitness + ".dll");
        }

        return well_known_modules;
    }

    void inject_module(const Path& path, bool required) {
        try {
            koalabox::win_util::load_library_or_throw(path);

            LOG_INFO(R"(✅ Loaded module: "{}")", path.string());

            loaded = true;
        } catch (const Exception& e) {
            const auto message =
                fmt::format("Error loading module \"{}\":\n\t{}", path.string(), e.what());

            if (required) {
                koalabox::util::panic(message);
            }

            LOG_WARN("{}", message);
        }
    }

    /**
     * Since `directory_iterator` and `recursive_directory_iterator`
     * don't share a common interface, we have to use these dirty tricks
     * to keep the code DRY.
     */
    enum class ControlOperation { NO_OP, CONTINUE_OP, RETURN_OP };

#define PROCESS_CONTROL_OPERATION(RESULT)                                                          \
    switch (RESULT) {                                                                              \
    case ControlOperation::CONTINUE_OP:                                                            \
        continue;                                                                                  \
    case ControlOperation::RETURN_OP:                                                              \
        return;                                                                                    \
    case ControlOperation::NO_OP: {                                                                \
    };                                                                                             \
    }

    ControlOperation process_file(const std::filesystem::directory_entry& entry) {
        LOG_TRACE(R"(Processing file: "{}")", entry.path().string());

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
        if (not koalabox::str::eq(path.extension().string(), ".dll")) {
            return ControlOperation::CONTINUE_OP;
        }

        const auto filename = path.filename().string();

        const static auto well_known_modules = generate_well_known_modules();

        for (const auto& dll : well_known_modules) {
            if (koalabox::str::eq(filename, dll)) {
                inject_module(path, true);

                return ControlOperation::RETURN_OP;
            }
        }

        return ControlOperation::NO_OP;
    }

    void inject_modules(const Path& starting_directory) {
        using namespace std::filesystem;

        LOG_DEBUG(R"(Beginning search in "{}")", starting_directory.string());

        if (config.auto_load) {
            LOG_INFO("Entering auto-loading mode");

            const static directory_options dir_options =
                (directory_options::follow_directory_symlink |
                 directory_options::skip_permission_denied);

            // First try searching in parent directories
            LOG_DEBUG("Searching in parent directories");

            auto current = starting_directory;
            Path previous;
            do {
                for (const auto& entry : directory_iterator(current, dir_options)) {
                    PROCESS_CONTROL_OPERATION(process_file(entry))
                }

                previous = current;
                current = current.parent_path();
            } while (not equivalent(current, previous));

            // Then recursively go over all files in current working directory
            LOG_DEBUG("Searching in subdirectories");

            for (const auto& entry :
                 recursive_directory_iterator(starting_directory, dir_options)) {
                PROCESS_CONTROL_OPERATION(process_file(entry))
            }
        } else {
            for (const auto& module : config.modules) {
                auto path = Path(module.path);

                if (path.is_absolute()) {
                    inject_module(path, module.required);
                } else {
                    inject_module(path, false);

                    if (not loaded) {
                        inject_module(self_directory / path, false);
                    }

                    if (not loaded) {
                        koalabox::util::panic(
                            "Error loading module with relative path. Search locations:\n{}\n{}",
                            absolute(path).string(),
                            absolute(self_directory / path).string()
                        );
                    }
                }
            }
        }
    }

    void init(const HMODULE& self_module) {
        try {
            DisableThreadLibraryCalls(self_module);

            koalabox::globals::init_globals(self_module, PROJECT_NAME);

            self_directory = koalabox::loader::get_module_dir(self_module);

            if (const auto config_path = self_directory / "Koaloader.config.json";
                fs::exists(config_path)) {
                try {
                    const auto config_str = koalabox::io::read_file(config_path);
                    config = Json::parse(config_str);
                } catch (const std::exception& e) {
                    koalabox::util::panic(std::format("Error reading config file: {}", e.what()));
                }
            }

            if (config.logging) {
                koalabox::logger::init_file_logger(self_directory / "Koaloader.log.log");
            }

            LOG_INFO("{} v{} | Compiled at '{}'", PROJECT_NAME, PROJECT_VERSION, __TIMESTAMP__);

            const auto exe_path = koalabox::win_util::get_module_file_name_or_throw(nullptr);
            LOG_DEBUG(R"(Executable path: "{}" [{}-bit])", exe_path, bitness);
            LOG_DEBUG(
                R"(Current working directory: "{}")", std::filesystem::current_path().string()
            );
            LOG_DEBUG(R"(Koaloader directory: "{}")", self_directory.string());

            if (config.enabled) {
                if (is_loaded_by_target()) {
                    inject_modules(self_directory);

                    if (not loaded) {
                        inject_modules(std::filesystem::absolute("."));
                    }
                } else {
                    LOG_DEBUG("Not loaded by target process. Skipping injections.");
                }
            } else {
                LOG_DEBUG("Koaloader is not enabled in config");
            }

            LOG_INFO("Initialization complete");
        } catch (const Exception& e) {
            koalabox::util::panic("Initialization error: {}", e.what());
        }
    }

    void shutdown() {
        LOG_INFO("💀 Shutdown complete");
    }
}
