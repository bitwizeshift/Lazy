# Lazy<T>
[![Build Status](https://travis-ci.org/bitwizeshift/Lazy.svg?branch=master)](https://travis-ci.org/bitwizeshift/Lazy)
[![Github Issues](https://img.shields.io/github/issues/bitwizeshift/Lazy.svg)](http://github.com/bitwizeshift/Lazy/issues)
[![Tested Compilers](https://img.shields.io/badge/compilers-gcc%20%7C%20clang-blue.svg)](#tested-compilers)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](http://bitwizeshift.github.io/Lazy)
[![GitHub License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/bitwizeshift/Lazy/master/LICENSE.md)
[![Github Releases](https://img.shields.io/github/release/bitwizeshift/Lazy.svg)](https://github.com/bitwizeshift/Lazy/releases)


## What is `Lazy<T>`?

`Lazy<T>` is a lightweight lazy-loading wrapper around any arbitrary class `T` written in modern C++. 

All `Lazy` objects have their construction deferred until the latest possible time to ensure the laziest
construction. Upon invocation of a member function, the `Lazy<T>` will instantiate type `T` by
forwarding `T` the arguments originally supplied with the construction of the `Lazy`.
 
Here's an example of `Lazy` in action:
```c++
auto lazy_string = lazy::make_lazy<std::string>("Hello world!");
// Creates Lazy object, doesn't construct std::string

// ... do other operations

std::cout << *lazy_string << "\n"; 
//           ^----------^
//           Constructs the std::string here for first use here

lazy_string->resize(10); // Already constructed, using the same std::string
```

*It's good to be lazy!*

This library has been written with an emphasis on performance, and aims to reduce unnecessary overhead due to redundant instantiations or duplications. In order to achieve this, deferred argument construction takes copies of all arguments to be passed to the constructor and, at construction time, will use move semantics to invoke the construction of the object. This reduces the need to pay for the cost of duplicate copies, to a cost of copy-and-move instead.

## Why be lazy?

Certain objects may be expensive to construct, especially when they perform complex algorithms, async callbacks, and connections to outside systems. In situations where these objects may not be used within
all possible execution paths, this can result in wasted cpu cycles. 

Often, a lazy-initialization pattern is implemented within the class for the specific-case where it is
needed, resulting in a lot of boilerplate code, and possible areas of failure. `Lazy<T>` seeks to make it easier to lazily-initialize entries by handling all of the work with a simple tested API that behaves like a standard c++ smart pointer. 

## How to Use

### Inclusion

`Lazy<T>` is a header-only library, making it easy to drop into any new project. 

To start using, add a `-I` to the `include/` directory, and include the header `lazy/Lazy.hpp` from the project.

```c++
#include <lazy/Lazy.hpp>

// use lazy::Lazy<T> 
```

### `Lazy<T>` Objects

All `Lazy` objects have `operator->` and `operator*` overloaded, making them behave similar to a smart pointer or an iterator. The underlying type will remain uninstantiated until such time that certain functions are accessed -- such as when the operators `->` or `*` are accessed. At which point it will attempt a construction of the underlying type `T`.

This means that the type managed by a `Lazy` does _not_ require a default or trivial constructor in order for this to properly operate; it simply requires a function to inform it how to instantiate the type at a later point.

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

`make_lazy` behaves similarly to `std::make_shared` and `std::make_unique` in that it forwards its arguments to the underlying type. The difference is that these arguments are stored until which time that the `Lazy` object is constructed for its first use.

Note that this means that arguments supplied to `make_lazy` will require a copy constructor, since copies are used. This is necessary to avoid dangling reference problems when the values passed go out of scope prior to construction of the object.

An example of using `make_lazy`:
```c++

auto lazy_string = lazy::make_lazy<std::string>("Hello World",5);

// do something

std::cout << *lazy_string << std::endl; 
//           ^~~~~~~~~~~^
//           Constructs std::string with the constructor std::string(const char*, size_t)
```

#### 3. Function delegation

If more complex logic is required for the construction of the `T` object, you can supply a construction (and optionally destruction) function-like object (function pointer, member-function,functor, or lambda).

The _construction_ function simple must return a `std::tuple` containing the types that will be forwarded to the constructor (I recommend making use of `std::make_tuple` to simplify this). 

The _destruction_ function must take type `T&` as a parameter and return `void`. The purpose of the destruction function is to give the chance to clean up anything that may not be managed by a destructor (or, in the case of using fundamental types like pointers, the chance to delete). The _destruction_ function will be called prior to calling `T`'s destructor.

An example of where this may become useful:
```c++
auto open_file = [](){
  return std::make_tuple(fopen("some/file/path","r"));
};
auto close_file = [](FILE* ptr){
  fclose(ptr);
};
auto lazy_file = lazy::Lazy<FILE*>(open_file,close_file);

// somewhere else
use_file(*lazy_file); // constructs the lazy file object
```
#### 4. Copy/Move Constructor/Assignment

A `Lazy` object is able to be constructed out of an instance of the underlying type `T` through copy or move construction. The same can also be done with instances of `Lazy<T>` as well.

In the case of `T` objects, the types will be used for deferred construction later on through a call to the copy or move constructors.
In the case of `Lazy<T>` objects, they are constructed immediately, provided the `Lazy` being copied or moved has also itself been instantiated. If it is not, only the construction/destruction functions are copied or moved.

Similarly, the `Lazy` objects can be assigned to other `Lazy` objects, or directly to the type `T`. 
For assignments to type `T`, if the lazy is not already constructed, it will lazily construct itself prior to assignment to avoid the need for a copy or move constructor. 
For assignments to type `Lazy`, if the lazy being assigned is already initialized, the value will be 
assigned to the currently constructed `Lazy`. If it is uninitialized, only the constructor to the 
type will be assigned to defer instantiation until later use.

Examples:
```c++
auto a = lazy::make_lazy<std::string>("Hello world");
auto b = a; // copy construction
```

##<a name="tested-compilers"></a> Tested Compilers

The following compilers are currently being tested through continuous integration with [Travis](https://travis-ci.org/bitwizeshift/Lazy).

Note that `Lazy` only works on compiler that provide proper conformance for c++11, meaning this
does not properly work on g++ before 4.8 

| Compiler        | Operating System                   |
|-----------------|------------------------------------|
| g++ 4.9.3       | Ubuntu 14.04.3 TLS                 |
| g++ 5.3.0       | Ubuntu 14.04.3 TLS                 |
| g++ 6.1.1       | Ubuntu 14.04.3 TLS                 |
| clang 3.5.0     | Ubuntu 14.04.3 TLS                 |
| clang 3.6.2     | Ubuntu 14.04.3 TLS                 |
| clang 3.8.0     | Ubuntu 14.04.3 TLS                 |
| clang xcode 6.0 | Darwin Kernel 13.4.0 (OSX 10.9.5)  |
| clang xcode 6.1 | Darwin Kernel 14.3.0 (OSX 10.10.3) |
| clang xcode 7.0 | Darwin Kernel 14.5.0 (OSX 10.10.5) |
| clang xcode 7.3 | Darwin Kernel 15.5.0 (OSX 10.11.5) |
| clang xcode 8.0 | Darwin Kernel 15.6.0 (OSX 10.11.6) |

### More

If any issues or bugs are encountered, please raise them through the [Github Issues Page](https://github.com/bitwizeshift/Lazy/issues). 

Other than that, this library is licensed under [MIT](#S-license), so feel free to make use of it and enjoy!

##<a name="S-license"></a> License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The class is licensed under the [MIT License](http://opensource.org/licenses/MIT):

Copyright &copy; 2016 [Matthew Rodusek](http://rodusek.me/)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.