#include "grader.h"

#include <cstring>
#include <future>
#include <spdlog/spdlog.h>

#include "system/process.h"
#include "compiler.h"
#include "io_handler.h"

namespace fs = boost::filesystem;

namespace ugg{
namespace {
test_result run_test(test const& test, fs::path const& executable_path) {
	auto solution_process = ugg::system::start_process(executable_path);
	auto io_handler = ugg::io_handler(solution_process);
	auto sender = io_handler.feed(test.input_path);
	auto output_verifier = io_handler.verify_output(test.output_path);

	auto solution_future = solution_process.exit_future();
	if (solution_future.wait_for(std::chrono::seconds(10)) != std::future_status::ready) {
		solution_process.kill();
		return {grade::timeout, {}};
	}
	sender.wait();

	if (!output_verifier.get()) {
		return {grade::incorrect, {}};
	} else if (auto solution_status = solution_future.get();
			solution_status.exit_status != system::exit_status::success) {
		return {grade::runtime_error, {}};
	} else if (solution_status.time_taken > test.time_limit) {
		return {grade::time_exceeded, {}};
	} else if (solution_status.memory_usage > test.memory_limit) {
		return {grade::memory_exceeded, {}};
	} else {
		return {grade::correct, {}};
	}
}
}

grader_result grader::grade(std::string_view problem_id, fs::path const& source_file) const {
	auto executable_path = ugg::compile(source_file);
	if (!executable_path) {
		return {ugg::grade::compilation_error, {}};
	}

	std::vector<test> tests;
	try {
		tests = test_loader(fs::path("problems")).load_tests(problem_id);
	} catch (std::exception const& e) {
		spdlog::error("Failed to load test config: {}", e.what());
		return {ugg::grade::internal_error, {}};
	}

	grader_result result{grade::correct, {}};
	for (auto const& test : tests) {
		spdlog::info("Running test {}/{}", test.id, tests.size());
		auto const test_result = result.test_results.emplace_back(run_test(test, *executable_path));

		if (test_result.test_grade != ugg::grade::correct) {
			spdlog::info("Test #{} failed", test.id);
			if (result.overall_grade == ugg::grade::correct) {
				result.overall_grade = test_result.test_grade;
			}
		}
	}
	return result;
}
}