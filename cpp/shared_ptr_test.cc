#define CATCH_CONFIG_MAIN
#include "catch.hpp"   

#include "shared_ptr.h"

TEST_CASE("shared_ptr") {
  SECTION("default constructor") {
    shared_ptr<int> sp;
    REQUIRE(sp.get() == nullptr);
  }

  SECTION("constructor") {
    shared_ptr<int> sp(new int(42));
    REQUIRE(*sp.get() == 42);
    REQUIRE(sp.use_count() == 1);
  }

  SECTION("copy constructor") {
    shared_ptr<int> sp1(new int(42));
    shared_ptr<int> sp2(sp1);
    REQUIRE(*sp2.get() == 42);
    REQUIRE(sp1.use_count() == 2);
    REQUIRE(sp1.get() == sp2.get());
  }

  SECTION("copy assignment") {
    shared_ptr<int> sp1(new int(42));
    shared_ptr<int> sp2;
    sp2 = sp1;
    REQUIRE(*sp2.get() == 42);
    REQUIRE(sp1.use_count() == 2);
    REQUIRE(sp1.get() == sp2.get());
  }

  SECTION("move constructor") {
    shared_ptr<int> sp1(new int(42));
    shared_ptr<int> sp2(std::move(sp1));
    REQUIRE(*sp2.get() == 42);
    REQUIRE(sp1.get() == nullptr);
    REQUIRE(sp1.use_count() == 0);
    REQUIRE(sp2.use_count() == 1);
  }

  SECTION("move assignment") {
    shared_ptr<int> sp1(new int(42));
    shared_ptr<int> sp2;
    sp2 = std::move(sp1);
    REQUIRE(*sp2.get() == 42);
    REQUIRE(sp1.get() == nullptr);
    REQUIRE(sp1.use_count() == 0);
    REQUIRE(sp2.use_count() == 1);
  }

  SECTION("deleter") {
    shared_ptr<int> sp(new int(42), [](void *ptr) { delete static_cast<int *>(ptr); });
    REQUIRE(*sp.get() == 42);

    sp.reset();
    sp = shared_ptr<int>(new int[10], [](void *ptr) { delete[] static_cast<int **>(ptr); });
    REQUIRE(sp.get() != nullptr);
  }

  SECTION("weak_ptr") {
    shared_ptr<int> sp(new int(42));
    weak_ptr<int> wp(sp);
    REQUIRE(*wp.lock().get() == 42);

    sp.reset();
    REQUIRE(wp.lock().get() == nullptr);
  }
}