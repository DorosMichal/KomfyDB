#ifndef __AGGREGATOR_H__
#define __AGGREGATOR_H__

#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "komfydb/common/type.h"

namespace {

using komfydb::common::Type;

};

namespace komfydb::execution {

class Aggregator {
 public:
  static const inline std::vector<int> NO_GROUPING = {};
  enum AggregateType {
    NONE,  // XXX field from group by clause with no aggregate function
    MAX,
    MIN,
    SUM,
    AVG,
    COUNT,
  };

  static absl::StatusOr<AggregateType> GetAggregateType(std::string_view fun) {
    absl::flat_hash_map<std::string, AggregateType> str_to_agg = {
        {"max", MAX}, {"min", MIN},     {"sum", SUM},
        {"avg", AVG}, {"count", COUNT}, {"none", NONE},
    };
    std::string lower_fun = std::string(fun);
    for (char& c : lower_fun) {
      c = std::tolower(c);
    }
    auto it = str_to_agg.find(lower_fun);
    if (it == str_to_agg.end()) {
      return absl::InvalidArgumentError(
          absl::StrCat("Unknown aggregate function ", fun));
    }
    return it->second;
  }

  static std::string AggregateTypeToString(AggregateType type) {
    switch (type) {
      case NONE:
        return "none";
      case MAX:
        return "max";
      case MIN:
        return "min";
      case SUM:
        return "sum";
      case AVG:
        return "avg";
      case COUNT:
        return "count";
    }
  }

  static bool IsApplicable(AggregateType type, Type field_type) {
    switch (field_type.GetValue()) {
      case Type::INT: {
        return true;
      }
      case Type::STRING: {
        switch (type) {
          case AVG:
          case SUM:
            return false;
          default:
            return true;
        }
      }
    }
  }
};

};  // namespace komfydb::execution

#endif  // __AGGREGATE_H__
