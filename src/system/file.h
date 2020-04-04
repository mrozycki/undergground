#pragma once

#include <string_view>

#include <boost/filesystem.hpp>

namespace ugg {
namespace system {
class file {
public:
    file(int fd, std::string_view mode = "r") : file_(fdopen(fd, mode.data())) { }
    file(boost::filesystem::path const& path, std::string_view mode = "r")
        : file_(fopen(path.c_str(), mode.data())) { }

    file(file const&) = delete;
    file& operator=(file const&) = delete;

    file(file&& other) : file_(nullptr) {
        std::swap(file_, other.file_);
    }

    file& operator=(file&& other) {
        file_ = nullptr;
        std::swap(file_, other.file_);
        return *this;
    }

    ~file() {
        fclose(file_);
    }

    FILE* get() const {
        return file_;
    }

    char getc() {
        char c;
        if (scanf("%c", &c) < 0) {
            return -1;
        } else {
            return c;
        }
    }

private:
    FILE* file_;
};
}
}