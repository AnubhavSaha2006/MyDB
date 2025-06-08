#include "query_parser.hpp"
#include <regex>
#include <sstream>
#include <cctype>

static inline void trim(std::string& s) {
    while (!s.empty() && std::isspace(s.front())) s.erase(s.begin());
    while (!s.empty() && std::isspace(s.back()))  s.pop_back();
}

static ColumnType parse_type(const std::string& t, std::size_t& charLen) {
    if (t == "int" || t == "INT") return ColumnType::INT;
    std::smatch m;
    std::regex rg(R"(CHAR\s*\(\s*(\d+)\s*\))", std::regex::icase);
    if (std::regex_match(t, m, rg)) { charLen = std::stoul(m[1]); return ColumnType::CHAR; }
    throw std::runtime_error("bad type");
}

std::optional<Query> QueryParser::parse(const std::string& in) {
    std::string s = in; trim(s);

    /* CREATE TABLE */
    {
        std::regex rg(R"(CREATE\s+TABLE\s+(\w+)\s*\((.+)\)\s*;?)", std::regex::icase);
        std::smatch m;
        if (std::regex_match(s, m, rg)) {
            CreateTable q; q.table = m[1];
            std::string cols = m[2];
            std::stringstream ss(cols); std::string col;
            while (std::getline(ss, col, ',')) {
                trim(col);
                auto pos = col.find(' ');
                std::string name = col.substr(0, pos);
                std::string typ = col.substr(pos + 1);
                trim(typ);
                std::size_t clen = 0;
                ColumnType ct = parse_type(typ, clen);
                q.columns.push_back({ name,ct,clen });
            }
            return q;
        }
    }
    /* INSERT */
    {
        std::regex rg(R"(INSERT\s+INTO\s+(\w+)\s+VALUES\s*\((.+)\)\s*;?)", std::regex::icase);
        std::smatch m;
        if (std::regex_match(s, m, rg)) {
            Insert q; q.table = m[1];
            std::string vals = m[2];
            std::stringstream ss(vals); std::string tok;
            while (std::getline(ss, tok, ',')) {
                trim(tok);
                if (tok.front() == '\'') tok = tok.substr(1, tok.size() - 2);
                q.values.push_back(tok);
            }
            return q;
        }
    }
    /* SELECT * WHERE / SELECT * */
    {
        std::regex rg1(R"(SELECT\s+\*\s+FROM\s+(\w+)\s*;?)", std::regex::icase);
        std::regex rg2(R"(SELECT\s+\*\s+FROM\s+(\w+)\s+WHERE\s+(\w+)\s*=\s*('?[\w\d]+'?)\s*;?)", std::regex::icase);
        std::smatch m;
        if (std::regex_match(s, m, rg2)) {
            SelectWhere q{ m[1],m[2],m[3] };
            if (q.value.front() == '\'') q.value = q.value.substr(1, q.value.size() - 2);
            return q;
        }
        if (std::regex_match(s, m, rg1)) {
            return SelectAll{ m[1] };
        }
    }
    /* DELETE */
    {
        std::regex rg(R"(DELETE\s+FROM\s+(\w+)\s+WHERE\s+(\w+)\s*=\s*('?[\w\d]+'?)\s*;?)", std::regex::icase);
        std::smatch m;
        if (std::regex_match(s, m, rg)) {
            DeleteWhere q{ m[1],m[2],m[3] };
            if (q.value.front() == '\'') q.value = q.value.substr(1, q.value.size() - 2);
            return q;
        }
    }
    return std::nullopt;          // parse error
}
