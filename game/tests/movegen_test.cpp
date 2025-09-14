#include <catch2/catch_all.hpp>
#include <vector>

TEST_CASE("Vector operations") {
  std::vector<int> v = {1, 2, 3};
  REQUIRE(v.size() == 3);
  REQUIRE(v[0] == 1);
  REQUIRE(v[1] == 2);
  REQUIRE(v[2] == 3);
}

TEST_CASE("Loop test") {
  int sum = 0;
  for (int i = 1; i <= 5; ++i) {
    sum += i;
  }
  REQUIRE(sum == 15);
}
