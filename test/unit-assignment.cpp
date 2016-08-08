/**
 * \file unit-assignment.cpp
 *
 * \brief Catch unit tests for all Lazy assignment operators
 *
 * \author Matthew Rodusek (matthew.rodusek@gmail.com)
 */
#include "catch.hpp"
#include <lazy/Lazy.hpp>

#include <tuple>
#include <string>

TEST_CASE("assignment")
{
  // copy value assignment
  SECTION("operator=(const T&)")
  {
    SECTION("initializes an uninitialized lazy object")
    {
      auto lazy_string = lazy::Lazy<std::string>();
      auto string      = std::string("hello world");

      lazy_string = string;

      REQUIRE(lazy_string.is_initialized());
    }

    SECTION("assigns an initialized lazy object")
    {
      auto lazy_string = lazy::Lazy<std::string>();
      auto string      = std::string("hello world");
      *lazy_string;

      lazy_string = string;

      REQUIRE(lazy_string.is_initialized());
    }
  }


  // move value assignment
  SECTION("operator=(T&&)")
  {
    SECTION("initializes an uninitialized lazy object")
    {
      auto lazy_string = lazy::Lazy<std::string>();
      auto string      = std::string("hello world");

      lazy_string = std::move(string);

      REQUIRE(lazy_string.is_initialized());
    }

    SECTION("assigns an initialized lazy object")
    {
      auto lazy_string = lazy::Lazy<std::string>();
      auto string      = std::string("hello world");
      *lazy_string;

      lazy_string = std::move(string);

      REQUIRE(lazy_string.is_initialized());
    }
  }


  // copy assignment
  SECTION("operator=(const Lazy<T>&)")
  {
    SECTION("initializes an uninitialized lazy object")
    {
      auto lazy_string  = lazy::Lazy<std::string>();
      auto lazy_string2 = lazy::Lazy<std::string>("hello world");
      *lazy_string2;

      lazy_string = lazy_string2;

      REQUIRE(lazy_string.is_initialized());
    }

    SECTION("assigns an initialized lazy object")
    {
      auto lazy_string  = lazy::Lazy<std::string>();
      auto lazy_string2 = lazy::Lazy<std::string>("hello world");
      *lazy_string;
      *lazy_string2;

      lazy_string = lazy_string2;

      REQUIRE(lazy_string.is_initialized());
    }

    SECTION("assigns an uninitialized lazy object")
    {
      auto lazy_string  = lazy::Lazy<std::string>();
      auto lazy_string2 = lazy::Lazy<std::string>("hello world");

      lazy_string = lazy_string2;

      REQUIRE_FALSE(lazy_string.is_initialized());
    }
  }


  // move assignment
  SECTION("operator=(Lazy<T>&&)")
  {
    SECTION("initializes an uninitialized lazy object")
    {
      auto lazy_string  = lazy::Lazy<std::string>();
      auto lazy_string2 = lazy::Lazy<std::string>("hello world");
      *lazy_string2;

      lazy_string = std::move(lazy_string2);

      REQUIRE(lazy_string.is_initialized());
    }

    SECTION("assigns an initialized lazy object")
    {
      auto lazy_string  = lazy::Lazy<std::string>();
      auto lazy_string2 = lazy::Lazy<std::string>("hello world");
      *lazy_string;
      *lazy_string2;

      lazy_string = std::move(lazy_string2);

      REQUIRE(lazy_string.is_initialized());
    }

    SECTION("assigns an uninitialized lazy object")
    {
      auto lazy_string  = lazy::Lazy<std::string>();
      auto lazy_string2 = lazy::Lazy<std::string>("hello world");

      lazy_string = std::move(lazy_string2);

      REQUIRE_FALSE(lazy_string.is_initialized());
    }
  }
}
