#include "komfydb/storage/heap_file.h"

#include <fstream>
#include <iostream>
#include <memory>

#include "komfydb/common/tuple_desc.h"
#include "komfydb/config.h"
#include "komfydb/storage/heap_page.h"
#include "komfydb/utils/status_macros.h"
#include "komfydb/utils/utility.h"

namespace komfydb::storage {

uint32_t HeapFile::table_cnt = 0;

HeapFile::HeapFile(std::fstream file, size_t file_length, TupleDesc tuple_desc,
                   uint32_t table_id, Permissions permissions)
    : file(std::move(file)),
      tuple_desc(tuple_desc),
      permissions(permissions),
      file_length(file_length),
      table_id(table_id) {}

HeapFile::~HeapFile() {
  file.close();
}

absl::StatusOr<std::unique_ptr<HeapFile>> HeapFile::Create(
    const absl::string_view file_path, TupleDesc tuple_desc,
    Permissions permissions) {
  std::ios_base::openmode mode = std::ios::binary | std::ios::in;
  if (permissions == Permissions::READ_WRITE) {
    mode |= std::ios::out;
  }

  std::fstream file;
  file.open(std::string(file_path), mode);
  if (!file.good()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Could not open db file: ", file_path));
  }

  file.seekg(0, file.end);
  uint32_t file_length = file.tellg();
  if (file_length % CONFIG_PAGE_SIZE) {
    file.close();
    return absl::InvalidArgumentError(
        absl::StrCat("File size ", std::to_string(file_length),
                     " not divisble by page size."));
  }

  return std::unique_ptr<HeapFile>(new HeapFile(
      std::move(file), file_length, tuple_desc, ++table_cnt, permissions));
}

std::fstream* HeapFile::GetFile() {
  return &file;
}

uint32_t HeapFile::GetId() {
  return table_id;
}

TupleDesc* HeapFile::GetTupleDesc() {
  return &tuple_desc;
}

absl::StatusOr<std::unique_ptr<Page>> HeapFile::ReadPage(PageId id) {
  if (id.GetTableId() != table_id) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Table ID does not match: ", table_id, "!=", id.GetTableId()));
  }

  uint64_t page_pos = (uint64_t)CONFIG_PAGE_SIZE * (uint64_t)id.GetPageNumber();
  if (page_pos >= file_length) {
    return absl::OutOfRangeError(
        absl::StrCat("Page number out of range: ", id.GetPageNumber(), " (",
                     file_length / CONFIG_PAGE_SIZE, ")"));
  }

  std::vector<uint8_t> data(CONFIG_PAGE_SIZE);
  file.seekg(page_pos);
  file.read((char*)data.data(), CONFIG_PAGE_SIZE);

  ASSIGN_OR_RETURN(std::unique_ptr<Page> page,
                   HeapPage::Create(id, &tuple_desc, data));

  return page;
}

int HeapFile::PageCount() {
  return file_length / CONFIG_PAGE_SIZE;
}

};  // namespace komfydb::storage
