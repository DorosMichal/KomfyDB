#ifndef __RECORD_H__
#define __RECORD_H__

#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/common/tuple.h"
#include "komfydb/common/type.h"
#include "komfydb/storage/record_id.h"

namespace {

using komfydb::common::IntField;
using komfydb::common::StringField;
using komfydb::common::Tuple;
using komfydb::common::TupleDesc;

};  // namespace

namespace komfydb::storage {

class Record : public Tuple {
 private:
  RecordId rid;

  void swap(Record& r);

 public:
  Record(const Record& r);

  Record(const Tuple& t, RecordId rid);

  Record(Record&& r) = default;

  Record& operator=(const Record& r);

  Record(const TupleDesc* tuple_desc, PageId pid, int tuple_no)
      : Tuple(tuple_desc), rid(pid, tuple_no) {}

  RecordId GetId();

  void SetId(PageId pid, int tuple_no);

  void SetId(RecordId rid);

  bool operator==(const Record& r) const;

  operator std::string() const;
};

};  // namespace komfydb::storage

#endif  // __RECORD_H__
