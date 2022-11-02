#include <vector>

#include "komfydb/utils/utility.h"

namespace komfydb::common {

std::string GenerateUuidV4() {
  static absl::BitGen bitgen;

  std::stringstream ss;
  int i;
  ss << std::hex;

  for (i = 0; i < 8; i++) {
    ss << absl::Uniform(absl::IntervalClosed, bitgen, 0, 15);
  }
  ss << "-";
  for (i = 0; i < 4; i++) {
    ss << absl::Uniform(absl::IntervalClosed, bitgen, 0, 15);
  }
  ss << "-4";
  for (i = 0; i < 3; i++) {
    ss << absl::Uniform(absl::IntervalClosed, bitgen, 0, 15);
  }
  ss << "-";
  ss << absl::Uniform(absl::IntervalClosed, bitgen, 8, 11);
  for (i = 0; i < 3; i++) {
    ss << absl::Uniform(absl::IntervalClosed, bitgen, 0, 15);
  }
  ss << "-";
  for (i = 0; i < 12; i++) {
    ss << absl::Uniform(absl::IntervalClosed, bitgen, 0, 15);
  }

  return ss.str();
}

};  // namespace komfydb::common
