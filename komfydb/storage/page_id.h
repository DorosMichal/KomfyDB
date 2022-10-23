#ifndef __PAGE_ID_H__
#define __PAGE_ID_H__

#include <vector>

namespace komfydb::storage {

class PageId {
 public:
  virtual std::vector<int> Serialize();

  virtual int GetTableId();

  virtual int HashCode();

  virtual bool operator==(const PageId& p) const;

  virtual int GetPageNumber();
};

};  // namespace komfydb::storage

#endif  // __PAGE_ID_H__
