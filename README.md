
---

#  MyDB — Lightweight In-Memory DBMS in Modern C++

A minimal yet powerful educational database system implemented in C++. It demonstrates key concepts in storage management, tuple encoding, buffer management, and basic SQL-like query execution, using modern C++ techniques and STL components.

---

##  Features Implemented

*  **Disk Manager**: Abstracted I/O for page-level storage
*  **Buffer Pool Manager**: Caches disk pages in memory
*  **LRU Replacer**: Page replacement strategy
*  **Page Abstraction**: Raw byte-buffer with metadata
*  **Table Heap**: Slotted-page heap file for tuple storage
*  **RID (Record ID)**: Logical location reference for tuples
*  **B+ Tree Index**: Simple in-memory `std::map`-based index
*  **Tuple Encoding**: Stores byte-level tuples (strings, ints)
*  **SQL-like Queries**: `INSERT`, `SELECT` supported
*  **Query Execution Engine**: Executes simple queries from CLI

---

##  File Structure Overview

```
src/
├── main.cpp                     # Entry point: accepts and executes user queries
├── execution/
│   ├── query_executor.hpp/cpp  # Runs queries using table + index
├── parser/
│   ├── query_parser.hpp/cpp    # Parses strings like "INSERT 1 value"
├── storage/
│   ├── buffer_pool_manager.hpp/cpp
│   ├── disk_manager.hpp/cpp
│   ├── lru_replacer.hpp
│   ├── page.hpp
│   ├── table_heap.hpp/cpp      # Slotted-page tuple management
│   ├── rid.hpp                 # Record ID abstraction
│   ├── tuple.hpp               # Typed tuple storage
│   ├── schema.hpp              # Column types (extendable)
│   ├── bplus_tree.hpp/cpp      # std::map-backed B+ Tree index
```

---

##  Currently Supported Queries

* `INSERT <key> <value>`
  → Inserts a tuple (with key as B+Tree index)
* `SELECT <key>`
  → Retrieves the value corresponding to the key

> ✔️ Output is printed directly to the console.

---

##  Example Usage

```sh
$ ./mydb
Enter queries (INSERT <key> <value> or SELECT <key>):
INSERT 1 hello
Inserted
SELECT 1
hello
```

---

##  Core Concepts Used

###  Modern C++ Highlights

* **Smart pointers** (internally encouraged for expansion)
* **RAII-based unpinning** (manual but structured)
* **Use of `std::optional`, `std::unordered_map`, `std::vector`**
* **Proper const correctness** and encapsulation
* **Custom memory layout for tuple storage using `std::byte`**
* **Modular abstraction** between components (DiskManager vs. BufferManager vs. TableHeap)

---

##  Design Philosophy

| Component           | Description                                        |
| ------------------- | -------------------------------------------------- |
| `Page`              | Raw memory + metadata wrapper (4KB default)        |
| `DiskManager`       | Abstracts file I/O at page granularity             |
| `BufferPoolManager` | Caches pages, handles replacement + dirty tracking |
| `TableHeap`         | Heap file using slotted-page layout                |
| `RID`               | (Page ID, Slot ID) reference                       |
| `Tuple`             | Encapsulates a value stored as raw bytes           |
| `QueryExecutor`     | Ties parser, index, and heap together              |
| `BPlusTree`         | Fast key lookup (via `std::map`)                   |
| `QueryParser`       | Simple parser for SQL-like input                   |

---


##  Tested Components

* ✅ Tuple insertion & retrieval
* ✅ Page unpinning and reuse
* ✅ LRU replacement on full buffer pool
* ✅ Simple SQL interface
* ✅ B+Tree indexing + RID mapping

---

## 🧑‍💻 How to Build

```bash
mkdir build && cd build
cmake ..
make
./mydb
```

> ⚠️ Requires C++17 or later.

---


