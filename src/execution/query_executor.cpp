#include "query_executor.hpp"
#include <sstream>

std::optional<std::string> QueryExecutor::execute(const Query& q) {
        try {
            return std::visit([&](auto&& cmd)->std::optional<std::string> {
                using T = std::decay_t<decltype(cmd)>;
                if constexpr (std::is_same_v<T, CreateTable>)   return exec_create(cmd);
                if constexpr (std::is_same_v<T, Insert>)        return exec_insert(cmd);
                if constexpr (std::is_same_v<T, SelectAll>)     return exec_select_all(cmd);
                if constexpr (std::is_same_v<T, SelectWhere>)   return exec_select_where(cmd);
                if constexpr (std::is_same_v<T, DeleteWhere>)   return exec_delete_where(cmd);
                return std::nullopt;
                }, q);
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return std::string("ERR: ") + e.what();
        }

}

/* ---------- helper impls ---------- */

std::string QueryExecutor::exec_create(const CreateTable& c) {
    // Schema ctor already accepts vector<ColumnDef>, no conversion needed
    cat_->create_table(c.table, Schema(c.columns));
    return "Table created";
}

std::string QueryExecutor::exec_insert(const Insert& in) {
    if (!cat_->exists(in.table)) return "ERR: no table";
    auto& tm = cat_->get(in.table);

    Tuple tup = tm.schema.serialize(in.values);
    RID rid;                       // insert into heap
    if (!tm.heap->insert_tuple(tup, rid)) return "ERR: heap full";

    int key = std::stoi(in.values[0]);   // primary key = first INT col
    tm.index->insert(key, rid);
    return "Inserted";
}

std::string QueryExecutor::exec_select_all(const SelectAll& s) {
    if (!cat_->exists(s.table)) return "ERR: no table";
    auto& tm = cat_->get(s.table);

    std::stringstream out;
    uint32_t page_id = tm.heap->first_page();

    /* pin first page */
    Page* page = tm.heap->bpm_->fetch_page(page_id);
    if (!page) return "ERR: fetch failed";

    const std::byte* raw = page->data();
    uint16_t slot_cnt = *reinterpret_cast<const uint16_t*>(raw + 2); // slot-count at offset 2

    for (uint16_t slot = 0; slot < slot_cnt; ++slot) {
        RID r(page_id, slot);
        Tuple t;
        if (!tm.heap->get_tuple(r, t)) continue;          // skip deleted / empty
        for (auto& v : tm.schema.deserialize(t)) out << v << ' ';
        out << '\n';
    }

    tm.heap->bpm_->unpin_page(page_id, false);            // don’t mark dirty
    return out.str();
}



std::string QueryExecutor::exec_select_where(const SelectWhere& sw) {
    if (!cat_->exists(sw.table)) return "ERR: no table";
    auto& tm = cat_->get(sw.table);

    int key = std::stoi(sw.value);
    auto ridOpt = tm.index->search(key);
    if (!ridOpt) return "NOT FOUND";

    Tuple t; tm.heap->get_tuple(*ridOpt, t);
    std::stringstream ss;
    for (auto& v : tm.schema.deserialize(t)) ss << v << ' ';
    return ss.str();
}

std::string QueryExecutor::exec_delete_where(const DeleteWhere& dw) {
    if (!cat_->exists(dw.table)) return "ERR: no table";
    auto& tm = cat_->get(dw.table);

    int key = std::stoi(dw.value);
    auto ridOpt = tm.index->search(key);
    if (!ridOpt) return "NOT FOUND";

    tm.heap->delete_tuple(*ridOpt);
    tm.index->remove(key);
    return "Deleted";
}
