#ifndef __DB_FILE_ITERATOR_H__
#define __DB_FILE_ITERATOR_H__


#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/storage/tuple.h"


namespace komfydb::storage {

class DbFileIterator {
 public: 
  virtual absl::Status Open();

  virtual void Close();

  virtual absl::StatusOr<bool> HasNext();
  
  virtual absl::StatusOr<Tuple> Next();
};

};


#endif  // __DB_FILE_ITERATOR_H__
