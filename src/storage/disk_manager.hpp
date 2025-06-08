#pragma once
#include <string>
#include <fstream>
#include <vector>

class DiskManager {
public:
    DiskManager(const std::string& filename);
    ~DiskManager();

    void read_page(uint32_t page_id, std::byte* out_buffer);
    void write_page(uint32_t page_id, const std::byte* buffer);


private:
    std::fstream file_;
    const size_t PAGE_SIZE = 4096;
};
