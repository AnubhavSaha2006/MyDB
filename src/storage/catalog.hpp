#pragma once
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include "storage/schema.hpp"
#include "storage/table_heap.hpp"
#include "storage/bplus_tree.hpp"
#include "storage/rid.hpp"

using RidIndex = BPlusTree<RID>;

struct TableMeta {
    Schema                       schema;
    std::unique_ptr<TableHeap>   heap;
    std::unique_ptr<RidIndex>    index;

    // full ctor
    TableMeta(Schema s,
        std::unique_ptr<TableHeap> h,
        std::unique_ptr<RidIndex> i)
        : schema(std::move(s)), heap(std::move(h)), index(std::move(i)) {
    }

    TableMeta(const TableMeta&) = delete;
    TableMeta& operator=(const TableMeta&) = delete;
    TableMeta(TableMeta&&) = default;
    TableMeta& operator=(TableMeta&&) = default;
};

class Catalog {
public:
    explicit Catalog(BufferPoolManager* bpm) : bpm_(bpm) {}

    /* Create table */
    void create_table(const std::string& name, const Schema& schema) {
        if (tables_.contains(name)) throw std::runtime_error("table exists");

        auto heap = std::make_unique<TableHeap>(bpm_);
        auto index = std::make_unique<RidIndex>();
        tables_.emplace(name,
            TableMeta(schema, std::move(heap), std::move(index)));
    }

    /* Lookup (throws if not present) */
    TableMeta& get(const std::string& name) {
        return tables_.at(name);          // NO default construction!
    }
    bool exists(const std::string& n) const { return tables_.contains(n); }

private:
    BufferPoolManager* bpm_;
    std::unordered_map<std::string, TableMeta> tables_;
};

