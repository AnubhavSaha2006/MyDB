#include "storage/disk_manager.hpp"
#include "storage/buffer_pool_manager.hpp"
#include "storage/catalog.hpp"
#include "parser/query_parser.hpp"
#include "execution/query_executor.hpp"

int main() {
    DiskManager dm("mydb.data");
    BufferPoolManager bpm(32, &dm);
    Catalog catalog(&bpm);
    QueryExecutor exec(&catalog);

    std::string line;
    std::cout << "Mini-SQL> ";
    while (std::getline(std::cin, line)) {
        auto q = QueryParser::parse(line);
        if (!q) { std::cout << "parse error\nMini-SQL> "; continue; }
        if (auto res = exec.execute(*q)) std::cout << *res << "\n";
        std::cout << "Mini-SQL> ";
    }
}
