#include "compiler.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "process.h"

namespace ugg {
std::optional<std::string> compile(std::string_view source_file) {
    spdlog::info("Compiling file {}", source_file);

    auto command = fmt::format("g++ -O2 -Wall solutions/{} -o solution 2>&1", source_file);
    spdlog::debug("Command: {}", command);

    auto compiler_process = ugg::process(command.c_str());
    if (compiler_process.read_char() == EOF) {
        spdlog::info("Compilation suceeded");
        return {"solution"};
    } else {
        spdlog::info("Compilation failed");
        return {};
    }
}
}