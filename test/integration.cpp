#include <catch2/catch.hpp>

#include <ctime>
#include <fstream>
#include <random>

#include <fmt/format.h>
#include <uuid.h>

#include <grader/compiler.h>
#include <grader/grader.h>
#include <grader/test_loader.h>

namespace fs = std::filesystem;

fs::path get_temporary_source_file() {
    static std::mt19937 generator(static_cast<unsigned long>(time(nullptr)));
    uuids::uuid_random_generator uuid_generator(generator);
    return fs::temp_directory_path() / fmt::format("{}.cpp", uuids::to_string(uuid_generator()));
}

fs::path store_source(std::string_view source) {
    auto source_file_path = get_temporary_source_file();
    std::fstream source_file(source_file_path.native(), std::ios::out);
    source_file << source;
    return source_file_path;
}

TEST_CASE("returns correct overall result", "[grader]") {
    auto grader = ugg::grader(std::make_unique<ugg::test_loader>("data"), std::make_unique<ugg::compiler>());

    SECTION("file that does not exist returns compilation error") {
        auto grade = grader.grade("0000", get_temporary_source_file());
        REQUIRE(grade.overall_grade == ugg::grade::compilation_error);
    }

    SECTION("file that does not compile returns compilation error") {
        auto source_file = store_source(R"(
            #include <iostream>

            int main() {
                error
            }
        )");

        auto grade = grader.grade("0000", source_file);
        REQUIRE(grade.overall_grade == ugg::grade::compilation_error);
    }

    SECTION("file that returns the wrong results gets 'incorrect' grade") {
        auto source_file = store_source(R"(
            #include <iostream>

            int main() {
                std::cout << "Hello\n";
            }
        )");

        auto grade = grader.grade("0000", source_file);
        REQUIRE(grade.overall_grade == ugg::grade::incorrect);
    }

    SECTION("file that returns the right result gets 'correct' grade") {
        auto source_file = store_source(R"(
            #include <iostream>

            int main() {
                std::cout << "Hello, world!\n";

                return 0;
            }
        )");

        auto grade = grader.grade("0000", source_file);
        REQUIRE(grade.overall_grade == ugg::grade::correct);
    }
}
