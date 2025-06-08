#pragma once

#include <vector>
#include <unordered_map>
#include <optional>
#include <stdexcept>
#include "page.hpp"
#include "disk_manager.hpp"
#include "lru_replacer.hpp"

class BufferPoolManager {
public:
    BufferPoolManager(size_t pool_size, DiskManager* disk_manager)
        : pool_size_(pool_size),
        pages_(pool_size),
        page_table_(),
        pinned_(pool_size, false),
        disk_manager_(disk_manager),
        next_page_id_(0),
        replacer_(pool_size) {
    }


    // Fetch a page from buffer pool (or load from disk)
    Page* fetch_page(uint32_t page_id) {
        // Check if page is already in memory
        if (page_table_.count(page_id)) {
            size_t frame_id = page_table_[page_id];
            pinned_[frame_id] = true;
            return &pages_[frame_id];
        }

        // Page not in memory — need to load into a frame
        std::optional<size_t> free_frame = find_free_frame();
        if (!free_frame.has_value()) {
            size_t victim_frame;
            if (!replacer_.victim(victim_frame)) {
                throw std::runtime_error("No free frame available and LRU empty");
            }

            // Evict victim
            uint32_t evicted_page = 0;
            for (auto it = page_table_.begin(); it != page_table_.end(); ++it) {
                if (it->second == victim_frame) {
                    evicted_page = it->first;
                    break;
                }
            }
            flush_page(evicted_page);
            page_table_.erase(evicted_page);
            free_frame = victim_frame;
        }

        size_t frame_id = free_frame.value();
        Page& page = pages_[frame_id];

        // Load content from disk
        disk_manager_->read_page(page_id, page.get_data().data());

        page.set_page_id(page_id);
        pinned_[frame_id] = true;
        page_table_[page_id] = frame_id;

        return &page;
    }

    // Unpin a page when done using it
    bool unpin_page(uint32_t page_id, bool is_dirty) {
        if (!page_table_.count(page_id)) return false;
        size_t frame_id = page_table_[page_id];
        pinned_[frame_id] = false;
        replacer_.insert(frame_id);

        // For now, we treat all unpinned pages as dirty and flush them
        if (is_dirty) {
            return flush_page(page_id);
        }
        return true;
    }

    // Flush (write back) a page to disk
    bool flush_page(uint32_t page_id) {
        if (!page_table_.count(page_id)) return false;
        size_t frame_id = page_table_[page_id];
        Page& page = pages_[frame_id];
        disk_manager_->write_page(page.get_page_id(), page.get_data().data());
        return true;
    }

    // Create a new page with a new page_id
    Page* new_page(uint32_t& new_page_id) {
        std::optional<size_t> free_frame = find_free_frame();
        if (!free_frame.has_value()) return nullptr;

        size_t frame_id = free_frame.value();
        Page& page = pages_[frame_id];

        new_page_id = next_page_id_++;
        page_table_[new_page_id] = frame_id;

        page.reset_memory();
        page.set_page_id(new_page_id);
        pinned_[frame_id] = true;

        return &page;
    }

private:
    size_t pool_size_;
    std::vector<Page> pages_;
    std::unordered_map<uint32_t, size_t> page_table_;
    std::vector<bool> pinned_;
    DiskManager* disk_manager_;
    uint32_t next_page_id_;
    LRUReplacer replacer_;


    std::optional<size_t> find_free_frame() {
        for (size_t i = 0; i < pool_size_; ++i) {
            if (!pinned_[i]) {
                // Remove old mapping if page was previously mapped
                for (auto it = page_table_.begin(); it != page_table_.end(); ++it) {
                    if (it->second == i) {
                        page_table_.erase(it);
                        break;
                    }
                }
                return i;
            }
        }
        return std::nullopt;
    }
};
