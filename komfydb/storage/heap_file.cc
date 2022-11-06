#include "komfydb/storage/heap_file.h"

#include <fstream>

#include "komfydb/utils/utility.h"
#include "komfydb/config.h"
#include "komfydb/utils/status_macros.h"
#include "komfydb/common/tuple_desc.h"

namespace komfydb::storage {

HeapFile::HeapFile(
    std::fstream& file, 
    TupleDesc td, 
    unsigned int table_id,
    Permissions permissions) 
    : file(file), td(td), table_id(table_id), permissions(permissions) {}

HeapFile::~HeapFile() {
  file.close();
}

absl::StatusOr<std::unique_ptr<HeapFile>> HeapFile::Create(
    const std::string& file_path, 
    TupleDesc* td,
    Permissions permissions) {
  ios_base::openmode mode = std::io::binary | std::io::in;
  if (permissions == Permissions::READ_WRITE) {
    mode |= std::io::out;
  }
  
  std::fstream file; 
  file.open(file_path, mode);
  if (!file.good()) {
    return absl::InvalidArgumentError("Could not open specified db file.");
  }

  file.seekg(0, file.end);
  unsigned int file_length = file.tellg();
  if (file_length % CONFIG_PAGE_SIZE) {
    file.close();
    return absl::InvalidArgumentError("File size not divisble by page size.");
  }

  return std::make_unique<HeapFile>(file, td, ++table_cnt, permissions);
}

std::fstream& HeapFile::GetFile() {
  return file;
}

unsigned int HeapFile::GetId() {
  return table_id;
}

unsigned int HeapFile::GetTupleDesc() {
  return td;
}

absl::StatusOr<std::unique_ptr<Page>> ReadPage(PageId id) {
  if (id.table_id != table_id) {
    return absl::InvalidArgumentError("Table ID does not match.");
  }

  file.seekg(0, file.end);
  unsigned int file_length = file.tellg();

  int page_pos = CONFIG_PAGE_SIZE * id.page_no;
  if (page_pos >= file_length) {
    return absl::InvalidArgumentError("Page number out of range.");
  }

  vector<uint8_t> data(CONFIG_PAGE_SIZE);
  file.seekg(page_pos);
  file.get(&data, CONFIG_PAGE_SIZE);

  ASSIGN_OR_RETURN(std::unique_ptr<Page> page, HeapPage::Create(id, &td, data));

  return page;
}

};

