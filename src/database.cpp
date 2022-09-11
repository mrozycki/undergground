#include "database.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace ugg {
namespace db {
std::optional<row> result::next_row() {
    if (!raw_result_) {
        return {};
    } else if (auto r = mysql_fetch_row(raw_result_.get())) {
        return {r};
    } else {
        return {};
    }
}

connection::connection(
    std::string_view hostname, std::string_view username, std::string_view password, std::string_view name) {
    if (!mysql_init(&mysql)) {
        throw std::runtime_error(fmt::format("Failed to initialize database: {}", mysql_error(&mysql)));
    }

    spdlog::info("Connecting to database at '{}'", hostname);
    if (!mysql_real_connect(&mysql, hostname.data(), username.data(), password.data(), name.data(), 0, NULL, 0)) {
        throw std::runtime_error(fmt::format("Failed to connect to database: {}", mysql_error(&mysql)));
    }
}

connection::~connection() {
    spdlog::info("Disconnecting from database");
    mysql_close(&mysql);
}

result connection::query(std::string_view query) {
    spdlog::debug("Database query: {}", query);
    if (mysql_real_query(&mysql, query.data(), query.size())) {
        throw std::runtime_error(fmt::format("Database query failed: ", mysql_error(&mysql)));
    }
    return result(mysql_store_result(&mysql));
}
} // namespace db
} // namespace ugg