#include "bplus_tree.hpp"

void BPlusTree::insert(int key, const RID& rid) {
    tree_[key] = rid;
}

std::optional<RID> BPlusTree::search(int key) const {
    auto it = tree_.find(key);
    if (it != tree_.end()) return it->second;
    return std::nullopt;
}

void BPlusTree::remove(int key) {
    tree_.erase(key);
}
