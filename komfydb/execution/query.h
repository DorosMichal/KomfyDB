#ifndef __QUERY_H__
#define __QUERY_H__

#include <memory>
#include <string_view>

#include "komfydb/execution/op_iterator.h"

namespace komfydb::execution {

class Query {
 public:
  enum Type { ITERATOR, CREATE_TABLE };

  std::unique_ptr<execution::OpIterator> iterator;
  std::string table_name;
  TupleDesc tuple_desc;
  std::string primary_key;
  Type type;

  Query(std::unique_ptr<execution::OpIterator> iterator);

  Query(std::string_view table_name, TupleDesc tuple_desc,
        std::string primary_key);

  Query(std::string_view table_name, TupleDesc tuple_desc);
};

};  // namespace komfydb::execution

#endif  // __QUERY_H__
