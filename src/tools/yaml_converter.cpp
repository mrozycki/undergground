#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

void convert_config(fs::path const& in_path, fs::path const& out_path, std::string const& id) {
    std::ifstream in(in_path);
    std::ofstream out(out_path);

    out << "id: " << id << "\n";
    out << "tests:\n";

    std::size_t number_of_tests;
    in >> number_of_tests;
    for (std::size_t i = 0; i < number_of_tests; ++i) {
        std::string in_file, out_file;
        std::size_t memory_limit, time_limit;
        in >> in_file >> out_file >> memory_limit >> time_limit;

        out << "  - input_file: " << in_file << "\n";
        out << "    output_file: " << out_file << "\n";
        out << "    memory_limit: " << memory_limit << "\n";
        out << "    time_limit: " << time_limit << "\n";
    }
}

void process_directory(fs::path const& path) {
    for (auto const& entry : fs::directory_iterator(path)) {
        if (!entry.is_directory()) {
            continue;
        }
        auto const id = entry.path().filename();
        std::cout << entry.path() << "\n";
        auto const tests_path = entry.path() / "tests";
        if (fs::exists(tests_path)) {
            convert_config(tests_path, entry.path() / "tests.yaml", id);
        } else {
            process_directory(entry.path());
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path to problem data>\n";
        return -1;
    }

    process_directory(fs::path(argv[1]));

    return 0;
}
