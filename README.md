
---


# MyDB — Lightweight In-Memory DBMS in Modern C++

*MyDB* is a small, pedagogical database engine written entirely in C++.  
It demonstrates fundamental database-system components—page layout, buffer management, tuple serialization, indexing, and a SQL-like front end—while applying modern C++17/20 facilities for safety and clarity.

---

## Contents

1. [Main Capabilities](#main-capabilities)  
2. [Supported SQL-Like Statements](#supported-sql-like-statements)  
3. [Project Structure](#project-structure)  
4. [Example Session](#example-session)  
5. [Building](#building)  
6. [Modern C++ Techniques Employed](#modern-c-techniques-employed)  

---

## Main Capabilities

| Layer | Implementation Highlights |
|-------|--------------------------|
| **Storage Engine** | 4 KB `Page` abstraction; `DiskManager` for page-level I/O |
| **Buffer Pool** | `BufferPoolManager` with **LRU replacement** and pin/unpin protocol |
| **Heap File** | `TableHeap` uses a classic **slotted-page** layout for variable-length tuples |
| **Tuple & RID** | `Tuple` stores raw bytes; `RID` identifies a record by `(page_id, slot_id)` |
| **Index** | Header-only, templated **B+ Tree** (custom nodes, split/merge, leaf chaining) |
| **Schema** | Variable column list, currently `INT` and fixed-length `CHAR(n)` |
| **Catalog** | Manages multiple tables, each with its own schema, heap, and index |
| **SQL-like Layer** | Hand-written **parser** and **executor** supporting `CREATE`, `INSERT`, `SELECT`, `DELETE` |
| **CLI** | Interactive REPL in `main.cpp` prints results or error messages immediately |

---

## Supported SQL-Like Statements

```sql
-- define a table (first column must be INT, becomes the primary key)
CREATE TABLE t1 (roll INT, name CHAR(20), address CHAR(40));

-- insert a row (INT literals are bare, CHAR literals are single-quoted)
INSERT INTO t1 VALUES (113, 'alice', 'NYC');

-- full scan
SELECT * FROM t1;

-- point lookup on primary key
SELECT * FROM t1 WHERE roll = 113;

-- logical delete (removes row from index and marks slot free)
DELETE FROM t1 WHERE roll = 113;
````

> Only `INT` and fixed-length `CHAR(n)` are supported.
> The first `INT` column is always used as the B+-tree key.

---

## Project Structure

```repl
src/
├── main.cpp                         # REPL entry point
│
├── parser/
│   ├── query.hpp                    # AST structs (CreateTable, Insert, …)
│   ├── query_parser.hpp/.cpp        # Regex-based SQL parser
│
├── execution/
│   ├── query_executor.hpp/.cpp      # Executes AST on catalog / storage
│
└── storage/
    ├── page.hpp                     # 4 KB page with header
    ├── disk_manager.hpp/.cpp        # Reads/writes pages on disk
    ├── lru_replacer.hpp             # LRU page replacer
    ├── buffer_pool_manager.hpp/.cpp # Frame cache with pinning
    ├── rid.hpp                      # Record identifier
    ├── tuple.hpp                    # Raw-byte tuple
    ├── schema.hpp                   # Column metadata + (de)serialisation
    ├── table_heap.hpp/.cpp          # Slotted-page heap file
    ├── bplus_tree.hpp               # Header-only custom B+ tree
    └── catalog.hpp                  # Table metadata registry
```


---

## Example Session

```bash
Mini-SQL> CREATE TABLE t1 (roll INT, name CHAR(20), address CHAR(40));
Table created

Mini-SQL> INSERT INTO t1 VALUES (123,'abc','abcd');
Mini-SQL> INSERT INTO t1 VALUES (113,'abcDE','abcd');

Mini-SQL> SELECT * FROM t1;
123 abc abcd
113 abcDE abcd

Mini-SQL> DELETE FROM t1 WHERE roll = 123;
Deleted

Mini-SQL> SELECT * FROM t1;
113 abcDE abcd

#Multiple tables work independently:

Mini-SQL> CREATE TABLE t2 (roll INT, name CHAR(20), value INT, loc CHAR(10));
Mini-SQL> INSERT INTO t2 VALUES (555,'x',999,'zzz');
Mini-SQL> SELECT * FROM t2;
555 x 999 zzz
```

---

## Building

```bash
mkdir build && cd build
cmake ..            # requires CMake 3.17+
cmake --build .     # any C++17/20 compiler (GCC ≥ 8, Clang ≥ 7, MSVC ≥ 19.29)
./mydb
```

---

## Modern C++ Techniques Employed

| Feature / Library                                   | Where Used                                              |
| --------------------------------------------------- | ------------------------------------------------------- |
| **`std::unique_ptr`, `std::shared_ptr`**            | Ownership of heap files, B+-tree nodes, catalog objects |
| **`std::optional`**                                 | B+-tree search results, parser returns                  |
| **`std::variant` + `std::visit`**                   | Type-safe AST dispatch in `QueryExecutor`               |
| **`std::byte`**                                     | Low-level tuple serialization and page buffers          |
| **`std::regex`**                                    | Lightweight SQL-like parsing                            |
| **RAII**                                            | Buffer-page pin/unpin, automatic flushing               |
| **Header-only Templates**                           | Generic B+-tree (`BPlusTree<ValueT>`)                   |
| **Const-correctness & `noexcept` (where relevant)** | Safer API contracts                                     |

---

### Next Steps (road-map)

* Support for `UPDATE` and `CHAR/VARCHAR` resizing
* Recovery (write-ahead logging)
* Multi-page heaps and index iterator scans
* Query optimizer and expression evaluation

---


