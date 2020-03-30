#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <boost/filesystem.hpp>

namespace ugg {
std::optional<boost::filesystem::path> compile(boost::filesystem::path source_file);
}