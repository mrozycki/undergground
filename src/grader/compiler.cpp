#include "compiler.h"

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
        ugg::system::start_process(compiler_path_, {"-O2", "-Wall", source_file.c_str(), "-o", output_path.c_str()});

    if (compiler_process.err().getc() == EOF) {
        spdlog::info("Compilation suceeded");
        return {output_path};
    } else {
        spdlog::info("Compilation failed");
        compiler_process.kill();
        return {};
    }
}
} // namespace ugg