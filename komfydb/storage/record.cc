#include "komfydb/storage/record.h"

namespace komfydb::storage {

void Record::swap(Record& r) {
  Tuple::swap(r);
  rid = r.rid;
}

Record& Record::operator=(const Record& r) {
  Record tmp(r);
  swap(tmp);
  return *this;
}

Record::Record(const Record& r) : Tuple(r), rid(r.rid) {}

RecordId Record::GetId() {
  return rid;
}

void Record::SetId(PageId pid, int tuple_no) {
  rid = RecordId(pid, tuple_no);
}

void Record::SetId(RecordId rid) {
  SetId(rid.GetPageId(), rid.GetTupleNumber());
}

bool Record::operator==(const Record& r) const {
  return Tuple::operator==(r) && r.rid == rid;
}

Record::operator std::string() const {
  return Tuple::operator std::string();
}

};  // namespace komfydb::storage
