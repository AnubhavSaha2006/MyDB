#pragma once
#include <vector>
#include <string>

enum class Type {
    INTEGER,
    STRING
};

struct Column {
    std::string name;
    Type type;
};

class Schema {
public:
    Schema(std::vector<Column> cols) : columns_(std::move(cols)) {}

    const std::vector<Column>& columns() const { return columns_; }

private:
    std::vector<Column> columns_;
};
