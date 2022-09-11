#pragma once

#include <memory>
#include <optional>
#include <string_view>

#include <mysql.h>

namespace ugg {
namespace db {
class row {
public:
    row(MYSQL_ROW raw_row) : raw_row_(raw_row) {}

    std::string_view operator[](std::size_t i) const { return {raw_row_[i]}; }

private:
    MYSQL_ROW raw_row_;
};

class result {
public:
    result(std::unique_ptr<MYSQL_RES> raw_result) : raw_result_(std::move(raw_result)) {}
    result(MYSQL_RES* raw_result) : raw_result_(raw_result) {}

    operator bool() const { return raw_result_ != nullptr; }

    std::optional<row> next_row();

private:
    std::unique_ptr<MYSQL_RES> raw_result_;
};

class connection {
public:
    connection(std::string_view hostname, std::string_view username, std::string_view password, std::string_view name);

    ~connection();

    result query(std::string_view query);

private:
    MYSQL mysql;
};
} // namespace db
} // namespace ugg