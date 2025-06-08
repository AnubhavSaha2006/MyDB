#include "table_heap.hpp"
#include "page.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <cstring>

constexpr size_t PAGE_HEADER_SIZE = 8; // 2 bytes free offset + 2 bytes slot count + reserved
constexpr size_t SLOT_ENTRY_SIZE = 4;  // 2 bytes offset, 2 bytes size

TableHeap::TableHeap(BufferPoolManager* bpm)
    : bpm_(bpm) {
    Page* page = bpm_->new_page(first_page_id_);
    if (!page) {
        throw std::runtime_error("Failed to allocate first table page");
    }

    // Zero out the page and initialize slot metadata
    std::memset(page->data(), 0, Page::PAGE_SIZE);

    // Initialize header: free offset = end of page, slot count = 0
    uint16_t* free_offset_ptr = reinterpret_cast<uint16_t*>(page->data());
    uint16_t* slot_count_ptr = reinterpret_cast<uint16_t*>(page->data() + 2);
    *free_offset_ptr = Page::PAGE_SIZE;
    *slot_count_ptr = 0;

    bpm_->unpin_page(first_page_id_, true);
}

bool TableHeap::insert_tuple(const Tuple& tuple, RID& rid) {
    Page* page = bpm_->fetch_page(first_page_id_);
    if (!page) return false;

    std::byte* raw = page->data();
    uint16_t* free_offset_ptr = reinterpret_cast<uint16_t*>(raw);
    uint16_t* slot_count_ptr = reinterpret_cast<uint16_t*>(raw + 2);

    size_t tuple_size = tuple.size();
    size_t required_space = tuple_size + SLOT_ENTRY_SIZE;

    size_t slot_array_end = 4 + (*slot_count_ptr) * SLOT_ENTRY_SIZE;

    if (*free_offset_ptr < slot_array_end + tuple_size) {
        bpm_->unpin_page(first_page_id_, false);
        return false; // Not enough space
    }

    // Insert tuple data at end of page (backward growth)
    *free_offset_ptr -= tuple_size;
    std::memcpy(reinterpret_cast<void*>(raw + *free_offset_ptr), tuple.data(), tuple_size);

    // Write slot entry (forward from front): offset and size
    uint16_t* slot_entry = reinterpret_cast<uint16_t*>(raw + 4 + (*slot_count_ptr) * SLOT_ENTRY_SIZE);
    slot_entry[0] = *free_offset_ptr;               // tuple offset
    slot_entry[1] = static_cast<uint16_t>(tuple_size);

    // Set RID
    rid = RID(first_page_id_, *slot_count_ptr);

    (*slot_count_ptr)++;
    bpm_->unpin_page(first_page_id_, true);
    return true;
}
bool TableHeap::get_tuple(const RID& rid, Tuple& tuple) {
    // 1. Fetch the page
    Page* page = bpm_->fetch_page(rid.page_id());
    if (!page) return false;

    std::byte* data = page->data();

    // 2. Read slot directory
    uint16_t slot_count = *reinterpret_cast<uint16_t*>(data + 2);
    if (rid.slot_id() >= slot_count) {
        bpm_->unpin_page(rid.page_id(), false);
        return false; // Invalid slot
    }

    // Each slot entry: [offset (2 bytes), size (2 bytes)]
    uint16_t* slot_entry = reinterpret_cast<uint16_t*>(data + 4 + rid.slot_id() * 4);
    uint16_t tuple_offset = slot_entry[0];
    uint16_t tuple_size = slot_entry[1];

    if (tuple_offset == 0 || tuple_size == 0) {
        bpm_->unpin_page(rid.page_id(), false);
        return false; // Deleted or empty slot
    }

    // 3. Copy tuple data
    tuple = Tuple(reinterpret_cast<const std::byte*>(data + tuple_offset), tuple_size);

    bpm_->unpin_page(rid.page_id(), false);
    return true;
}
// table_heap.cpp
bool TableHeap::delete_tuple(const RID& rid) {
    Page* page = bpm_->fetch_page(rid.page_id());
    if (!page) return false;
    auto* data = page->data();
    uint16_t* slot = reinterpret_cast<uint16_t*>(data + 4 + rid.slot_id() * 4);
    slot[0] = slot[1] = 0;          // mark empty
    bpm_->unpin_page(rid.page_id(), true);
    return true;
}



