#pragma once

#include <cstddef>   // for std::byte
#include <array>
#include <span>
#include<stdexcept> // for std::runtime_error
#include <cstring>   // for memcpy
#include <cstdint>   // for fixed-width ints

class Page {
public:
    static constexpr size_t PAGE_SIZE = 4096;
    static constexpr size_t HEADER_SIZE = 8;  // 4 bytes for page_id + 4 for lsn

    Page() {
        reset_memory();
    }

    // Resets the memory to zero
    void reset_memory() {
        std::memset(data_.data(), 0, PAGE_SIZE);
    }

    // Get the full raw buffer (for DiskManager)
    std::span<std::byte> get_data() {
        return std::span<std::byte>(data_);
    }

    // Write something to the usable part of the page
    void write_content(const std::byte* src, size_t offset, size_t len) {
        if (offset + len > usable_size()) throw std::runtime_error("write overflow");
        std::memcpy(data_.data() + HEADER_SIZE + offset, src, len);
    }

    // Read content from the page
    void read_content(std::byte* dest, size_t offset, size_t len) const {
        if (offset + len > usable_size()) throw std::runtime_error("read overflow");
        std::memcpy(dest, data_.data() + HEADER_SIZE + offset, len);
    }

    // Page ID getter/setter (4 bytes at offset 0)
    uint32_t get_page_id() const {
        uint32_t val;
        std::memcpy(&val, data_.data(), sizeof(uint32_t));
        return val;
    }

    void set_page_id(uint32_t pid) {
        std::memcpy(data_.data(), &pid, sizeof(uint32_t));
    }

    // Log Sequence Number getter/setter (next 4 bytes)
    uint32_t get_lsn() const {
        uint32_t val;
        std::memcpy(&val, data_.data() + 4, sizeof(uint32_t));
        return val;
    }

    void set_lsn(uint32_t lsn) {
        std::memcpy(data_.data() + 4, &lsn, sizeof(uint32_t));
    }

    size_t usable_size() const {
        return PAGE_SIZE - HEADER_SIZE;
    }
    std::byte* data() {                // non-const
        return data_.data();
    }
    const std::byte* data() const {    // const overload
        return data_.data();
    }
private:
    std::array<std::byte, PAGE_SIZE> data_;
};
