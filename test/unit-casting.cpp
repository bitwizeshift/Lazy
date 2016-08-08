/**
 * \file unit-casting.cpp
 *
 * \brief Catch unit tests for all Lazy casting operators
 *
 * \author Matthew Rodusek (matthew.rodusek@gmail.com)
 */
#include "catch.hpp"
#include <lazy/Lazy.hpp>

#include <tuple>
#include <string>

TEST_CASE("casting")
{

  SECTION("operator bool()")
  {
    SECTION("returns false for uninitialized lazy")
    {
      auto lazy_string = lazy::Lazy<std::string>("hello world");
      auto is_initialized = static_cast<bool>(lazy_string);

      REQUIRE_FALSE(is_initialized);
    }

    SECTION("returns true for initialized lazy")
    {
      auto lazy_string = lazy::Lazy<std::string>("hello world");
      *lazy_string;

      auto is_initialized = static_cast<bool>(lazy_string);

      REQUIRE(is_initialized);
    }
  }


  SECTION("operator reference()")
  {
    SECTION("lazy initialized uninitialized lazy")
    { // That's a mouthful
      auto lazy_string           = lazy::Lazy<std::string>("hello world");
      auto is_initialized_before = lazy_string.is_initialized();

      std::string& str = static_cast<std::string&>(lazy_string);

      auto is_initialized_after = lazy_string.is_initialized();

      REQUIRE(is_initialized_before != is_initialized_after);
    }

    SECTION("lazy is convertible to reference of type")
    {
      auto lazy_string           = lazy::Lazy<std::string>("hello world");

      std::string& str = static_cast<std::string&>(lazy_string);

      REQUIRE(str=="hello world");
    }
  }
}
