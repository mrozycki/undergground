#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace ugg {
class compiler {
public:
    explicit compiler(std::filesystem::path const& compiler_path = {"/usr/bin/g++"});

    std::optional<std::filesystem::path> compile(std::filesystem::path const& source_file);

private:
    std::filesystem::path compiler_path_;
};
} // namespace ugg
