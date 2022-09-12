#include "grader/test_loader.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

namespace ugg {
std::vector<test> test_loader::load_tests(std::string_view problem_id) const {
    auto test_path = data_path_ / problem_id;
    spdlog::info("Loading test config from {}", fs::absolute(test_path).native());

    auto const config = YAML::LoadFile((test_path / "tests.yaml").string());
    std::vector<test> tests;
    tests.reserve(config["tests"].size());

    std::size_t i = 0;
    for (auto const& test_config : config["tests"]) {
        auto& test = tests.emplace_back();
        test.id = ++i;
        test.input_path = test_path / test_config["input_file"].as<std::string>();
        test.output_path = test_path / test_config["output_file"].as<std::string>();
        test.memory_limit = test_config["memory_limit"].as<int>();
        test.time_limit = test_config["time_limit"].as<int>();
    }
    return tests;
}
} // namespace ugg
