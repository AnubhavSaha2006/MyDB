#pragma once
#include <cstdint>

class RID {
public:
    RID() = default;
    RID(uint32_t page_id, uint16_t slot_id)
        : page_id_(page_id), slot_id_(slot_id) {
    }

    uint32_t page_id() const { return page_id_; }
    uint16_t slot_id() const { return slot_id_; }

    bool operator==(const RID& other) const {
        return page_id_ == other.page_id_ && slot_id_ == other.slot_id_;
    }

private:
    uint32_t page_id_;
    uint16_t slot_id_;
};
