#include "grader/compiler.h"

#include <cstdio>
#include <ctime>

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <uuid.h>

#include "system/process.h"

namespace fs = std::filesystem;

namespace ugg {
compiler::compiler(fs::path const& compiler_path) : compiler_path_(compiler_path) {
}

namespace {
fs::path generate_random_filename() {
    static std::mt19937 generator(static_cast<unsigned long>(time(nullptr)));
    uuids::uuid_random_generator uuid_generator(generator);
    return fs::temp_directory_path() / uuids::to_string(uuid_generator());
}
} // namespace

std::optional<fs::path> compiler::compile(fs::path const& source_file) {
    spdlog::info("Compiling file: {}", source_file.native());

    auto const output_path = generate_random_filename();
    spdlog::info("Temporary file path: {}", output_path.native());
    auto compiler_process = ugg::system::start_process(
        compiler_path_, {"-O2", "-Wall", "-Werror", source_file.c_str(), "-o", output_path.c_str()});
    if (!compiler_process) {
        spdlog::error("Failed to start compiler process");
        return {};
    }

    auto compiler_future = compiler_process->exit_future();
    if (compiler_future.wait_for(std::chrono::seconds(10)) != std::future_status::ready) {
        spdlog::error("Compilator did not finish in 10s, killing, stderr: {}", compiler_process->err().dump());
        compiler_process->kill();
        return {};
    }

    if (compiler_future.get().status != system::exit_status::success) {
        spdlog::info("Compilation failed, stderr: {}", compiler_process->err().dump());
        return {};
    }

    spdlog::info("Compilation succeeded");
    return {output_path};
}
} // namespace ugg
