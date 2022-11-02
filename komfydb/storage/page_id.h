#ifndef __PAGE_ID_H__
#define __PAGE_ID_H__

#include <stdint.h>

#include <vector>

namespace komfydb::storage {

class PageId {
 private:
  int table_id;
  int page_no;

 public:
  PageId(int table_id, int page_no);

  std::vector<uint8_t> Serialize();

  int GetTableId() const;

  int GetPageNumber() const;

  // TODO(HashCode)
  // int HashCode();

  bool operator==(const PageId& p) const;
};

};  // namespace komfydb::storage

#endif  // __PAGE_ID_H__
