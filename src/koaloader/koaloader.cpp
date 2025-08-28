#include <set>

#include <koalabox/config.hpp>
#include <koalabox/globals.hpp>
#include <koalabox/loader.hpp>
#include <koalabox/logger.hpp>
#include <koalabox/path.hpp>
#include <koalabox/str.hpp>
#include <koalabox/util.hpp>
#include <koalabox/win.hpp>

#include <build_config.h>

#include <koaloader/koaloader.hpp>

namespace {
    namespace kb = koalabox;
    namespace fs = std::filesystem;

    fs::path self_directory;

    bool loaded = false;

    bool is_loaded_by_target() {
        if(koaloader::config.targets.empty()) {
            return true;
        }

        static const auto executable_path = koalabox::win::get_module_path(nullptr);
        static const auto executable_name = kb::path::to_str(executable_path.filename());

        bool target_found = false;
        for(const auto& target : koaloader::config.targets) {
            if(koalabox::str::eq(target, executable_name)) {
                LOG_DEBUG("Target found: '{}'", target);
                target_found = true;
                break;
            }
        }

        return target_found;
    }

    std::set<std::string> generate_well_known_modules() {
        const std::set<std::string> well_known_names{
            "Unlocker",
            "Lyptus",
            "ScreamAPI",
            "SmokeAPI",
            "UplayR1Unlocker",
            "UplayR2Unlocker",
            "KoaloaderA",
            "KoaloaderB",
            "KoaloaderC",
        };

        std::set<std::string> well_known_modules;

        for(const auto& name : well_known_names) {
            well_known_modules.insert(name + ".dll");
            well_known_modules.insert(std::format("{}{}.dll", name, kb::util::BITNESS));
        }

        return well_known_modules;
    }

    void inject_module(const fs::path& path, const bool required) {
        try {
            koalabox::win::load_library_or_throw(path);

            LOG_INFO(R"(✅ Loaded module: "{}")", path.string());

            loaded = true;
        } catch(const std::exception& e) {
            const auto message = fmt::format(
                R"(Error loading module "{}": {})",
                path.string(),
                e.what()
            );

            if(required) {
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
        case ControlOperation::CONTINUE_OP:                                                        \
            continue;                                                                              \
        case ControlOperation::RETURN_OP:                                                          \
            return;                                                                                \
        case ControlOperation::NO_OP: {                                                            \
        };                                                                                         \
    }

    ControlOperation process_file(const std::filesystem::directory_entry& entry) {
        LOG_TRACE(R"(Processing file: "{}")", entry.path().string());

        // Skip directories
        if(entry.is_directory()) {
            return ControlOperation::CONTINUE_OP;
        }

        const auto& path = entry.path();

        // Skip files without filename
        if(not path.has_filename()) {
            return ControlOperation::CONTINUE_OP;
        }

        // Skip non-DLLs
        if(not koalabox::str::eq(path.extension().string(), ".dll")) {
            return ControlOperation::CONTINUE_OP;
        }

        const auto filename = path.filename().string();

        const static auto well_known_modules = generate_well_known_modules();

        for(const auto& dll : well_known_modules) {
            if(koalabox::str::eq(filename, dll)) {
                inject_module(path, true);

                return ControlOperation::RETURN_OP;
            }
        }

        return ControlOperation::NO_OP;
    }

    void inject_modules(const fs::path& starting_directory) {
        LOG_DEBUG(R"(Beginning search in "{}")", kb::path::to_str(starting_directory));

        if(koaloader::config.auto_load) {
            LOG_INFO("Entering auto-loading mode");

            static constexpr fs::directory_options dir_options =
                fs::directory_options::follow_directory_symlink |
                fs::directory_options::skip_permission_denied;

            // First try searching in parent directories
            LOG_DEBUG("Searching in parent directories");

            auto current = starting_directory;
            fs::path previous;
            do {

                for(const auto& entry : fs::directory_iterator(current, dir_options)) {
                    PROCESS_CONTROL_OPERATION(process_file(entry))
                }

                previous = current;
                current = current.parent_path();
            } while(not equivalent(current, previous));

            // Then recursively go over all files in current working directory
            LOG_DEBUG("Searching in subdirectories");

            for(const auto& entry : fs::recursive_directory_iterator(starting_directory, dir_options)) {
                PROCESS_CONTROL_OPERATION(process_file(entry))
            }
        } else {
            for(const auto& module : koaloader::config.modules) {
                const auto path = kb::path::from_str(module.path);

                if(path.is_absolute()) {
                    inject_module(path, module.required);
                } else {
                    inject_module(path, false);

                    if(not loaded) {
                        inject_module(self_directory / path, false);
                    }

                    if(not loaded) {
                        koalabox::util::panic(
                            std::format(
                                "Error loading module with relative path. Search locations:\n{}\n{}",
                                kb::path::to_str(absolute(path)),
                                kb::path::to_str(absolute(self_directory / path).string())
                            )
                        );
                    }
                }
            }
        }
    }
}

namespace koaloader {
    Config config{};

    void init(const HMODULE self_module) {
        try {
            kb::globals::init_globals(self_module, PROJECT_NAME);

            self_directory = kb::win::get_module_path(self_module).parent_path();

            config = kb::config::parse<Config>();

            if(config.logging) {
                koalabox::logger::init_file_logger(kb::paths::get_log_path());
            }

            LOG_INFO("{} v{} | Built at '{}'", PROJECT_NAME, PROJECT_VERSION, __TIMESTAMP__);
            LOG_DEBUG("Parsed config:\n{}", nlohmann::ordered_json(config).dump(2));

            const auto exe_path = koalabox::win::get_module_path(nullptr);

            LOG_DEBUG(
                R"(Executable path: "{}" [{}-bit])",
                kb::path::to_str(exe_path),
                kb::util::BITNESS
            );
            LOG_DEBUG(
                R"(Current working directory: "{}")",
                kb::path::to_str(std::filesystem::current_path())
            );
            LOG_DEBUG(R"(Koaloader directory: "{}")", kb::path::to_str(self_directory));

            if(config.enabled) {
                if(is_loaded_by_target()) {
                    inject_modules(self_directory);

                    if(not loaded) {
                        inject_modules(std::filesystem::absolute("."));
                    }
                } else {
                    LOG_DEBUG("Not loaded by target process. Skipping injections.");
                }
            } else {
                LOG_DEBUG("Koaloader is not enabled in config");
            }

            LOG_INFO("Initialization complete");
        } catch(const std::exception& e) {
            koalabox::util::panic(std::format("Initialization error: {}", e.what()));
        }
    }

    void shutdown() {
        LOG_INFO("Shutdown complete");
    }
}
