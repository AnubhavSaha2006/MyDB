#pragma once
#include "../storage/bplus_tree.hpp"
#include "../storage/table_heap.hpp"
#include "../parser/query_parser.hpp"
#include <string>
#include <optional>

class QueryExecutor {
public:
    QueryExecutor(TableHeap* table, BPlusTree* index);

    // Execute a parsed query and return result as string
    std::optional<std::string> execute(const Query& query);

private:
    TableHeap* table_;
    BPlusTree* index_;
};
