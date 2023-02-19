#include "komfydb/execution/query.h"

namespace komfydb::execution {

Query::Query(std::unique_ptr<execution::OpIterator> iterator)
    : iterator(std::move(iterator)), type(ITERATOR) {}

Query::Query(std::string_view table_name, TupleDesc tuple_desc,
             std::string primary_key)
    : table_name(table_name),
      tuple_desc(tuple_desc),
      primary_key(primary_key),
      type(CREATE_TABLE) {}

Query::Query(std::string_view table_name, TupleDesc tuple_desc)
    : Query(table_name, tuple_desc, 0) {}

};  // namespace komfydb::execution
