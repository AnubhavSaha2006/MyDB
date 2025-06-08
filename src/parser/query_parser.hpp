#pragma once
#include <string>
#include <variant>
#include <optional>
struct InsertQuery {
    int key;
    std::string value;
};

struct SelectQuery {
    int key;
};

using Query = std::variant<InsertQuery, SelectQuery>;

class QueryParser {
public:
    // Parse a simple query string into a Query object
    static std::optional<Query> parse(const std::string& sql);
};
