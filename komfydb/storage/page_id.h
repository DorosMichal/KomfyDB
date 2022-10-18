#ifndef __PAGE_ID_H__
#define __PAGE_ID_H__ 

#include <vector>

namespace komfydb::storage {

class PageId {
  virtual std::vector<int> Serialize();

  virtual int GetTableId();

  virtual int HashCode();

  virtual bool operator == (const PageId &p) const;

  virtual int GetPageNumber();
};

};


#endif  // __PAGE_ID_H__ 
