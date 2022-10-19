#ifndef __HEAP_PAGE_ID_H__
#define __HEAP_PAGE_ID_H__

#include <vector>

#include "komfydb/storage/page_id.h"

namespace komfydb::storage {

class HeapPageId : PageId {
 private:
  // TODO how to represent this?
 public:
  HeapPageId(int table_id, int page_no);

  int GetPageNumer();

  std::vector<int> Serialize() override;

  int GetTableId() override;

  int HashCode() override;

  // TODO Should this be a overriden virtual function? I guess the argument
  // should be HeapPageId rather than PageId...
  bool operator==(const PageId& p) const override;

  int GetPageNumber() override;
};

};  // namespace komfydb::storage

#endif  // __HEAP_PAGE_ID_H__
