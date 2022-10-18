#include <iostream>

#include "absl/strings/string_view.h"
#include "absl/strings/str_cat.h"

std::string get_greeter(absl::string_view name) {
  return absl::StrCat("Hello, ", name, "!");
}
