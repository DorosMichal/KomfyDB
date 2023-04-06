#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include "absl/status/statusor.h"

#include "komfydb/execution/op_iterator.h"
#include "komfydb/utils/status_macros.h"

namespace komfydb::execution {

class Executor {
 public:
  absl::Status PrettyExecute(std::unique_ptr<OpIterator> iterator,
                             std::ostream& os = std::cout);

  absl::Status PrettyShowTables(std::vector<std::string> tables,
                                std::ostream& os = std::cout);

  absl::Status PrettyShowColumns(std::string_view table, TupleDesc* tuple_desc,
                                 std::ostream& os = std::cout);

  absl::Status PythonExecute(std::unique_ptr<OpIterator> iterator,
                             std::ostream& os = std::cout);

 private:
  void InitializePrettyPrinter(TupleDesc* iterator_tuple_desc);

  TupleDesc* tuple_desc;
  int terminal_width;
  int column_width;
};

};  // namespace komfydb::execution

#endif
