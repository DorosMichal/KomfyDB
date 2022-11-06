#ifndef __PAGE_ID_H__
#define __PAGE_ID_H__

#include <stdint.h>

#include <vector>

#include "absl/container/flat_hash_map.h"

namespace komfydb::storage {

class PageId {
 private:
  int table_id;
  int page_no;

 public:
  PageId() = default;

  PageId(int table_id, int page_no);

  std::vector<uint8_t> Serialize();

  int GetTableId() const;

  int GetPageNumber() const;

  // TODO(HashCode)
  // int HashCode();

  bool operator==(const PageId& p) const;

  template <typename H>
  friend H AbslHashValue(H h, const PageId& pid) {
    return H::combine(std::move(h), pid.table_id, pid.page_no);
  }
};

};  // namespace komfydb::storage

#endif  // __PAGE_ID_H__
