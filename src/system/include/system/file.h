#pragma once

#include <string_view>

#include <boost/filesystem.hpp>

namespace ugg {
namespace system {
class file {
public:
    file(int fd, std::string_view mode = "r") : file_(fdopen(fd, mode.data())) {}
    file(boost::filesystem::path const& path, std::string_view mode = "r") : file_(fopen(path.c_str(), mode.data())) {}

    file(file const&) = delete;
    file& operator=(file const&) = delete;

    file(file&& other) : file_(nullptr) { std::swap(file_, other.file_); }

    file& operator=(file&& other) {
        file_ = nullptr;
        std::swap(file_, other.file_);
        return *this;
    }

    ~file() {
        if (file_) {
            fclose(file_);
        }
    }

    FILE* get() const { return file_; }

    std::string dump() {
        std::string result = "";
        char* line = NULL;
        size_t buffer_size = 0;
        long length = 0;
        while ((length = getline(&line, &buffer_size, file_)) != -1) {
            result += line;
        }
        free(line);
        return result;
    }

private:
    FILE* file_;
};
} // namespace system
} // namespace ugg