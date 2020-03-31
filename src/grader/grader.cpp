#include "grader.h"

#include <cstring>
#include <future>
#include <spdlog/spdlog.h>

#include "system/process.h"
#include "io_handler.h"
#include "test_loader.h"

namespace fs = boost::filesystem;

namespace {
enum grade {
    TIMEOUT = 0,
    TIME_EXCEEDED = 3,
    MEMORY_EXCEEDED = 4,
    RUNTIME_ERROR = 5,
    INCORRECT = 6,
    CORRECT = 7
};
}

namespace ugg {
int grade(std::string_view problem_id, fs::path const& executable_path) {
	std::vector<test> tests;
	try {
		tests = test_loader(fs::path("problems")).load_tests(problem_id);
	} catch (std::exception const& e) {
		spdlog::error("Failed to load test config: {}", e.what());
		return -1;
	}

	for (auto const& test : tests) {
		spdlog::info("Running test {}/{}", test.id, tests.size());
		auto const final_grade = [executable_path, test] {
			auto solution_process = ugg::system::start_process(executable_path);
			auto io_handler = ugg::io_handler(solution_process);
			auto sender = io_handler.feed(test.input_path);
			auto output_verifier = io_handler.verify_output(test.output_path);

			auto solution_future = solution_process.exit_future();
			if (solution_future.wait_for(std::chrono::seconds(10)) != std::future_status::ready) {
				solution_process.kill();
				return TIMEOUT;
			}

			sender.wait();
			output_verifier.wait();

			if (!output_verifier.get()) {
				return INCORRECT;
			} else if (auto solution_status = solution_future.get();
					solution_status.exit_status != system::exit_status::success) {
				return RUNTIME_ERROR;
			} else if (solution_status.time_taken > test.time_limit) {
				return TIME_EXCEEDED;
			} else if (solution_status.memory_usage > test.memory_limit) {
				return MEMORY_EXCEEDED;
			} else {
				return CORRECT;
			}
		}();

		if (final_grade != CORRECT) {
			spdlog::info("Test #{} failed, grade: {}", test.id, final_grade);
			return final_grade + test.id * 8;
		}
	}

	spdlog::info("All tests suceeded, grade: 7");
	return CORRECT;
}
}