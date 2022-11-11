#ifndef __RECORD_H__
#define __RECORD_H__

#include "komfydb/common/tuple.h"
#include "komfydb/storage/record_id.h"

namespace {

using komfydb::common::Tuple;
using komfydb::common::TupleDesc;

};  // namespace

namespace komfydb::storage {

// TODO Actually I have no idea how double inheritance is implemented in C++,
// so we need to investigate it. However I feel like this is exactly what
// we want to do here: Tuple is a representation of a tuple (duh)
// and shouldn't have any code regarding I/O operations (i.e. storage)
// That's why we need Record, which is a tuple residing on the disk and have
// it's own RecordId.
class Record : public Tuple {
 private:
  RecordId rid;

 public:
  Record(const Record& r);

  Record(Record&& r) = default;

  Record(const TupleDesc* td, PageId pid, int tuple_no)
      : Tuple(td), rid(pid, tuple_no) {}

  RecordId GetId();

  void SetId(PageId pid, int tuple_no);

  void SetId(RecordId rid);

  bool operator==(const Record& r) const;
};

};  // namespace komfydb::storage

#endif  // __RECORD_H__
