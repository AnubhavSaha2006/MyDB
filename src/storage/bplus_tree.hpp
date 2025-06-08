#pragma once
#include <map>
#include <vector>
#include <optional>
#include "rid.hpp"

class BPlusTree {
public:
    // Insert a key and its RID
    void insert(int key, const RID& rid);

    // Search for a key, return its RID if found
    std::optional<RID> search(int key) const;

    // Remove a key
    void remove(int key);

private:
    // For simplicity, use std::map as the underlying structure
    std::map<int, RID> tree_;
};
