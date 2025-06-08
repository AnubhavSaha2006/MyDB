#pragma once
#include <string>
#include <optional>
#include "../storage/catalog.hpp"          // ? root-level catalog
#include "../parser/query.hpp"

class QueryExecutor {
public:
    explicit QueryExecutor(Catalog* cat) : cat_(cat) {}
    std::optional<std::string> execute(const Query& q);

private:
    Catalog* cat_;

    /* helpers */
    std::string exec_create(const CreateTable&);
    std::string exec_insert(const Insert&);
    std::string exec_select_all(const SelectAll&);
    std::string exec_select_where(const SelectWhere&);
    std::string exec_delete_where(const DeleteWhere&);
};
