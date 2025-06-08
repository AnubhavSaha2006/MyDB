#include "query_parser.hpp"
#include <sstream>

std::optional<Query> QueryParser::parse(const std::string& sql) {
    std::istringstream iss(sql);
    std::string cmd;
    iss >> cmd;
    if (cmd == "INSERT") {
        int key;
        std::string value;
        iss >> key >> value;
        if (!iss.fail()) return InsertQuery{ key, value };
    }
    else if (cmd == "SELECT") {
        int key;
        iss >> key;
        if (!iss.fail()) return SelectQuery{ key };
    }
    return std::nullopt;
}
