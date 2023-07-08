#include <chrono>
#include <csignal>
#include <thread>

#include <fmt/format.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "database.h"
#include "grader/grader.h"

namespace fs = std::filesystem;

bool keep_going = true;
void signal_handler(int signum) {
    if (signum == SIGKILL) {
        spdlog::info("Received kill signal");
        keep_going = false;
    } else {
        spdlog::info("Received signal: {}", signum);
    }
}

int main() try {
    signal(SIGINT, signal_handler);
    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/basic-log.txt");
    auto stderr_logger = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    auto combined_logger = spdlog::logger("combined_logger", {file_logger, stderr_logger});
    spdlog::set_default_logger(std::make_shared<spdlog::logger>(combined_logger));

    spdlog::info("Starting the grader daemon");
    spdlog::info("Initializing database");
    auto db = ugg::db::connection("127.0.0.1", "grader", "123456", "undergground");
    auto grader = ugg::grader();

    spdlog::info("Waiting for submissions");
    while (keep_going) {
        auto result = db.query(R"(
            SELECT submissions.id, category, storage
            FROM submissions, problems
            WHERE problemid = problems.id AND grade = '0'
        )");

        while (auto row = result.next_row()) {
            spdlog::info("Processing submission #{}, problem {}", (*row)[0], (*row)[1]);
            auto grader_result = grader.grade((*row)[1], fs::path("solutions") / (*row)[2].data());
            spdlog::info("Submission #{}, grade {}", (*row)[0], static_cast<int>(grader_result.overall_grade));
            db.query(fmt::format(
                R"(
                    UPDATE submissions
                    SET grade = '{}', checktime = CURRENT_TIMESTAMP
                    WHERE id = '{}'
                )",
                static_cast<int>(grader_result.overall_grade),
                (*row)[0]));
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
} catch (std::exception const& e) { spdlog::critical(e.what()); } catch (...) {
    spdlog::critical("Unkown error occurred. Exiting");
}
