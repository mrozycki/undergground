#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <boost/filesystem.hpp>

namespace ugg {
class compiler {
public:
    explicit compiler(boost::filesystem::path const& compiler_path = {"/usr/bin/g++"});

    std::optional<boost::filesystem::path> compile(boost::filesystem::path const& source_file);

private:
    boost::filesystem::path compiler_path_;
};
} // namespace ugg