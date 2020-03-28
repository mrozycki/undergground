#pragma once

namespace ugg {
class process {
public:
    process(char const* command)
        : process_file(popen(command, "r")) {}

    ~process() {
        pclose(process_file);
    }

    char read_char() {
        return fgetc(process_file);
    }

private:
    FILE* process_file;
};
}