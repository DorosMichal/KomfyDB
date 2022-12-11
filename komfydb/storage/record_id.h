#ifndef __RECORD_ID_H__
#define __RECORD_ID_H__

#include "komfydb/storage/page_id.h"

namespace komfydb::storage {

class RecordId {
 protected:
  PageId pid;

  int tuple_no;

 public:
  RecordId(PageId pid, int tuple_no);

  int GetTupleNumber() const;

  PageId GetPageId() const;

  bool operator==(const RecordId& rid) const;

  operator std::string() const;

  // TODO(HashCode)
  // int HashCode();
};

};  // namespace komfydb::storage

#endif  // __RECORD_ID_H__
