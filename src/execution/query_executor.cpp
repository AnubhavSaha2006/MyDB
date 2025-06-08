#include "query_executor.hpp"
#include "../storage/tuple.hpp"

QueryExecutor::QueryExecutor(TableHeap* table, BPlusTree* index)
    : table_(table), index_(index) {
}

std::optional<std::string> QueryExecutor::execute(const Query& query) {
    if (std::holds_alternative<InsertQuery>(query)) {
        const auto& q = std::get<InsertQuery>(query);
        Tuple tuple(q.value);
        RID rid;
        if (table_->insert_tuple(tuple, rid)) {
            index_->insert(q.key, rid);
            return "Inserted";
        }
        else {
            return "Insert failed";
        }
    }
    else if (std::holds_alternative<SelectQuery>(query)) {
        const auto& q = std::get<SelectQuery>(query);
        auto rid_opt = index_->search(q.key);
        if (rid_opt) {
            Tuple tuple;
            if (table_->get_tuple(*rid_opt, tuple)) {
                return tuple.to_string();
            }
        }
        return "Not found";
    }
    return std::nullopt;
}
