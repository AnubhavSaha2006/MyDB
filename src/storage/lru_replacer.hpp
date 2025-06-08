#pragma once
#include <iostream>
#include <list>
#include <unordered_map>

class LRUReplacer {
public:
    LRUReplacer(size_t capacity) : capacity_(capacity) {}

    // Add a frame to the LRU list
    void insert(size_t frame_id) {
        // If already present, move to front
        std::cout << "LRU Insert frame: " << frame_id << "\n";

        if (map_.count(frame_id)) {
            lst_.erase(map_[frame_id]);
        }
        else if (lst_.size() >= capacity_) {
            // Evict one if full
            size_t old = lst_.back();
            lst_.pop_back();
            map_.erase(old);
        }
        lst_.push_front(frame_id);
        map_[frame_id] = lst_.begin();
    }

    // Erase a frame (e.g., if pinned)
    bool erase(size_t frame_id) {
        if (!map_.count(frame_id)) return false;
        lst_.erase(map_[frame_id]);
        map_.erase(frame_id);
        return true;
    }

    // Select and remove LRU victim
    bool victim(size_t& frame_id) {
        if (lst_.empty()) return false;
        frame_id = lst_.back();
        std::cout << "LRU Victim frame: " << frame_id << "\n";
        lst_.pop_back();
        map_.erase(frame_id);
        return true;
    }

    size_t size() const {
        return lst_.size();
    }

private:
    size_t capacity_;
    std::list<size_t> lst_;
    std::unordered_map<size_t, std::list<size_t>::iterator> map_;
};
