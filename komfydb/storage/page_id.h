#ifndef __PAGE_ID_H__
#define __PAGE_ID_H__

#include <stdint.h>

#include <vector>

#include "absl/container/flat_hash_map.h"

namespace komfydb::storage {

class PageId {
 private:
  uint32_t table_id;
  uint32_t page_no;

 public:
  PageId() = default;

  PageId(uint32_t table_id, uint32_t page_no);

  std::vector<uint8_t> Serialize();

  uint32_t GetTableId() const;

  uint32_t GetPageNumber() const;

  bool operator==(const PageId& p) const;

  operator std::string() const;

  template <typename H>
  friend H AbslHashValue(H h, const PageId& pid) {
    return H::combine(std::move(h), pid.table_id, pid.page_no);
  }
};

};  // namespace komfydb::storage

#endif  // __PAGE_ID_H__
