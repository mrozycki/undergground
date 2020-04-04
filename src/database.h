#pragma once

#include <memory>
#include <optional>
#include <string_view>

#include <mysql.h>

namespace ugg {
namespace db {
class row {
public:
    row(MYSQL_ROW row) : row_(row) {}

    std::string_view operator[](std::size_t i) const { return {row_[i]}; }

private:
    MYSQL_ROW row_;
};

class result {
public:
    result(std::unique_ptr<MYSQL_RES> result) : result_(std::move(result)) {}
    result(MYSQL_RES* result) : result_(result) {}

    operator bool() const { return result_ != nullptr; }

    std::optional<row> next_row();

private:
    std::unique_ptr<MYSQL_RES> result_;
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