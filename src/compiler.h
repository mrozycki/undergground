#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace ugg {
std::optional<std::string> compile(std::string_view source_file);
}