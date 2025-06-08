#include <iostream>
#include <string>
#include "storage/disk_manager.hpp"
#include "storage/buffer_pool_manager.hpp"
#include "storage/table_heap.hpp"
#include "storage/tuple.hpp"
#include "storage/rid.hpp"
#include "storage/bplus_tree.hpp"
#include "parser/query_parser.hpp"
#include "execution/query_executor.hpp"

int main() {
    DiskManager disk_manager("test.db");
    BufferPoolManager bpm(10, &disk_manager);
    TableHeap table(&bpm);
    BPlusTree index;
    QueryExecutor executor(&table, &index);

    std::cout << "Enter queries (INSERT <key> <value> or SELECT <key>):\n";
    std::string line;
    while (std::getline(std::cin, line)) {
        auto query = QueryParser::parse(line);
        if (!query) {
            std::cout << "Parse error\n";
            continue;
        }
        auto result = executor.execute(*query);
        if (result) std::cout << *result << "\n";
        else std::cout << "Execution error\n";
    }
    return 0;
}
