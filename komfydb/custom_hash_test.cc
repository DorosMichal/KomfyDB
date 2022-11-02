#include "gtest/gtest.h"

#include "absl/hash/hash_testing.h"

#include "komfydb/storage/page_id.h"

namespace {

using komfydb::storage::PageId;

TEST(PageId, Hash) {
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
      PageId(1, 2),
      PageId(2, 3),
      PageId(0, 0),
      PageId(2, 4),
  }));
}

};  // namespace
