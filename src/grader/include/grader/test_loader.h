#pragma once

#include <boost/filesystem.hpp>

namespace ugg {
struct test {
    std::size_t id;
    boost::filesystem::path input_path;
    boost::filesystem::path output_path;
    int memory_limit;
    int time_limit;
};

class test_loader {
public:
    test_loader(boost::filesystem::path data_path) : data_path_(std::move(data_path)) {}

    std::vector<test> load_tests(std::string_view problem_id) const;

private:
    boost::filesystem::path data_path_;
};
} // namespace ugg
