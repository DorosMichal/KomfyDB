#include "komfydb/storage/heap_file.h"

#include <errno.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include "glog/logging.h"

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
    const std::filesystem::path file_path, TupleDesc tuple_desc,
    Permissions permissions) {
  std::ios_base::openmode mode = std::ios::binary | std::ios::in;
  if (permissions == Permissions::READ_WRITE) {
    mode |= std::ios::out;
  }

  // This is a hack to open the file in append mode to create it if it didn't
  // exist. Opening with std::ios::app prevents to read or write to the already
  // existing part of the file.
  (void)std::ofstream(file_path, std::ios::app | std::ios::binary);
  std::fstream file;
  file.open(file_path, mode);
  if (!file.good()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Could not open db file: ", file_path.string(), ": ", strerror(errno)));
  }

  file.seekg(0, file.end);
  uint32_t file_length = file.tellg();
  if (file_length % CONFIG_PAGE_SIZE) {
    file.close();
    return absl::InvalidArgumentError(
        absl::StrCat("File size ", std::to_string(file_length),
                     " not divisble by page size."));
  }

  LOG(INFO) << "Opened " << file_path << ", size=" << file_length << "B";

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

absl::Status HeapFile::WritePage(Page* page) {
  PageId id = page->GetId();
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

  ASSIGN_OR_RETURN(std::vector<uint8_t> data, page->GetPageData());
  file.seekp(page_pos, std::ios_base::beg);
  file.write((char*)data.data(), CONFIG_PAGE_SIZE);
  return absl::OkStatus();
}

int HeapFile::PageCount() {
  return file_length / CONFIG_PAGE_SIZE;
}

absl::StatusOr<std::unique_ptr<Page>> HeapFile::CreatePage() {
  file_length += CONFIG_PAGE_SIZE;
  std::vector<uint8_t> empty_data(CONFIG_PAGE_SIZE, 0);
  return HeapPage::Create(PageId(table_id, PageCount() - 1), &tuple_desc,
                          empty_data);
}

};  // namespace komfydb::storage
