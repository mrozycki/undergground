#include "io_handler.h"

#include <cstdio>

#include <spdlog/spdlog.h>

namespace ugg {
std::future<void> io_handler::feed(boost::filesystem::path const& input_path) {
    return std::async(std::launch::async, [this, &input_path] {
        spdlog::info("Starting input sender");
        auto input_file = system::file(input_path);
        char input_chunk[32];
        while (fscanf(input_file.get(), "%s", input_chunk) != -1) {
            fprintf(solution_process_.in().get(), "%s\n", input_chunk);
        }
        spdlog::info("Input sender finished");
    });
}

std::future<bool> io_handler::verify_output(boost::filesystem::path const& output_path) {
    return std::async(std::launch::async, [this, &output_path] {
        spdlog::info("Starting output verifier");
        auto expected = system::file(output_path);
        auto& actual = solution_process_.out();
        char expected_chunk[32], actual_chunk[32];
        while (fscanf(actual.get(), "%s", actual_chunk) != -1 && fscanf(expected.get(), "%s", expected_chunk) != -1) {
            if (strcmp(actual_chunk, expected_chunk)) {
                return false;
            }
        }

        if (fscanf(actual.get(), "%s", actual_chunk) != -1 || fscanf(expected.get(), "%s", expected_chunk) != -1) {
            return false;
        } else {
            return true;
        }
    });
}
} // namespace ugg