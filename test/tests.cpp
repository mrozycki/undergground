#include <catch2/catch.hpp>

#include <grader/grader.h>

TEST_CASE("compilation error when file does not exist", "[compiler]") {
    auto grader = ugg::grader();
    auto grade = grader.grade("0000", boost::filesystem::path("solution.cpp"));

    REQUIRE(grade.overall_grade == ugg::grade::compilation_error);
}