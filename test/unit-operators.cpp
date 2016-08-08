/**
 * \file unit-operators.cpp
 *
 * \brief Catch unit tests for all Lazy operators
 *
 * \author Matthew Rodusek (matthew.rodusek@gmail.com)
 */
#include "catch.hpp"
#include <Lazy.hpp>

#include <tuple>
#include <string>

TEST_CASE("operators")
{
  SECTION("Lazy<T>::swap(Lazy<T>&)")
  {
    SECTION("Swaps elements")
    {
      lazy::Lazy<std::string> lazy_string1("Hello world");
      lazy::Lazy<std::string> lazy_string2("Goodbye world");

      auto lazy_string1_before = *lazy_string1;
      auto lazy_string2_before  = *lazy_string2;

      lazy_string1.swap(lazy_string2);

      auto is_swapped = lazy_string1_before == (*lazy_string2) &&
                        lazy_string2_before == (*lazy_string1);

      REQUIRE(is_swapped);
    }

    SECTION("is callable from unqualified swap")
    {
      lazy::Lazy<std::string> lazy_string1("Hello world");
      lazy::Lazy<std::string> lazy_string2("Goodbye world");

      swap(lazy_string1,lazy_string2);

      auto is_swapped = static_cast<std::string&>(lazy_string1) == "Goodbye world" &&
                        static_cast<std::string&>(lazy_string2) == "Hello world";

      REQUIRE(is_swapped);
    }
  }


  SECTION("Lazy<T>::is_initialized()")
  {
    SECTION("Is true for initialized lazy")
    {
      lazy::Lazy<std::string> lazy_string("Hello world");

      REQUIRE_FALSE(lazy_string.is_initialized());
    }

    SECTION("is false for uninitialized lazy")
    {
      lazy::Lazy<std::string> lazy_string("Hello world");
      *lazy_string;

      REQUIRE(lazy_string.is_initialized());
    }
  }


  SECTION("Lazy<T>::get()")
  {
    SECTION("lazy initializes entry")
    {
      lazy::Lazy<std::string> lazy_string("Hello world");

      auto is_initialized_before = lazy_string.is_initialized();
      lazy_string.get();
      auto is_initialized_after = lazy_string.is_initialized();

      REQUIRE(is_initialized_before != is_initialized_after);
    }

    SECTION("retrieves pointer to lazy instance")
    {
      lazy::Lazy<std::string> lazy_string("Hello world");

      auto str = lazy_string.get();

      REQUIRE(*str == "Hello world");
    }
  }


  SECTION("Lazy<T>::operator*()")
  {
    SECTION("lazy initializes entry")
    {
      lazy::Lazy<std::string> lazy_string("Hello world");

      auto is_initialized_before = lazy_string.is_initialized();
      *lazy_string;
      auto is_initialized_after = lazy_string.is_initialized();

      REQUIRE(is_initialized_before != is_initialized_after);
    }

    SECTION("dereferences lazy instance")
    {
      lazy::Lazy<std::string> lazy_string("Hello world");

      REQUIRE((*lazy_string) == "Hello world");
    }
  }


  SECTION("Lazy<T>::operator->()")
  {
    SECTION("lazy initializes entry")
    {
      lazy::Lazy<std::string> lazy_string("Hello world");

      auto is_initialized_before = lazy_string.is_initialized();
      lazy_string->empty(); // just call something
      auto is_initialized_after = lazy_string.is_initialized();

      REQUIRE(is_initialized_before != is_initialized_after);
    }

    SECTION("returns pointer to lazy instance")
    {
      lazy::Lazy<std::string> lazy_string("Hello world");

      auto str = lazy_string.operator->();

      REQUIRE(*str == "Hello world");
    }
  }
}
