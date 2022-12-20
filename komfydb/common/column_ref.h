#ifndef __COLUMN_REF_H__
#define __COLUMN_REF_H__

#include <string>

namespace komfydb::common {

struct ColumnRef {
  // Table alias.
  std::string table;
  // Column name.
  std::string column;

  ColumnRef() : table(""), column("") {}

  ColumnRef(std::string_view table, std::string_view column)
      : table(table), column(column) {}

  bool IsValid() { return table != "" && column != ""; }

  bool IsStar() { return table == "null" && column == "*"; }

  operator std::string() const { return table + "." + column; }
};

const ColumnRef COLUMN_REF_STAR("null", "*");

};  // namespace komfydb::common

#endif  // __COLUMN_REF_H__
