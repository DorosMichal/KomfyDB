#include <stdint.h>

#include <vector>

#include "page_id.h"

namespace komfydb::storage {

PageId::PageId(uint32_t table_id, uint32_t page_no)
    : table_id(table_id), page_no(page_no) {}

std::vector<uint8_t> PageId::Serialize() {
  std::vector<uint8_t> data(sizeof(table_id) + sizeof(page_no));
  ((uint32_t*)&data)[0] = table_id;
  ((uint32_t*)&data)[1] = page_no;
  return data;
}

uint32_t PageId::GetTableId() const {
  return table_id;
}

uint32_t PageId::GetPageNumber() const {
  return page_no;
}

bool PageId::operator==(const PageId& p) const {
  return table_id == p.GetTableId() && page_no == p.GetPageNumber();
}

PageId::operator std::string() const {
  return std::to_string(table_id) + ", " + std::to_string(page_no);
}

};  // namespace komfydb::storage
