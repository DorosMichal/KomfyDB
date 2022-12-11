#include "komfydb/storage/record_id.h"

namespace komfydb::storage {

RecordId::RecordId(PageId pid, int tuple_no) : pid(pid), tuple_no(tuple_no) {}

int RecordId::GetTupleNumber() const {
  return tuple_no;
}

PageId RecordId::GetPageId() const {
  return pid;
}

bool RecordId::operator==(const RecordId& rid) const {
  return tuple_no == rid.tuple_no && pid == rid.pid;
}

RecordId::operator std::string() const {
  return std::string(pid) + ", " + std::to_string(tuple_no);
}

};  // namespace komfydb::storage
