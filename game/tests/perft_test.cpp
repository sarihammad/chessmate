#include <catch2/catch_all.hpp>

TEST_CASE("Basic arithmetic test") {
  REQUIRE(1 + 1 == 2);
  REQUIRE(2 * 3 == 6);
}

TEST_CASE("String test") {
  std::string hello = "hello";
  REQUIRE(hello == "hello");
  REQUIRE(hello.length() == 5);
}
