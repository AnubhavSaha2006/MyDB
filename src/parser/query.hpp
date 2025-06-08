#pragma once
#include <string>
#include <vector>
#include <variant>

enum class ColumnType { INT, CHAR };

struct ColumnDef { std::string name; ColumnType type; std::size_t len; };

/* — AST — */
struct CreateTable {
    std::string              table;
    std::vector<ColumnDef>   columns;
};
struct Insert {
    std::string              table;
    std::vector<std::string> values;   // raw literals
};
struct SelectAll { std::string table; };
struct SelectWhere { std::string table; std::string col; std::string value; };
struct DeleteWhere { std::string table; std::string col; std::string value; };

using Query = std::variant<CreateTable, Insert, SelectAll, SelectWhere, DeleteWhere>;
