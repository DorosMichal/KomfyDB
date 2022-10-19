#ifndef __HEAP_PAGE_FACTORY_H__
#define __HEAP_PAGE_FACTORY_H__

#include <stdint.h>

#include <vector>

#include "absl/status/statusor.h"

#include "komfydb/storage/heap_page.h"
#include "komfydb/storage/heap_page_id.h"

namespace komfydb::storage {

class HeapPageFactory {
 public:
  HeapPageFactory();

  absl::StatusOr<std::unique_ptr<HeapPage>> Create(HeapPageId id,
                                                   std::vector<uint8_t> data);
};

};  // namespace komfydb::storage

#endif  // __HEAP_PAGE_FACTORY_H__
