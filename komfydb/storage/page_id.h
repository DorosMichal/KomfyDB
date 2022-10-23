#ifndef __PAGE_ID_H__
#define __PAGE_ID_H__

#include <stdint.h>

#include <vector>

namespace komfydb::storage {

class PageId {
 public:
  virtual std::vector<uint8_t> Serialize();

  virtual int GetTableId() const;

  virtual int GetPageNumber() const;

  // TODO(HashCode)
  // virtual int HashCode();

  virtual bool operator==(const PageId& p) const;
};

};  // namespace komfydb::storage

#endif  // __PAGE_ID_H__
