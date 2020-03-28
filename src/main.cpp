#include <chrono>
#include <csignal>
#include <thread>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "compiler.h"
#include "database.h"
#include "grader.h"

bool keep_going = true;
void signal_handler(int signum) {
	spdlog::info("Received kill signal");
	keep_going = false;
}

int process_submission(std::string_view id, std::string_view problem, std::string_view submission_file) {
	spdlog::info("Processing submission request #{}; problem {}", id, problem);
	auto executable_path = ugg::compile(submission_file);
	if (!executable_path) {
		return 2;
	}

	return ugg::grade(problem, *executable_path);
}

int main()
try {
	signal(SIGINT, signal_handler);

	spdlog::info("Starting the grader daemon");
	spdlog::info("Initializing database");
	auto db = ugg::db::connection("127.0.0.1", "grader", "123456", "undergground");

	spdlog::info("Waiting for submissions");
	while (keep_going)
	{
		auto result = db.query(R"(
			SELECT submissions.id, category, storage
			FROM submissions, problems
			WHERE problemid = problems.id AND grade = '0'
		)");

		while (auto row = result.next_row())
		{
			auto grade = process_submission((*row)[0], (*row)[1], (*row)[2]);
			spdlog::info("Storing grade in database");
			db.query(fmt::format(R"(
				UPDATE submissions
				SET grade = '{}', checktime = CURRENT_TIMESTAMP
				WHERE id = '{}'
			)", grade, (*row)[0]));
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
} catch (std::exception const& e) {
	spdlog::critical(e.what());
} catch (...) {
	spdlog::critical("Unkown error occurred. Exiting");
}
