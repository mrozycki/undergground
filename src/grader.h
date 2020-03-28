#pragma once

#include <string_view>

namespace ugg {
int grade(std::string_view problem_id, std::string_view executable_path);
}