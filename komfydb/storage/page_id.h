#ifndef __PAGE_ID_H__
#define __PAGE_ID_H__

#include <stdint.h>

#include <vector>

#include "absl/container/flat_hash_map.h"

namespace komfydb::storage {

class PageId {
 private:
  unsigned int table_id;
  unsigned int page_no;

 public:
  PageId(int table_id, int page_no);

  std::vector<uint8_t> Serialize();

  unsigned int GetTableId() const;

  unsigned int GetPageNumber() const;

  bool operator==(const PageId& p) const;

  template <typename H>
  friend H AbslHashValue(H h, const PageId& pid) {
    return H::combine(std::move(h), pid.table_id, pid.page_no);
  }
};

};  // namespace komfydb::storage

#endif  // __PAGE_ID_H__
