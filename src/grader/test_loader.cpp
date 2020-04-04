#include "grader/test_loader.h"

#include <spdlog/spdlog.h>

namespace fs = boost::filesystem;

namespace ugg {
std::vector<test> test_loader::load_tests(std::string_view problem_id) const {
    auto test_path = data_path_ / problem_id.data();
    spdlog::info("Loading test config from {}", fs::absolute(test_path).native());
    std::fstream test_config(test_path / "tests");
    if (!test_config) {
        throw std::runtime_error("Cannot open test config file");
    }

    std::size_t test_count;
    test_config >> test_count;

    std::vector<test> tests;
    tests.reserve(test_count);
    for (std::size_t i = 0; i < test_count; i++) {
        auto& test = tests.emplace_back();
        test.id = i + 1;
        std::string input_filename, output_filename;
        test_config >> input_filename >> output_filename >> test.memory_limit >> test.time_limit;
        test.input_path = test_path / input_filename;
        test.output_path = test_path / output_filename;
    }
    return tests;
}
} // namespace ugg