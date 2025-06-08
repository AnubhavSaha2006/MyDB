/************************  storage/schema.hpp  ************************/
#pragma once
#include <vector>
#include <string>
#include <cstring>      // std::memcpy
#include <cstddef>      // std::byte
#include <stdexcept>
#include "parser/query.hpp"   // brings ColumnType, ColumnDef
#include "storage/tuple.hpp"  // your Tuple class

/* ---------- Column alias (reuse ColumnDef from parser) ---------- */
using Column = ColumnDef;     // keeps older code working

class Schema {
public:
    explicit Schema(std::vector<Column> cols) : cols_(std::move(cols)) {}

    const std::vector<Column>& columns() const { return cols_; }

    /* -------- Serialize raw literals -> Tuple bytes -------- */
    Tuple serialize(const std::vector<std::string>& raw) const {
        if (raw.size() != cols_.size())
            throw std::runtime_error("value/column count mismatch");

        std::vector<std::byte> buf;
        for (std::size_t i = 0; i < cols_.size(); ++i) {
            const auto& col = cols_[i];

            if (col.type == ColumnType::INT) {
                int v = std::stoi(raw[i]);
                const auto* p = reinterpret_cast<const std::byte*>(&v);
                buf.insert(buf.end(), p, p + sizeof(int));
            }
            else {                                // CHAR(n)
                std::string s = raw[i];
                if (s.size() > col.len) s.resize(col.len);
                while (s.size() < col.len) s.push_back('\0');   // pad
                const auto* p = reinterpret_cast<const std::byte*>(s.data());
                buf.insert(buf.end(), p, p + col.len);
            }
        }
        return Tuple(buf.data(), buf.size());
    }

    /* -------- Deserialize Tuple -> vector<string> -------- */
    std::vector<std::string> deserialize(const Tuple& tup) const {
        std::vector<std::string> out;
        const std::byte* p = tup.data();

        for (const auto& col : cols_) {
            if (col.type == ColumnType::INT) {
                int v;
                std::memcpy(&v, p, sizeof(int));
                out.push_back(std::to_string(v));
                p += sizeof(int);
            }
            else {                                // CHAR(n)
                std::string s(reinterpret_cast<const char*>(p), col.len);
                while (!s.empty() && s.back() == '\0') s.pop_back(); // trim pad
                out.push_back(s);
                p += col.len;
            }
        }
        return out;
    }

private:
    std::vector<Column> cols_;
};
