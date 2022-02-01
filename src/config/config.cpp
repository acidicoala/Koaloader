#include "config.hpp"
#include "util/util.hpp"

#include <fstream>
#include <memory>

namespace config {

    Config read(Path path) { // NOLINT(performance-unnecessary-value-param)
        if (not std::filesystem::exists(path)) {
            return {};
        }

        try {
            std::ifstream ifs(path);
            nlohmann::json json;
            ifs >> json;

            return json.get<Config>();
        } catch (const std::exception& ex) {
            util::error_box(__func__, fmt::format("Failed to parse config file: {}", ex.what()));
            exit(GetLastError()); // NOLINT(cppcoreguidelines-narrowing-conversions)
        }
    }

}
