#ifndef __QUERY_H__
#define __QUERY_H__

#include <memory>
#include <string_view>

#include "komfydb/execution/op_iterator.h"

namespace komfydb::execution {

class Query {
 public:
  enum Type {
    ITERATOR,
    CREATE_TABLE,
    SHOW_TABLES,
    SHOW_COLUMNS,
  };

  std::unique_ptr<execution::OpIterator> iterator;
  std::string table_name;
  TupleDesc tuple_desc;
  std::string primary_key;
  Type type;

  Query(std::unique_ptr<execution::OpIterator> iterator);

  Query(std::string_view table_name, TupleDesc tuple_desc,
        std::string_view primary_key);

  Query();

  Query(std::string_view table_name);
};

};  // namespace komfydb::execution

#endif  // __QUERY_H__
