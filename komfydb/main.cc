#include <iostream>

#include "absl/strings/str_join.h"

#include "hello_greeter.h"

int main() {
  std::vector<std::string> v = {"foo", "bar", "baz"};
  std::string s = absl::StrJoin(v, "-");

  std::cout << "Joined string: " << s << "\n";
  std::cout << get_greeter("Franek") << "\n";

  return 0;
}
