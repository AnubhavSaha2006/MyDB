#pragma once

#include "buffer_pool_manager.hpp"
#include "tuple.hpp"
#include "rid.hpp"
#include <optional>

class TableHeap {
public:
    explicit TableHeap(BufferPoolManager* bpm);

    // Inserts tuple into the table
    bool insert_tuple(const Tuple& tuple, RID& rid);
    bool get_tuple(const RID& rid, Tuple& tuple);
    // table_heap.hpp
    bool delete_tuple(const RID& rid);          // NEW
    uint32_t first_page() const { return first_page_id_; }   // one-liner
    friend class QueryExecutor;
private:
    BufferPoolManager* bpm_;
    uint32_t first_page_id_;
};
