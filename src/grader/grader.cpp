#include "grader.h"

#include <cstring>
#include <future>
#include <sys/time.h>
#include <spdlog/spdlog.h>

#include "system/process.h"

namespace fs = boost::filesystem;

namespace {
struct test {
	std::size_t id;
	fs::path input_path;
	fs::path output_path;
	int memory_limit;
	int time_limit;
};

std::vector<test> load_tests(fs::path const& test_path) {
	std::fstream test_config(test_path / "tests");
	if (!test_config) {
		throw std::runtime_error("Cannot open test config file");
	}

	std::size_t test_count;
	test_config >> test_count;

	std::vector<test> tests;
	tests.reserve(test_count);
	for (auto i = 0; i < test_count; i++) {
		auto& test = tests.emplace_back();
		test.id = i+1;
		std::string input_filename, output_filename;
		test_config >> input_filename >> output_filename >> test.memory_limit >> test.time_limit;
		test.input_path = test_path / input_filename;
		test.output_path = test_path / output_filename;
	}
	return tests;
}

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
		tests = load_tests(fs::path("problems") / std::string(problem_id));
	} catch (std::exception const& e) {
		spdlog::error("Failed to load test config: {}", e.what());
		return -1;
	}

	for (auto const& test : tests) {
		spdlog::info("Running test {}/{}", test.id, tests.size());
		auto const final_grade = [executable_path, test] {
			auto solution_process = ugg::system::start_process(executable_path);

			auto sender = std::async(std::launch::async, [&solution_process, test] {
				spdlog::info("Starting input sender");
				auto indata = system::file(test.input_path);
				char instr[32];
				while (indata.scanf("%s", instr) != -1) {
					solution_process.in().printf("%s\n", instr);
				}
				spdlog::info("Input sender finished");
			});

			auto output_verifier = std::async(std::launch::async, [&solution_process, test] {
				spdlog::info("Starting output verifier");
				auto outdata = system::file(test.output_path);
				auto answer = solution_process.out();
				char outstr[32], ansstr[32];
				while (answer.scanf("%s", ansstr) != -1 && outdata.scanf("%s", outstr) != -1) {
					if (strcmp(ansstr, outstr)) {
						return false;
					}
				}

				if (answer.scanf("%s", ansstr) != -1 || outdata.scanf("%s", outstr) != -1) {
					return false;
				} else {
					return true;
				}
			});

			auto status_grade = [&solution_process, &test] {
				auto solution_future = solution_process.exit_future();
				if (solution_future.wait_for(std::chrono::seconds(10)) != std::future_status::ready) {
					solution_process.kill();
					return TIMEOUT;
				}

				auto solution_status = solution_future.get();
				if (solution_status.exit_status != system::exit_status::success) {
					return RUNTIME_ERROR;
				} else if (solution_status.time_taken > test.time_limit) {
					return TIME_EXCEEDED;
				} else if (solution_status.memory_usage > test.memory_limit) {
					return MEMORY_EXCEEDED;
				} else {
					return CORRECT;
				}
			}();

			sender.wait();
			output_verifier.wait();

			if (!output_verifier.get()) {
				return INCORRECT;
			} else {
				return status_grade;
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