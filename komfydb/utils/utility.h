#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <sstream>
#include <string>
#include <vector>

#include "absl/random/random.h"
#include "absl/status/statusor.h"

#include "komfydb/common/column_ref.h"

namespace komfydb::common {

template <typename T>
void JoinVectors(const std::vector<T>& v1, const std::vector<T>& v2,
                 std::vector<T>& res) {
  res.clear();
  res.reserve(v1.size() + v2.size());
  res.insert(res.end(), v1.begin(), v1.end());
  res.insert(res.end(), v2.begin(), v2.end());
}

std::string GenerateUuidV4();

};  // namespace komfydb::common

#endif  // __UTILITY_H__
