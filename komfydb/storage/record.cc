#include "komfydb/storage/record.h"

namespace komfydb::storage {

RecordId Record::GetId() {
  return *this;
}

void Record::SetId(PageId pid, int tuple_no) {
  this->pid = pid;
  this->tuple_no = tuple_no;
}

void Record::SetId(RecordId rid) {
  SetId(rid.GetPageId(), rid.GetTupleNumber());
}

};  // namespace komfydb::storage
