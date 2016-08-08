/**
 * \file unit-constructor.cpp
 *
 * \brief Catch unit tests for all Lazy constructors
 *
 * \author Matthew Rodusek (matthew.rodusek@gmail.com)
 */
#include "catch.hpp"
#include <Lazy.hpp>

#include <tuple>
#include <string>

TEST_CASE("constructors")
{
  //--------------------------------------------------------------------------
  // T
  //--------------------------------------------------------------------------

  SECTION("Lazy<T>()")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto lazy_string = lazy::Lazy<std::string>();

      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }


  SECTION("Lazy<T>(Func,Func)")
  {
    SECTION("creates an uninitialized lazy object")
    {
//      auto create_string = [](){
//        return std::make_tuple("hello world",5);
//      };
//      auto destroy_string = [](std::string& str){
//        // do something
//      };
//      auto lazy_string = lazy::Lazy<std::string>(create_string,destroy_string);
//
//      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }


  SECTION("Lazy<T>(const T&)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto string      = std::string("hello world");
      auto lazy_string = lazy::Lazy<std::string>(string);

      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }


  SECTION("Lazy<T>(T&&)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto string      = std::string("hello world");
      auto lazy_string = lazy::Lazy<std::string>(std::move(string));

      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }


  SECTION("Lazy<T>(const Lazy<T>&)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto lazy_original = lazy::Lazy<std::string>("hello world");
      auto lazy_new      = lazy::Lazy<std::string>(lazy_original);

      REQUIRE_FALSE( lazy_new.is_initialized() );
    }

    SECTION("creates an initialized lazy object")
    {
      auto lazy_original = lazy::Lazy<std::string>("hello world");
      *lazy_original; // force-instantiate

      auto lazy_new = lazy::Lazy<std::string>(lazy_original);

      REQUIRE( lazy_new.is_initialized() );
    }
  }


  SECTION("Lazy<T>(Lazy<T>&&)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto lazy_original = lazy::Lazy<std::string>("hello world");
      auto lazy_new      = lazy::Lazy<std::string>(std::move(lazy_original));

      REQUIRE_FALSE( lazy_new.is_initialized() );
    }

    SECTION("creates an initialized lazy object")
    {
      auto lazy_original = lazy::Lazy<std::string>("hello world");
      *lazy_original; // force-instantiate

      auto lazy_new = lazy::Lazy<std::string>(std::move(lazy_original));

      REQUIRE( lazy_new.is_initialized() );
    }
  }


  SECTION("Lazy<T>(args...)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto lazy_string = lazy::make_lazy<std::string>("hello world",5);

      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }

  //--------------------------------------------------------------------------
  // Const T
  //--------------------------------------------------------------------------

  SECTION("Lazy<const T>()")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto lazy_string = lazy::Lazy<const std::string>();

      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }


  SECTION("Lazy<const T>(Func,Func)")
  {
    SECTION("creates an uninitialized lazy object")
    {
//      auto create_string = [](){
//        return std::make_tuple("hello world",5);
//      };
//      auto destroy_string = [](const std::string& str){
//        // do something
//      };
//      auto lazy_string = lazy::Lazy<const std::string>(create_string,destroy_string);
//
//      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }


  SECTION("Lazy<const T>(const T&)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto string      = std::string("hello world");
      auto lazy_string = lazy::Lazy<const std::string>(string);

      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }


  SECTION("Lazy<const T>(T&&)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto string      = std::string("hello world");
      auto lazy_string = lazy::Lazy<const std::string>(std::move(string));

      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }


  SECTION("Lazy<const T>(const Lazy<const T>&)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto lazy_original = lazy::Lazy<const std::string>("hello world");
      auto lazy_new      = lazy::Lazy<const std::string>(lazy_original);

      REQUIRE_FALSE( lazy_new.is_initialized() );
    }

    SECTION("creates an initialized lazy object")
    {
      auto lazy_original = lazy::Lazy<const std::string>("hello world");
      *lazy_original;
      auto lazy_new      = lazy::Lazy<const std::string>(lazy_original);

      REQUIRE( lazy_new.is_initialized() );
    }
  }


  SECTION("Lazy<const T>(Lazy<const T>&&)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto lazy_original = lazy::Lazy<const std::string>("hello world");
      auto lazy_new      = lazy::Lazy<const std::string>(std::move(lazy_original));

      REQUIRE_FALSE( lazy_new.is_initialized() );
    }
  }


  SECTION("Lazy<const T>(args...)")
  {
    SECTION("creates an uninitialized lazy object")
    {
      auto lazy_string = lazy::make_lazy<const std::string>("hello world",5);

      REQUIRE_FALSE( lazy_string.is_initialized() );
    }
  }
}
