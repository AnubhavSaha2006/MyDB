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
private:
    BufferPoolManager* bpm_;
    uint32_t first_page_id_;
};
