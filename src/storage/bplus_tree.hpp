/************************  bplus_tree.hpp  ***********************
 * Header-only generic B+-tree (order = max keys per node)
 * - key   : int
 * - value : templated ValueT   (e.g. RID, Tuple, std::string…)
 *****************************************************************/
#pragma once
#include <vector>
#include <memory>
#include <optional>
#include <algorithm>

template <typename ValueT>
class BPlusTree {
public:
    explicit BPlusTree(int order = 4) : order_(order) {
        root_ = std::make_shared<Node>(true);
    }

    /* ================= PUBLIC API ================ */

    /// returns false if duplicate key
    bool insert(int key, const ValueT& value) {
        int up_key = 0; NodePtr new_child{};
        if (!insert_inner(root_, key, value, up_key, new_child)) return false;

        if (new_child) {                               // root split
            auto new_root = std::make_shared<Node>(false);
            new_root->keys = { up_key };
            new_root->children = { root_, new_child };
            root_ = new_root;
        }
        return true;
    }

    std::optional<ValueT> search(int key) const {
        return search_inner(root_, key);
    }

    bool remove(int key) {                             // simple (no merge)
        return remove_inner(root_, key);
    }

private:
    /* ================= NODE DEF ================== */
    struct Node;
    using NodePtr = std::shared_ptr<Node>;
    struct Node {
        bool is_leaf;
        std::vector<int>     keys;
        std::vector<NodePtr> children;   // internal
        std::vector<ValueT>  values;     // leaf
        NodePtr              next;       // leaf chaining
        explicit Node(bool leaf) : is_leaf(leaf) {}
    };

    NodePtr root_;
    int     order_;                      // max keys

    /* ================= HELPERS =================== */
    static int lower(const std::vector<int>& v, int k) {
        return std::lower_bound(v.begin(), v.end(), k) - v.begin();
    }
    static int upper(const std::vector<int>& v, int k) {
        return std::upper_bound(v.begin(), v.end(), k) - v.begin();
    }

    /* ---------- insertion recursion ------------- */
    bool insert_inner(NodePtr n, int key, const ValueT& val,
        int& up_key, NodePtr& new_child)
    {
        /* leaf */
        if (n->is_leaf) {
            int pos = lower(n->keys, key);
            if (pos < n->keys.size() && n->keys[pos] == key) return false; // dup

            n->keys.insert(n->keys.begin() + pos, key);
            n->values.insert(n->values.begin() + pos, val);

            if ((int)n->keys.size() < order_) { new_child = nullptr; return true; }
            split_leaf(n, up_key, new_child);
            return true;
        }

        /* internal */
        int idx = upper(n->keys, key);
        int promote{}; NodePtr child_split{};
        if (!insert_inner(n->children[idx], key, val, promote, child_split)) return false;

        if (child_split) {
            n->keys.insert(n->keys.begin() + idx, promote);
            n->children.insert(n->children.begin() + idx + 1, child_split);
            if ((int)n->keys.size() < order_) { new_child = nullptr; return true; }
            split_internal(n, up_key, new_child);
        }
        else new_child = nullptr;
        return true;
    }

    void split_leaf(NodePtr leaf, int& up_key, NodePtr& new_leaf) {
        int mid = leaf->keys.size() / 2;
        new_leaf = std::make_shared<Node>(true);
        new_leaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
        new_leaf->values.assign(leaf->values.begin() + mid, leaf->values.end());
        leaf->keys.resize(mid);
        leaf->values.resize(mid);

        new_leaf->next = leaf->next;
        leaf->next = new_leaf;
        up_key = new_leaf->keys.front();
    }

    void split_internal(NodePtr n, int& up_key, NodePtr& new_n) {
        int mid = n->keys.size() / 2;
        up_key = n->keys[mid];
        new_n = std::make_shared<Node>(false);

        new_n->keys.assign(n->keys.begin() + mid + 1, n->keys.end());
        new_n->children.assign(n->children.begin() + mid + 1, n->children.end());
        n->keys.resize(mid);
        n->children.resize(mid + 1);
    }

    /* ---------- search ---------- */
    std::optional<ValueT> search_inner(NodePtr n, int key) const {
        if (n->is_leaf) {
            int pos = lower(n->keys, key);
            if (pos < n->keys.size() && n->keys[pos] == key) return n->values[pos];
            return std::nullopt;
        }
        int idx = upper(n->keys, key);
        return search_inner(n->children[idx], key);
    }

    /* ---------- remove (simple) ---------- */
    bool remove_inner(NodePtr n, int key) {
        if (n->is_leaf) {
            int pos = lower(n->keys, key);
            if (pos == n->keys.size() || n->keys[pos] != key) return false;
            n->keys.erase(n->keys.begin() + pos);
            n->values.erase(n->values.begin() + pos);
            return true;
        }
        int idx = upper(n->keys, key);
        return remove_inner(n->children[idx], key);
    }
};
