#pragma once

#include <filesystem>
#include <vector>

namespace ugg {
struct test {
    std::size_t id;
    std::filesystem::path input_path;
    std::filesystem::path output_path;
    int memory_limit;
    std::chrono::milliseconds time_limit;
};

class test_loader {
public:
    test_loader(std::filesystem::path data_path) : data_path_(std::move(data_path)) {}

    std::vector<test> load_tests(std::string_view problem_id) const;

private:
    std::filesystem::path data_path_;
};
} // namespace ugg
