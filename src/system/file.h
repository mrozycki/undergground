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

    ~file() {
        // fclose(file_);
    }

    template<typename FormatString, typename ...Args>
    int printf(FormatString const& format, Args const&... args) {
        return fprintf(file_, format, args...);
    }

    template<typename FormatString, typename ...Args>
    int scanf(FormatString const& format, Args&&... args) {
        return fscanf(file_, format, std::forward<Args>(args)...);
    }

    int getc() {
        int c;
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