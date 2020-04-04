#include "grader/compiler.h"

#include <cstdio>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "system/process.h"

namespace fs = boost::filesystem;

namespace ugg {
compiler::compiler(boost::filesystem::path const& compiler_path) : compiler_path_(compiler_path) {
}

std::optional<boost::filesystem::path> compiler::compile(boost::filesystem::path const& source_file) {
    spdlog::info("Compiling file: {}", source_file.native());

    auto output_path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    spdlog::info("Temporary file path: {}", output_path.native());
    auto compiler_process =
        ugg::system::start_process(compiler_path_, {"-O2", "-Wall", "-Werror", source_file.c_str(), "-o", output_path.c_str()});

    auto compiler_future = compiler_process.exit_future();
    if (compiler_future.wait_for(std::chrono::seconds(10)) != std::future_status::ready) {
        spdlog::error("Compilator did not finish in 10s, killing");
        compiler_process.kill();
        return {};
    }

    if (compiler_future.get().exit_status != system::exit_status::success) {
        spdlog::info("Compilation failed");
        auto& output = compiler_process.err();
        char *line = NULL;
        size_t buffer_size = 0;
        long length = 0;
        while ((length = getline(&line, &buffer_size, output.get())) != -1) {
            line[length - 1] = '\0';
            spdlog::info(">>> {}", line);
        }
        free(line);
        return {};
    }

    spdlog::info("Compilation succeeded");
    return {output_path};
}
} // namespace ugg