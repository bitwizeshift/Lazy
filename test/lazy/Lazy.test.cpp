/**
 * \file Lazy.test.cpp
 *
 * \brief Test cases for Lazy::Lazy
 *
 * \author Matthew Rodusek (matthew.rodusek@gmail.com)
 */

#include <lazy/Lazy.hpp>

#include <catch.hpp>
#include <string>

//----------------------------------------------------------------------------
// Constructor / Destructor
//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy()", "[ctor]")
{
  lazy::Lazy<std::string> Lazy;

  SECTION("Default constructs a Lazy")
  {
    SECTION("Is uninitialized")
    {
      REQUIRE_FALSE( Lazy.has_value() );
    }

    SECTION("Default constructs on initialization")
    {
      REQUIRE( Lazy.value() == std::string() );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy( const Lazy& )", "[ctor]")
{
  lazy::Lazy<std::string> Lazy( lazy::in_place, "Hello World" );

  SECTION("Copy constructs an uninitialized Lazy")
  {
    auto copy = Lazy;

    SECTION("Is uninitialized")
    {
      REQUIRE_FALSE( copy.has_value() );
    }

    SECTION("Copy constructs on initialization")
    {
      REQUIRE( copy.value() == "Hello World" );
    }
  }

  SECTION("Copy constructs an initialized Lazy")
  {
    Lazy.initialize();
    auto copy = Lazy;

    SECTION("Is initialized")
    {
      REQUIRE( copy.has_value() );
    }

    SECTION("Contains the copied value")
    {
      REQUIRE( *copy == "Hello World" );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy( Lazy&& )", "[ctor]")
{
  lazy::Lazy<std::string> Lazy = "Hello World";

  SECTION("Copy constructs an uninitialized Lazy")
  {
    auto move = std::move(Lazy);

    SECTION("Is uninitialized")
    {
      REQUIRE_FALSE( move.has_value() );
    }

    SECTION("Copy constructs on initialization")
    {
      REQUIRE( move.value() == "Hello World" );
    }
  }

  SECTION("Copy constructs an initialized Lazy")
  {
    Lazy.initialize();
    auto move = std::move(Lazy);

    SECTION("Is initialized")
    {
      REQUIRE( move.has_value() );
    }

    SECTION("Contains the moved value")
    {
      REQUIRE( *move == "Hello World" );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy( const Lazy<U>& )", "[ctor]")
{
  lazy::Lazy<const char*> Lazy = "Hello World";

  SECTION("Copy constructs an uninitialized Lazy")
  {
    lazy::Lazy<std::string> copy(Lazy);

    SECTION("Is uninitialized")
    {
      REQUIRE_FALSE( copy.has_value() );
    }

    SECTION("Copy constructs on initialization")
    {
      REQUIRE( copy.value() == "Hello World" );
    }
  }

  SECTION("Copy constructs an initialized Lazy")
  {
    Lazy.initialize();
    lazy::Lazy<std::string> copy(Lazy);

    SECTION("Is initialized")
    {
      REQUIRE( copy.has_value() );
    }

    SECTION("Contains the moved value")
    {
      REQUIRE( *copy == "Hello World" );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy( Lazy<U>&& )", "[ctor]")
{
  lazy::Lazy<const char*> Lazy = "Hello World";

  SECTION("Move constructs an uninitialized Lazy")
  {
    lazy::Lazy<std::string> move(std::move(Lazy));

    SECTION("Is uninitialized")
    {
      REQUIRE_FALSE( move.has_value() );
    }

    SECTION("Copy constructs on initialization")
    {
      REQUIRE( move.value() == "Hello World" );
    }
  }

  SECTION("Move constructs an initialized Lazy")
  {
    Lazy.initialize();
    lazy::Lazy<std::string> move(std::move(Lazy));

    SECTION("Is initialized")
    {
      REQUIRE( move.has_value() );
    }

    SECTION("Contains the moved value")
    {
      REQUIRE( *move == "Hello World" );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy( U&& )", "[ctor]")
{
  lazy::Lazy<int> Lazy = 3.5;

  SECTION("Value constructs the Lazy")
  {
    SECTION("Is uninitialized")
    {
      REQUIRE_FALSE( Lazy.has_value() );
    }

    SECTION("Default constructs on initialization")
    {
      REQUIRE( Lazy.value() == int(3.5) );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy( in_place_t, Args&&...args )", "[ctor]")
{
  lazy::Lazy<std::string> Lazy( lazy::in_place, "Hello World", 5 );

  SECTION("Variadic contructs an uninitialized Lazy")
  {
    SECTION("Is uninitialized")
    {
      REQUIRE_FALSE( Lazy.has_value() );
    }

    SECTION("Constructs on initialization")
    {
      REQUIRE( Lazy.value() == "Hello" );
    }
  }
}

//----------------------------------------------------------------------------
// Assignment
//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::operator=( const Lazy& )", "[assignment]")
{
  lazy::Lazy<std::string> source = "Hello World";
  lazy::Lazy<std::string> destination = "Goodbye World";

  SECTION("Copy assigns uninitialized destination with uninitialized source")
  {
    bool before = destination.has_value();
    destination = source;
    bool after  = destination.has_value();

    SECTION("destination is uninitialized")
    {
      REQUIRE_FALSE( destination.has_value() );
    }

    SECTION("source is uninitialized")
    {
      REQUIRE_FALSE( source.has_value() );
    }

    SECTION("destination state is unchanged")
    {
      REQUIRE( before == after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == source.value() );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Copy assigns uninitialized destination with initialized source")
  {
    source.initialize();
    bool before = destination.has_value();
    destination = source;
    bool after  = destination.has_value();

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("destination state is changed")
    {
      REQUIRE( before != after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == source.value() );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Copy assigns initialized copy with uninitialized source")
  {
    destination.initialize();
    bool before = source.has_value();
    destination = source;
    bool after  = source.has_value();

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("source state is changed")
    {
      REQUIRE( before != after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == source.value() );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Copy assigns initialized copy with initialized source")
  {
    destination.initialize();
    source.initialize();
    destination = source;

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == source.value() );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::operator=( Lazy&& )", "[assignment]")
{
  const char* string = "Hello World";
  lazy::Lazy<std::string> source = string;
  lazy::Lazy<std::string> destination = "Goodbye World";

  SECTION("Move assigns uninitialized destination with uninitialized source")
  {
    bool before = destination.has_value();
    destination = std::move(source);
    bool after  = destination.has_value();

    SECTION("destination is uninitialized")
    {
      REQUIRE_FALSE( destination.has_value() );
    }

    SECTION("source is uninitialized")
    {
      REQUIRE_FALSE( source.has_value() );
    }

    SECTION("destination state is unchanged")
    {
      REQUIRE( before == after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == string );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Move assigns uninitialized destination with initialized source")
  {
    source.initialize();
    bool before = destination.has_value();
    destination = std::move(source);
    bool after  = destination.has_value();

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("destination state is changed")
    {
      REQUIRE( before != after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == string );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Move assigns initialized copy with uninitialized source")
  {
    destination.initialize();
    bool before = source.has_value();
    destination = std::move(source);
    bool after  = source.has_value();

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("source state is changed")
    {
      REQUIRE( before != after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == string );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Move assigns initialized copy with initialized source")
  {
    destination.initialize();
    source.initialize();
    destination = std::move(source);

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == string );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::operator=( const Lazy<U>& )", "[assignment]")
{
  lazy::Lazy<const char*> source = "Hello World";
  lazy::Lazy<std::string> destination = "Goodbye World";

  SECTION("Copy assigns uninitialized destination with uninitialized source")
  {
    bool before = destination.has_value();
    destination = source;
    bool after  = destination.has_value();

    SECTION("destination is uninitialized")
    {
      REQUIRE_FALSE( destination.has_value() );
    }

    SECTION("source gets initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("destination state is unchanged")
    {
      REQUIRE( before == after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == source.value() );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Copy assigns uninitialized destination with initialized source")
  {
    source.initialize();
    bool before = destination.has_value();
    destination = source;
    bool after  = destination.has_value();

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("destination state is changed")
    {
      REQUIRE( before != after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == source.value() );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Copy assigns initialized copy with uninitialized source")
  {
    destination.initialize();
    bool before = source.has_value();
    destination = source;
    bool after  = source.has_value();

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("source state is changed")
    {
      REQUIRE( before != after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == source.value() );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Copy assigns initialized copy with initialized source")
  {
    destination.initialize();
    source.initialize();
    destination = source;

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == source.value() );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::operator=( Lazy<U>&& )", "[assignment]")
{
  const char* string = "Hello World";
  lazy::Lazy<const char*> source = string;
  lazy::Lazy<std::string> destination = "Goodbye World";

  SECTION("Move assigns uninitialized destination with uninitialized source")
  {
    bool before = destination.has_value();
    destination = std::move(source);
    bool after  = destination.has_value();

    SECTION("destination is uninitialized")
    {
      REQUIRE_FALSE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("destination state is unchanged")
    {
      REQUIRE( before == after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == string );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Move assigns uninitialized destination with initialized source")
  {
    source.initialize();
    bool before = destination.has_value();
    destination = std::move(source);
    bool after  = destination.has_value();

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("destination state is changed")
    {
      REQUIRE( before != after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == string );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Move assigns initialized copy with uninitialized source")
  {
    destination.initialize();
    bool before = source.has_value();
    destination = std::move(source);
    bool after  = source.has_value();

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("source state is changed")
    {
      REQUIRE( before != after );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == string );
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Move assigns initialized copy with initialized source")
  {
    destination.initialize();
    source.initialize();
    destination = std::move(source);

    SECTION("destination is initialized")
    {
      REQUIRE( destination.has_value() );
    }

    SECTION("source is initialized")
    {
      REQUIRE( source.has_value() );
    }

    SECTION("Initializes destination with value of source")
    {
      REQUIRE( destination.value() == string );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::operator=( U&& )", "[assignment]")
{
  const char* string = "Hello World";
  lazy::Lazy<std::string> Lazy;

  SECTION("Assigning to uninitialized Lazy")
  {
    SECTION("Copy-assigns lvalue references")
    {
      bool before = Lazy.has_value();
      Lazy = string;
      bool after  = Lazy.has_value();

      SECTION("Initializes Lazy")
      {
        REQUIRE( before != after );
      }

      SECTION("Lazy contains new value")
      {
        REQUIRE( *Lazy == string );
      }
    }

    SECTION("Move-assigns rvalue references")
    {
      auto copy = string;
      bool before = Lazy.has_value();
      Lazy = std::move(string);
      bool after  = Lazy.has_value();

      SECTION("Initializes Lazy")
      {
        REQUIRE( before != after );
      }

      SECTION("Lazy contains new value")
      {
        REQUIRE( *Lazy == copy );
      }
    }
  }

  //--------------------------------------------------------------------------

  SECTION("Assigning to initialized Lazy")
  {
    Lazy.initialize();

    SECTION("Copy-assigns lvalue references")
    {
      bool before = Lazy.has_value();
      Lazy = string;
      bool after  = Lazy.has_value();

      SECTION("Lazy is already initialized")
      {
        REQUIRE( before == after );
      }

      SECTION("Lazy contains new value")
      {
        REQUIRE( *Lazy == string );
      }
    }

    //--------------------------------------------------------------------------

    SECTION("Move-assigns rvalue references")
    {
      Lazy = std::move(string);

      auto copy = string;
      bool before = Lazy.has_value();
      Lazy = std::move(string);
      bool after  = Lazy.has_value();

      SECTION("Lazy is already initialized")
      {
        REQUIRE( before == after );
      }

      SECTION("Lazy contains new value")
      {
        REQUIRE( *Lazy == copy );
      }
    }
  }
}

//----------------------------------------------------------------------------
// Modifiers
//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::initialize()", "[modifiers]")
{
  lazy::Lazy<int> Lazy = 42;

  SECTION("Initializes uninitialized Lazy")
  {
    bool before = Lazy.has_value();
    Lazy.initialize();
    bool after = Lazy.has_value();

    SECTION("Lazy changes state")
    {
      REQUIRE( before != after );
    }

    SECTION("Lazy contains value")
    {
      REQUIRE( *Lazy == 42 );
    }
  }

  SECTION("Doesn't modify initialized Lazy")
  {
    Lazy.initialize();
    bool before = Lazy.has_value();
    Lazy.initialize();
    bool after = Lazy.has_value();

    SECTION("Lazy state doesn't change")
    {
      REQUIRE( before == after );
    }

    SECTION("Lazy contains value")
    {
      REQUIRE( *Lazy == 42 );
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::reset()", "[modifiers]")
{
   lazy::Lazy<int> Lazy = 42;

   SECTION("Does nothing to uninitialized lazies")
   {
     bool before = Lazy.has_value();
     Lazy.reset();
     bool after = Lazy.has_value();

     REQUIRE( before == after );
   }

   SECTION("Does nothing to uninitialized lazies")
   {
     Lazy.initialize();
     bool before = Lazy.has_value();
     Lazy.reset();
     bool after = Lazy.has_value();

     REQUIRE( before != after );
   }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::swap( Lazy& )", "[modifiers]")
{
  int left_value  = 0xdead;
  int right_value = 0xbeef;
  lazy::Lazy<int> left  = left_value;
  lazy::Lazy<int> right = right_value;

  SECTION("Swapping uninitialized Lazy with uninitialized Lazy")
  {
    left.swap(right);

    SECTION("Left is still uninitialized")
    {
      REQUIRE_FALSE( left.has_value() );
    }

    SECTION("Right is still uninitialized")
    {
      REQUIRE_FALSE( right.has_value() );
    }

    SECTION("Left initializes with right's value")
    {
      REQUIRE( left.value() == right_value );
    }

    SECTION("Right initializes with left's value")
    {
      REQUIRE( right.value() == left_value );
    }
  }

  SECTION("Swapping initialized Lazy with uninitialized Lazy")
  {
    left.initialize();
    left.swap(right);

    SECTION("Left is initialized")
    {
      REQUIRE( left.has_value() );
    }

    SECTION("Right is ninitialized")
    {
      REQUIRE( right.has_value() );
    }

    SECTION("Left initializes with right's value")
    {
      REQUIRE( left.value() == right_value );
    }

    SECTION("Right initializes with left's value")
    {
      REQUIRE( right.value() == left_value );
    }
  }

  SECTION("Swapping uninitialized Lazy with initialized Lazy")
  {
    right.initialize();
    left.swap(right);

    SECTION("Left is initialized")
    {
      REQUIRE( left.has_value() );
    }

    SECTION("Right is ninitialized")
    {
      REQUIRE( right.has_value() );
    }

    SECTION("Left initializes with right's value")
    {
      REQUIRE( left.value() == right_value );
    }

    SECTION("Right initializes with left's value")
    {
      REQUIRE( right.value() == left_value );
    }
  }

  SECTION("Swapping initialized Lazy with initialized Lazy")
  {
    left.initialize();
    right.initialize();
    left.swap(right);

    SECTION("Left is initialized")
    {
      REQUIRE( left.has_value() );
    }

    SECTION("Right is ninitialized")
    {
      REQUIRE( right.has_value() );
    }

    SECTION("Left initializes with right's value")
    {
      REQUIRE( left.value() == right_value );
    }

    SECTION("Right initializes with left's value")
    {
      REQUIRE( right.value() == left_value );
    }
  }
}

//---------------------------------------------------------------------------
// Observers
//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::operator bool")
{
  lazy::Lazy<int> Lazy;

  SECTION("Returns true when initialized")
  {
    Lazy.initialize();

    REQUIRE( Lazy );
  }

  SECTION("Returns false when uninitialized")
  {
    REQUIRE_FALSE( Lazy );
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::has_value")
{
  lazy::Lazy<int> Lazy;

  SECTION("Returns true when initialized")
  {
    Lazy.initialize();

    REQUIRE( Lazy.has_value() );
  }

  SECTION("Returns false when uninitialized")
  {
    REQUIRE_FALSE( Lazy.has_value() );
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::value() &")
{
  int value = 42;
  lazy::Lazy<int> Lazy = value;

  SECTION("Initializes uninitialized Lazy")
  {
    bool before = Lazy.has_value();
    Lazy.value();
    bool after = Lazy.has_value();

    REQUIRE( before != after );
  }

  SECTION("Returns the deferred value")
  {
    REQUIRE( Lazy.value() == value );
  }

  SECTION("Returns non-const lvalue reference")
  {
    bool result = std::is_same<decltype(Lazy.value()),int&>::value;

    REQUIRE( result );
  }
}

TEST_CASE("lazy::Lazy::value() const &")
{
  int value = 42;
  const lazy::Lazy<int> Lazy = value;

  SECTION("Initializes uninitialized Lazy")
  {
    bool before = Lazy.has_value();
    Lazy.value();
    bool after = Lazy.has_value();

    REQUIRE( before != after );
  }

  SECTION("Returns the deferred value")
  {
    REQUIRE( Lazy.value() == value );
  }

  SECTION("Returns const lvalue reference")
  {
    bool result = std::is_same<decltype(Lazy.value()),const int&>::value;

    REQUIRE( result );
  }

}

TEST_CASE("lazy::Lazy::value() &&")
{
  int value = 42;
  lazy::Lazy<int> Lazy = value;

  SECTION("Initializes uninitialized Lazy")
  {
    bool before = Lazy.has_value();
    std::move(Lazy).value();
    bool after = Lazy.has_value();

    REQUIRE( before != after );
  }

  SECTION("Returns the deferred value")
  {
    REQUIRE( std::move(Lazy).value() == value );
  }

  SECTION("Returns non-const rvalue reference")
  {
    bool result = std::is_same<decltype(std::move(Lazy).value()),int&&>::value;

    REQUIRE( result );
  }

}

TEST_CASE("lazy::Lazy::value() const &&")
{
  int value = 42;
  const lazy::Lazy<int> Lazy = value;

  SECTION("Initializes uninitialized Lazy")
  {
    bool before = Lazy.has_value();
    std::move(Lazy).value();
    bool after = Lazy.has_value();

    REQUIRE( before != after );
  }

  SECTION("Returns the deferred value")
  {
    REQUIRE( std::move(Lazy).value() == value );
  }

  SECTION("Returns const rvalue reference")
  {
    bool result = std::is_same<decltype(std::move(Lazy).value()),const int&&>::value;

    REQUIRE( result );
  }

}

//----------------------------------------------------------------------------

TEST_CASE("lazy::Lazy::value_or( U&& ) const &")
{
  const lazy::Lazy<int> Lazy = 42;

  SECTION("Returns underlying value when initialized")
  {
    Lazy.initialize();

    REQUIRE( Lazy.value_or(24) == 42 );
  }

  SECTION("Returns default_value when initialized")
  {
    REQUIRE( Lazy.value_or(24) == 24 );
  }
}

TEST_CASE("lazy::Lazy::value_or( U&& ) &&")
{
  lazy::Lazy<int> lazy = 42;

  SECTION("Returns underlying value when initialized")
  {
    std::move(lazy).initialize();

    REQUIRE( std::move(lazy).value_or(24) == 42 );
  }

  SECTION("Returns default_value when initialized")
  {
    REQUIRE( std::move(lazy).value_or(24) == 24 );
  }
}

//----------------------------------------------------------------------------
// Utility Generators
//----------------------------------------------------------------------------

TEST_CASE("lazy::make_lazy<T>( Args&&... )", "[utility]")
{
  auto lazy = lazy::make_lazy<std::string>("Hello World", 5);

  SECTION("Creates uninitialized Lazy")
  {
    REQUIRE_FALSE( lazy.has_value() );
  }

  SECTION("Initializes by calling underlying constructor")
  {
    REQUIRE( lazy.value() == std::string("Hello World", 5) );
  }
}

//----------------------------------------------------------------------------

TEST_CASE("lazy::make_kazy_generator<T>( Ctor&& )", "[utility]")
{
  auto lazy = lazy::make_lazy_generator<int>([](){ return std::make_tuple( 42 ); });

  SECTION("Creates uninitialized Lazy")
  {
    REQUIRE_FALSE( lazy.has_value() );
  }

  SECTION("Initializes with value stored in generator")
  {
    REQUIRE( lazy.value() == 42 );
  }
}
