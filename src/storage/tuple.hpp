#pragma once

#include <vector>
#include <string>
#include <cstddef>   // std::byte
#include <cstring>   // memcpy
#include <stdexcept>

class Tuple {
public:
    Tuple() = default;

    // Construct from raw bytes pointer
    Tuple(const std::byte* data, size_t size) {
        assign(data, data + size);
    }

    // Construct from std::string (convert chars to bytes)
    Tuple(const std::string& val) {
        data_.resize(val.size());
        std::memcpy(data_.data(), val.data(), val.size());
    }

    // Assign from a byte range
    void assign(const std::byte* begin, const std::byte* end) {
        if (begin > end) {
            throw std::runtime_error("Invalid byte range");
        }
        data_.assign(begin, end);
    }

    // Access underlying data pointer (const)
    const std::byte* data() const {
        return data_.data();
    }

    // Size of the tuple data
    size_t size() const {
        return data_.size();
    }

    // Convert tuple content to std::string (for debugging / text data)
    std::string to_string() const {
        return std::string(reinterpret_cast<const char*>(data_.data()), data_.size());
    }

private:
    std::vector<std::byte> data_;
};

