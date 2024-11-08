#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "unique_ptr.h"

TEST_CASE("unique_ptr") {
  SECTION("constructor") {
    unique_ptr<int> up(new int(42));
    REQUIRE(*up.get() == 42);
  }

  SECTION("move constructor") {
    unique_ptr<int> up1(new int(42));
    unique_ptr<int> up2(std::move(up1));
    REQUIRE(*up2.get() == 42);
    REQUIRE(up1.get() == nullptr);
  }

  SECTION("move assignment") {
    unique_ptr<int> up1(new int(42));
    unique_ptr<int> up2;
    up2 = std::move(up1);
    REQUIRE(*up2.get() == 42);
    REQUIRE(up1.get() == nullptr);
  }

  SECTION("pointer manipulation") {
    unique_ptr<int> up(new int(42));
    up.reset();
    REQUIRE(up.get() == nullptr);

    up.reset(new int(42));
    REQUIRE(*up.get() == 42);
    auto *ptr = up.release();
    REQUIRE(ptr != nullptr);
    REQUIRE(up.get() == nullptr);
    delete ptr;
  }
}