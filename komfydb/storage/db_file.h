#ifndef __DB_FILE_H__
#define __DB_FILE_H__

namespace komfydb::storage {

class DbFile {
 public:
  Page ReadPage(PageId id);
};

};  // namespace komfydb::storage

#endif  // __DB_FILE_H__
