#include <stdint.h>

#include <vector>

#include "heap_page_id.h"

namespace komfydb::storage {

HeapPageId::HeapPageId(int table_id, int page_no)
    : table_id(table_id), page_no(page_no) {}

std::vector<uint8_t> HeapPageId::Serialize() {
  std::vector<uint8_t> data(sizeof(table_id) + sizeof(page_no));
  ((int*)&data)[0] = table_id;
  ((int*)&data)[1] = page_no;
  return data;
}

int HeapPageId::GetTableId() const {
  return table_id;
}

int HeapPageId::GetPageNumber() const {
  return page_no;
}

bool HeapPageId::operator==(const PageId& p) const {
  return table_id == p.GetTableId() && page_no == p.GetPageNumber();
}

};  // namespace komfydb::storage
