#ifndef __RECORD_ID_H__
#define __RECORD_ID_H__


#include "komfydb/storage/page_id.h"


namespace komfydb::storage {

class RecordId {
 private:
  int tuple_no;
 public:
  RecordId(PageId pid, int tuple_no);

  int GetTupleNumber();

  PageId GetPageId();

  bool operator == (const RecordId& rid) const;

  int HashCode(); 
};

};


#endif  // __RECORD_ID_H__
