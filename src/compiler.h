#pragma once

#include <optional>
#include <string>

namespace ugg {
std::optional<std::string> compile(std::string const& source_file);
}