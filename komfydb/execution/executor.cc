#include "komfydb/execution/executor.h"

#include <sys/ioctl.h>

#include <algorithm>
#include <vector>

#include "komfydb/common/td_item.h"
#include "komfydb/common/tuple_desc.h"

namespace {

using namespace komfydb::common;

void PrintLines(std::vector<std::string>& lines, std::ostream& os) {
  for (auto& line : lines) {
    os << line << std::endl;
  }
}

// WrapWord("abcabca", 3) -> { "abc", "abc", "a" }
std::vector<std::string> WrapWord(std::string word, int length) {
  std::vector<std::string> result;
  int parts = word.length() / length;
  for (int i = 0; i < parts; i++) {
    result.push_back(word.substr(i * length, length));
  }
  result.push_back(word.substr(parts * length));
  return result;
}

std::string GetLineBreak(int column_width, int count) {
  std::string result = "+";
  for (int i = 0; i < count; i++) {
    if (i > 0) {
      result += "+";
    }
    result += std::string(column_width, '-');
  }
  result += "+";
  return result;
}

std::vector<std::vector<std::string>> WrapColumns(
    std::vector<std::string>& columns, int column_width) {
  std::vector<std::vector<std::string>> result;
  for (auto& column : columns) {
    result.push_back(WrapWord(column, column_width - 2));
  }
  return result;
}

std::string ExtendToColumnWidth(std::string& content, int column_width) {
  int sz = column_width - content.size();
  return content + std::string(sz, ' ');
}

std::vector<std::string> ConcatenateWrappedColumns(
    std::vector<std::vector<std::string>>& wrapped_columns, int column_width) {
  std::vector<std::string> result;
  int row = 0;
  while (true) {
    std::string current = "|";
    bool unempty = false;

    for (int column = 0; column < wrapped_columns.size(); column++) {
      if (column > 0) {
        current += '|';
      }
      std::vector<std::string>& wrapped_column = wrapped_columns[column];
      current += ' ';
      std::string append = std::string(column_width - 1, ' ');
      if (wrapped_column.size() > row) {
        append = ExtendToColumnWidth(wrapped_column[row], column_width - 1);
        unempty = true;
      }
      current += append;
    }

    current += '|';
    if (unempty) {
      result.push_back(current);
      row++;
    } else {
      break;
    }
  }
  return result;
}

std::vector<std::string> GetFieldNames(TupleDesc* tuple_desc) {
  std::vector<std::string> result;
  std::vector<TDItem> items = tuple_desc->GetItems();
  for (auto& item : items) {
    result.push_back(std::string(item));
  }
  return result;
}

std::vector<std::string> GetLines(std::vector<std::string> contents,
                                  int column_width) {
  auto wrapped_contents = WrapColumns(contents, column_width);
  return ConcatenateWrappedColumns(wrapped_contents, column_width);
}

std::vector<std::string> GetTableHeader(TupleDesc* tuple_desc, int column_width,
                                        int count) {
  std::vector<std::string> result;
  std::string line_break = GetLineBreak(column_width, count);
  std::vector<std::string> field_names = GetFieldNames(tuple_desc);
  auto header_lines = GetLines(field_names, column_width);
  result.push_back(line_break);
  for (auto& line : header_lines) {
    result.push_back(line);
  }
  return result;
}

void PrintHeader(TupleDesc* tuple_desc, int column_width, std::ostream& os) {
  auto lines = GetTableHeader(tuple_desc, column_width, tuple_desc->Length());
  PrintLines(lines, os);
}

std::vector<std::string> GetTupleLines(Tuple* tuple, int length,
                                       int column_width) {
  std::vector<std::string> values;
  for (int i = 0; i < length; i++) {
    Field* field = *tuple->GetField(i);
    values.push_back(std::string(*field));
  }
  return GetLines(values, column_width);
}

};  // namespace

namespace komfydb::execution {

void Executor::InitializePrettyPrinter(TupleDesc* iterator_tuple_desc) {
  tuple_desc = iterator_tuple_desc;

  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  terminal_width = w.ws_col;
  // +-----+-----+
  // | ... | ... |
  //  <---> <--->
  column_width = (terminal_width - 1) / tuple_desc->Length() - 1;
}

absl::Status Executor::PrettyExecute(std::unique_ptr<OpIterator> iterator,
                                     uint64_t limit, std::ostream& os) {
  InitializePrettyPrinter(iterator->GetTupleDesc());
  PrintHeader(tuple_desc, column_width, os);
  int length = tuple_desc->Length();
  uint64_t cnt = 0;

  std::string line_break = GetLineBreak(column_width, length);
  RETURN_IF_ERROR(iterator->Open());
  ITERATE_RECORDS(iterator, record) {
    auto tuple_lines = GetTupleLines((*record).get(), length, column_width);
    os << line_break << std::endl;
    PrintLines(tuple_lines, os);
    if (limit && ++cnt >= limit) {
      break;
    }
  }
  os << line_break << std::endl;
  return absl::OkStatus();
}

};  // namespace komfydb::execution
