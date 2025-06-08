#pragma once
#include <optional>
#include "query.hpp"

class QueryParser {
public:
    static std::optional<Query> parse(const std::string& s);
};
