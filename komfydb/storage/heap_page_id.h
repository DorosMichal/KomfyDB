#ifndef __HEAP_PAGE_ID_H__
#define __HEAP_PAGE_ID_H__

#include <stdint.h>

#include <vector>

#include "komfydb/storage/page_id.h"

namespace komfydb::storage {

class HeapPageId : PageId {
 private:
  int table_id;
  int page_no;

 public:
  HeapPageId(int table_id, int page_no);

  std::vector<uint8_t> Serialize() override;

  int GetTableId() const override;

  int GetPageNumber() const override;

  // TODO(HashCode)
  // int HashCode() override;

  bool operator==(const PageId& p) const override;
};

};  // namespace komfydb::storage

#endif  // __HEAP_PAGE_ID_H__
