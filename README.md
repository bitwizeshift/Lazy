# Lazy<T>

## What is `Lazy<T>`?

`Lazy<T>` is a lightweight lazy-loading wrapper around any arbitrary class `T` written in modern C++. 

It works by lazily forwarding constructor arguments stored in a `std::tuple` to `T`'s constructor prior 
to the first invocation of the lazy object. 

Construction is handled by passing functions that return the appropriate constructor signature stored in a `std::tuple` to the `Lazy<T>` object. Upon invocation of a member function, `Lazy<T>` will instantiate the type by unpacking and forwarding the supplied arguments.

All constructor signatures called to `Lazy<T>` are statically type-checked for validity with the underlying 
type `T` class, and will assert if the signature does not conform to a given constructor.

Here's an example of how this works:
```c++
auto lazy_string = lazy::make_lazy<std::string>("Hello world!");
// Creates Lazy object, doesn't construct std::string

// ... do other operations

std::cout << *lazy_string << "\n"; 
//           ^----------^
//           Constructs the std::string here for first use

lazy_string->resize(10); // Already constructed, using the same std::string
```

## How to Use

### Inclusion

`Lazy<T>` is a header-only library, making it easy to drop into any new project. 

To start using, just include `Lazy.hpp`.

### `Lazy<T>` Objects

All `Lazy` objects have `operator->` and `operator*` overloaded, making them behave similar to a smart pointer or an iterator. The underlying type will remain uninstantiated until such time that either `->` or `*` is used, at which point it will attempt a construction of the underlying type `T`.

This means that the type managed by a `Lazy` does _not_ require a default or trivial constructor in order for this to properly operate; it simply requires a a function to inform it how to instantiate the type at a later point.

### Construction

There are various ways to construct lazy objects through `Lazy<T>` depending on the required needs. 

1. Default construction
2. `make_lazy<T>` utility for forwarding arguments
3. Function delegation for manually specifying construction/destruction behavior
4. Copy/Move construction/assignment

#### 1. Default Construction

If you have a type that is to be lazily constructed with the default or trivial constructor, use the default constructor for the `Lazy` class. This isn't particularly exciting, but this is the default behavior.

```c++
auto lazy_foo = lazy::Lazy<Foo>();

// ...

lazy_foo->do_something(); // constructs Foo object with Foo()
```

#### 2. `make_lazy<T>` Utility Function

If no custom logic is required to construct the type `T` other than the constructor arguments, then you can easily make use of the `lazy::make_lazy<T>(...)` utility function. 

`make_lazy` behaves similarly to `std::make_shared` and `std::make_unique` in that it forwards it's arguments to the underlying type. The difference is that these arguments are stored until which time that the `Lazy` object is constructed for its first use.

Note that this means that arguments supplied to `make_lazy` will require a copy constructor, since copies are used. This is necessary to avoid dangling reference problems when the values passed go out of scope prior to construction of the object.

An example of using `make_lazy`:
```c++

auto lazy_string = lazy::make_lazy<std::string>("Hello World",10);

// do something

std::cout << *lazy_string << std::endl; 
//           ^
//           Constructs std::string with the constructor std::string(const char*, size_t)
```

#### 3. Function delegation

If more complex logic is required for the construction of the `T` object, you can supplie a construction (and optionally destruction) function-like object (function pointer, member-function,functor,or lambda).

The _construction_ function simple must return a `std::tuple` containing the types that will be forwarded to the constructor (I recommend making use of `std::make_tuple` to simplify this). 

The _destruction_ function must take type `T&` as a parameter and return void. The purpose of the destruction function is to give the chance to clean up anything that may not be managed by a destructor (or, in the case of using fundamental types like pointers, the chance to delete). The _destruction_ function will be called prior to calling `T`'s destructor.

An example of where this may become useful:
```c++
auto create_file = [](){
  return std::make_tuple(fopen("some/file/path","r"));
};
auto close_file = [](FILE* ptr){
  fclose(ptr);
};
auto lazy_file = lazy::Lazy<FILE*>(create_file,close_file);

// somewhere else
use_file(*lazy_file); // constructs the lazy file object
```
#### 4. Copy/Move Constructor/Assignment

A `Lazy` object is able to be constructed out of an instance of the underlying type `T` through copy or move construction. The same can also be done out of instances of `Lazy<T>` as well.

In the case of `T` objects, the types will be used for deferred construction later on through a call to the copy or move constructors.
In the case of `Lazy<T>` objects, they are constructed immediately, provided the `Lazy` being copied or moved has also itself been instantiated. If it is not, only the construction/destruction functions are copied or moved

Similarly, the `Lazy` objects can be assigned to other `Lazy` objects, or directly to the type `T`. 
For assignments, if `T` is not already constructed, the assignment will become a call to the copy or move constructor for `T`. If it is already constructed, then it will be a call to the copy or move asignment operator.

Examples:
```c++
auto a = lazy::make_lazy<std::string>("Hello world");
auto b = a; // copy construction
```

### More

Though this library is mostly complete, it has not been thoroughly tested yet.
If any issues or bugs are encountered, please raise them through the [Github Issues Page](https://github.com/bitwizeshift/Lazy/issues). 

Other than that, this library is licensed under [MIT](LICENSE.md), so feel free to make use of it and enjoy!