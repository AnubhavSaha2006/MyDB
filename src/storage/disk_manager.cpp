#include "disk_manager.hpp"
#include "storage/page.hpp"

#include <iostream>

DiskManager::DiskManager(const std::string& filename) {
    file_.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_.is_open()) {
        file_.open(filename, std::ios::out | std::ios::binary);
        file_.close();
        file_.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }
}

DiskManager::~DiskManager() {
    file_.close();
}

void DiskManager::read_page(uint32_t page_id, std::byte* out_buffer) {
    file_.seekg(page_id * Page::PAGE_SIZE);
    file_.read(reinterpret_cast<char*>(out_buffer), Page::PAGE_SIZE);
}

void DiskManager::write_page(uint32_t page_id, const std::byte* buffer) {
    file_.seekp(page_id * Page::PAGE_SIZE);
    file_.write(reinterpret_cast<const char*>(buffer), Page::PAGE_SIZE);
    file_.flush();
}

