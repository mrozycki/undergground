#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "compiler.h"
#include "test_loader.h"

namespace ugg {
enum class grade {
    timeout = 0,
    internal_error = 1,
    compilation_error = 2,
    time_exceeded = 3,
    memory_exceeded = 4,
    runtime_error = 5,
    incorrect = 6,
    correct = 7
};

struct test_result {
    grade test_grade;
    std::string additional_information;
};

struct grader_result {
    grade overall_grade;
    std::vector<test_result> test_results;
};

class grader {
public:
    explicit grader(std::unique_ptr<test_loader> test_loader, std::unique_ptr<compiler> compiler)
        : test_loader_(std::move(test_loader)), compiler_(std::move(compiler)) {}

    grader()
        : grader(
              std::make_unique<test_loader>(std::filesystem::path("problems")),
              std::make_unique<compiler>(std::filesystem::path("/usr/bin/g++"))) {}

    grader_result grade(std::string_view problem_id, std::filesystem::path const& source_file) const;

private:
    std::unique_ptr<test_loader> test_loader_;
    std::unique_ptr<compiler> compiler_;
};
} // namespace ugg
