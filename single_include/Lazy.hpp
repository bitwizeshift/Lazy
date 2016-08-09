/**
 * \file Lazy.hpp
 *
 * \brief This is the main include file for the \c Lazy library.
 *
 * Including this gives access to \c lazy::Lazy<T> and the utility
 * \c lazy::make_lazy functions.
 *
 *
 * \author Matthew Rodusek (matthew.rodusek@gmail.com)
 * \copyright Matthew Rodusek
 */

/*
 * The MIT License (MIT)
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2016 Matthew Rodusek <http://rodusek.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef LAZY_LAZY_HPP_
#define LAZY_LAZY_HPP_

#include <type_traits>
#include <functional>
#include <tuple>
#include <cstdlib>

namespace lazy{

  namespace detail{

    // c++14 index sequence

    /// \brief type-trait to expand into a sequence of integers
    ///
    /// \note This is included in c++14 under 'utility', but not in c++11
    template<typename T, T... Ints>
    class integer_sequence{
      static constexpr std::size_t size(){ return sizeof...(Ints); }
    };

    /// \brief Type alias of the common-case for integer sequences of std::size_t
    template<std::size_t...Ints>
    using index_sequence = integer_sequence<std::size_t,Ints...>;

    /// \brief type-trait helper to build an integer sequence
    template<std::size_t Start, std::size_t N, std::size_t... Ints>
    struct build_index_sequence
      : public build_index_sequence<Start, N - 1, N - 1, Ints...>{};

    template<std::size_t Start, std::size_t... Ints>
    struct build_index_sequence<Start, Start, Ints...>{
      typedef index_sequence<Ints...> type;
    };

    /// \brief type-trait helper to build an index sequence from 0 to N
    template<std::size_t N>
    using make_index_sequence = typename build_index_sequence<0,N>::type;

    /// \brief type-trait helper to build an index sequence of 0 to Args indices
    template<typename...Args>
    using index_sequence_for = make_index_sequence<sizeof...(Args)>;

    template<bool b>
    using boolean_constant = std::integral_constant<bool,b>;

    /// \brief Type-trait for identities (always defines \c T as \c type)
    ///
    /// This aliases \c T as \c ::type
    template<typename T>
    struct identity{
      typedef T type;
    };

    //------------------------------------------------------------------------

    /// \brief Type trait for getting the nth argument type from a variadic
    ///        template.
    ///
    /// This is used for composition in \c function_traits
    ///
    /// The result is aliased as \c ::type
    template<std::size_t n, typename...Args>
    struct arg_tuple
      : public identity<
          typename std::tuple_element<n, std::tuple<Args...>
        >::type>
    {
      static_assert(n>=sizeof...(Args),"Index out of range");
    };

    //------------------------------------------------------------------------

    /// \brief Identity type-trait for all function traits to inherit from
    ///
    /// This type is only used in composition.
    ///
    /// This aliases the following common types:
    /// - The number of arguments to the function as \c ::arity
    /// - The type of the return as \c ::result_type
    /// - The nth argument as \c ::arg<n>::type
    template<typename Ret,typename...Args>
    struct function_traits_identity{
      static constexpr std::size_t arity = sizeof...(Args); /// Number of arguments

      typedef Ret result_type; /// Return type

      template<std::size_t n>
      using arg = arg_tuple<n,Args...>; /// Alias of the nth arg
    };

    //------------------------------------------------------------------------

    /// \brief Type traits for retrieving various parts of a function
    ///
    /// This aliases the following
    /// - The number of arguments to the function as \c ::arity
    /// - The type of the return as \c ::result_type
    /// - The nth argument as \c ::arg<n>::type
    template<typename T>
    struct function_traits_impl{
      static_assert(std::is_function<T>::value,"T must be function type!");
    };

    template <typename Ret, typename...Args>
    struct function_traits_impl<Ret(Args...)>
      : public function_traits_identity<Ret,Args...>{};

    template <typename Ret, typename...Args>
    struct function_traits_impl<Ret(*)(Args...)>
      : public function_traits_identity<Ret,Args...>{};

    //------------------------------------------------------------------------

    /// \brief Type traits for retrieving various parts of a member function
    ///
    /// This aliases the following
    /// - The number of arguments to the function as \c ::arity
    /// - The type of the return as \c ::result_type
    /// - The nth argument as \c ::arg<n>::type
    template<typename T>
    struct member_function_traits_impl{
      static_assert(std::is_member_function_pointer<T>::value,"T must be member function pointer!");
    };

    template <typename C, typename Ret, typename... Args>
    struct member_function_traits_impl<Ret(C::*)(Args...)>
      : public function_traits_identity<Ret,Args...>{};

    template <typename C, typename Ret, typename... Args>
    struct member_function_traits_impl<Ret(C::*)(Args...) const>
      : public function_traits_identity<Ret,Args...>{};

    template <typename C, typename Ret, typename... Args>
    struct member_function_traits_impl<Ret(C::*)(Args...) volatile>
      : public function_traits_identity<Ret,Args...>{};

    template <typename C, typename Ret, typename... Args>
    struct member_function_traits_impl<Ret(C::*)(Args...) const volatile>
      : public function_traits_identity<Ret,Args...>{};

    //------------------------------------------------------------------------

    /// \brief Type trait for retrieving various parts of a functor
    ///
    /// This aliases the following
    /// - The number of arguments to the function as \c ::arity
    /// - The type of the return as \c ::result_type
    /// - The nth argument as \c ::arg<n>::type
    template<typename T>
    struct functor_traits_impl : public member_function_traits_impl<decltype(&T::operator())>{
      static_assert(std::is_class<T>::value,"T must be a class type!");
    };

    //------------------------------------------------------------------------

    /// \brief Type traits to retrieve the various parts of a callable
    ///        function-like object
    ///
    /// This aliases the following
    /// - The number of arguments to the function as \c ::arity
    /// - The type of the return as \c ::result_type
    /// - The nth argument as \c ::arg<n>::type
    ///
    /// \tparam T the function to retrieve types for
    template<typename T>
    struct function_traits : public std::conditional<
      std::is_class<T>::value,
      functor_traits_impl<T>,
      typename std::conditional<
        std::is_member_function_pointer<T>::value,
        member_function_traits_impl<T>,
        function_traits_impl<T>
      >::type
    >::type{};


    /// \brief Type trait for determining whether the given type is a functor
    ///
    /// This only works for normal, non-templated operator() types
    ///
    /// This aliases the result as \c ::value
    template<typename T>
    class is_functor{
      typedef char yes_type;
      typedef char(&no_type)[2];

      template<typename C> static yes_type test(decltype(&C::operator()));
      template<typename C> static no_type (&test(...));

    public:
      static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };

    //------------------------------------------------------------------------

    /// \brief type-trait to determine if a type provided is a \c std::tuple
    ///
    /// \note this also detects \c std::pair
    ///
    /// The result is aliased as \c ::value
    template<typename T>
    struct is_tuple : public std::false_type{};

    template<typename...Args>
    struct is_tuple<std::tuple<Args...>> : public std::true_type{};

    template<typename...Args>
    struct is_tuple<std::pair<Args...>> : public std::true_type{};

    //------------------------------------------------------------------------

    /// \brief type-trait that behaves like \c std::is_constructible<T>,
    ///        but determines whether a tuple contains argument types
    ///        that allow construction of the supplied type \c T
    ///
    /// The result is aliased as \c ::value
    template<typename T, typename Tuple>
    struct is_tuple_constructible : public std::false_type{};

    template<typename T, typename...Args>
    struct is_tuple_constructible<T,std::tuple<Args...>> : public std::is_constructible<T,Args...>{};

    template<typename T, typename...Args>
    struct is_tuple_constructible<T,std::pair<Args...>> : public std::is_constructible<T,Args...>{};

    /// \brief Type trait to determine whether or not the type \c T is
    ///        a callable (function, member function, functor)
    ///
    /// The result is aliased as \c ::type
    template<typename T>
    struct is_callable : std::conditional<
      std::is_class<T>::value,
      detail::is_functor<T>,
      typename std::conditional<
        std::is_member_function_pointer<T>::value,
        std::is_member_function_pointer<T>,
        std::is_function<T>
      >::type
    >::type{};

  } // namespace detail

  ////////////////////////////////////////////////////////////////////////////
  /// \brief Lazy class used for lazy-loading any type
  ///
  /// The stored lazy-loaded class, \c T, will always be instantiated
  /// before being accessed, and destructed when put out of scope.
  ///
  /// \tparam T the type contained within this \c Lazy
  ////////////////////////////////////////////////////////////////////////////
  template<typename T>
  class Lazy final
  {
    //------------------------------------------------------------------------
    // Public Member Types
    //------------------------------------------------------------------------
  public:

    using this_type = Lazy<T>; ///< Instance of this type

    using value_type = T;  ///< The underlying type of this Lazy
    using pointer    = T*; ///< The pointer type of the Lazy
    using reference  = T&; ///< The reference type of the Lazy

    //------------------------------------------------------------------------
    // Construction / Destruction / Assignment
    //------------------------------------------------------------------------
  public:

    /// \brief Default constructor; no initialization takes place
    Lazy( );

    /// \brief Constructs a \c Lazy given the \p constructor and \p destructor
    ///        functions
    ///
    /// \note The \p constructor function must return a \c std::tuple containing
    ///       the arguments to pass to \c T's constructor for lazy-construction
    ///
    /// \param constructor function to use for construction
    /// \param destructor  function to use prior to destruction
    template<
      typename CtorFunc,
      typename DtorFunc = void(value_type&),
      typename = typename std::enable_if<detail::is_callable<CtorFunc>::value>::type,
      typename = typename std::enable_if<detail::is_callable<DtorFunc>::value>::type
    >
    explicit Lazy( const CtorFunc& constructor,
                   const DtorFunc& destructor = default_destructor );

    /// \brief Constructs a \c Lazy by copying another \c Lazy
    ///
    /// \note If \p rhs is initialized, then this copy will also be initialized
    ///
    /// \param rhs the \c Lazy to copy
    Lazy( const Lazy<T>& rhs );

    /// \brief Constructs a \c Lazy by moving another \c Lazy
    ///
    /// \note If \p rhs is initialized, then this moved version will
    ///       also be initialized
    ///
    /// \param rhs the \c Lazy to move
    Lazy( Lazy<T>&& rhs );

    /// \brief Constructs a \c Lazy by calling \c T's copy constructor
    ///
    /// \note This does not initialize the \c Lazy. Instead, it stores this value as
    ///       a copy and move-constructs it later, if necessary
    ///
    /// \param rhs the \c T to copy
    explicit Lazy( const value_type& rhs );

    /// \brief Constructs a \c Lazy from a given rvalue \c T
    ///
    /// \note This does not initialize the \c Lazy. Instead, it stores this value as
    ///       a copy and move-constructs it later, if necessary
    ///
    /// \param rhs the \c T to move
    explicit Lazy( value_type&& rhs );

    //------------------------------------------------------------------------

    /// \brief Destructs this \c Lazy and it's \c T
    ~Lazy( );

    //------------------------------------------------------------------------

    /// \brief Assigns a \c Lazy to this \c Lazy
    ///
    /// \note This will construct a new \c T if the \c Lazy is not already
    ///       initialized, otherwise it will assign
    ///
    /// \param rhs the \c Lazy on the right-side of the assignment
    /// \return reference to (*this)
    this_type& operator=( const this_type& rhs );

    /// \brief Assigns a \c Lazy to this \c Lazy
    ///
    /// \note This will construct a new \c T if the \c Lazy is not already
    ///       initialized, otherwise it will assign
    ///
    /// \param rhs the rvalue \c Lazy on the right-side of the assignment
    /// \return reference to (*this)
    this_type& operator=( this_type&& rhs );

    /// \brief Assigns a \c T to this \c Lazy
    ///
    /// \note This will construct a new \c T if the \c Lazy is not already
    ///       initialized, otherwise it will assign
    ///
    /// \param rhs the \c T on the right-side of the assignment
    /// \return reference to (\c ptr())
    value_type& operator=( const value_type& rhs );

    /// \brief Assigns an rvalue \c T to this \c Lazy
    ///
    /// \note This will construct a new \c T if the \c Lazy is not already
    ///       initialized, otherwise it will assign
    ///
    /// \param rhs the \c T on the right-side of the assignment
    /// \return reference to (\c ptr())
    value_type& operator=( value_type&& rhs );

    //------------------------------------------------------------------------
    // Casting
    //------------------------------------------------------------------------
  public:

    /// \brief Converts this \c Lazy into a reference
    ///
    /// \return the reference to the lazy-loaded object
    explicit operator reference() const;

    /// \brief Checks whether this \c Lazy has an instantiated object
    ///
    /// \return \c true if this lazy has an instantiated object
    explicit operator bool() const noexcept;

    //------------------------------------------------------------------------
    // Operators
    //------------------------------------------------------------------------
  public:

    /// \brief Swapperator class for no-exception swapping
    ///
    /// \param rhs the rhs to swap
    void swap(Lazy<T>& rhs) noexcept;

    /// \brief Boolean to check if this \c Lazy is initialized.
    ///
    /// \return \c true if the underlying type \c T is initialized.
    bool is_initialized() const noexcept;

    /// \brief Gets a pointer to the underlying type
    ///
    /// \note This has been added to have a similar API to smart pointers
    ///
    /// \return the pointer to the underlying type
    pointer get() const;

    /// \brief Dereferences this \c Lazy object into the lazy-loaded object
    ///
    /// \return a constant reference to the lazy-loaded object
    reference operator*() const;

    /// \brief Dereferences this \c Lazy object into the lazy-loaded object
    ///
    /// \return a pointer to the lazy-loaded object
    pointer operator->() const;

    //------------------------------------------------------------------------
    // Private Member Types
    //------------------------------------------------------------------------
  private:

    /// \brief Constructor tag for tag-dispatching VA Arguments
    struct ctor_va_args_tag{};

    using unqualified_pointer = typename std::remove_cv<T>::type*;
    using ctor_function_type  = std::function<void()>;
    using dtor_function_type  = std::function<void(T&)>;

    using storage_type = typename std::aligned_storage<sizeof(T),alignof(T)>::type;

    //------------------------------------------------------------------------
    // Private Members
    //------------------------------------------------------------------------
  private:

    mutable storage_type m_storage;        ///< The storage to hold the lazy type
    mutable bool         m_is_initialized; ///< Is the type initialized?
    ctor_function_type   m_constructor;    ///< The construction function
    dtor_function_type   m_destructor;     ///< The destruction function

    //------------------------------------------------------------------------
    // Private Static Member Functions
    //------------------------------------------------------------------------
  private:

    /// \brief A default destructor function for this Lazy object
    ///
    /// \param x the \c T type to be destructed
    static void default_destructor( value_type& x ) noexcept;

    //------------------------------------------------------------------------
    // Private Constructor
    //------------------------------------------------------------------------
  private:

    /// \brief Constructs a \c Lazy by constructing it's \c T with its constructor
    ///
    /// \param tag  unused tag for dispatching to VA constructor
    /// \param args arguments to \c T's constructor
    template<typename...Args>
    explicit Lazy( ctor_va_args_tag tag, Args&&...args );

    //------------------------------------------------------------------------
    // Private Member Functions
    //------------------------------------------------------------------------
  private:

    /// \brief Gets a pointer to the data stored in this \c Laz
    ///
    /// \return the constant pointer to the object
    unqualified_pointer ptr() const noexcept;

    /// \brief Forcibly initializes the \c Lazy
    void lazy_construct() const;

    /// \brief Constructs a \c Lazy object using \c T's copy constructor
    ///
    /// \param x Instance of \c T to copy.
    void construct( const value_type& x ) const;

    /// \brief Constructs a \c Lazy object using \c T's move constructor
    ///
    /// \param x Instance of rvalue \c T to copy
    void construct( value_type&& x ) const;

    /// \brief Constructs a \c Lazy object using the arguments for \c T's constructor
    ///
    /// \param tag  tag to dispatch to this VA args constructor
    /// \param args the arguments to forward to the constructor
    template<typename...Args>
    void construct( ctor_va_args_tag tag, Args&&...args ) const;

    /// \brief Constructs a \c Lazy object using the arguments provided in a
    ///        \c std::tuple for \c T's constructor
    ///
    /// \param tag  the tag for tag-dispatching
    /// \param args the arguments to forward to the constructor
    template<typename...Args>
    void construct( const std::tuple<Args...>& args ) const;

    /// \brief Constructs a \c lazy object by passing all values stored in a
    ///        \c std::tuple to \c T's constructor
    ///
    /// \param unused unused parameter for getting index list
    template<typename...Args, std::size_t...Is>
    void tuple_construct( const std::tuple<Args...>& args,
                          const detail::index_sequence<Is...>& unused ) const noexcept( std::is_nothrow_constructible<T,Args...>::value );

    //------------------------------------------------------------------------

    /// \brief Destructs the \c Lazy object
    void destruct( ) const;

    //------------------------------------------------------------------------

    /// \brief Copy-assigns type at \c rhs
    ///
    /// \param rhs the value to assign
    void assign( const value_type& rhs ) const noexcept( std::is_nothrow_copy_assignable<T>::value );

    /// \brief Copy-assigns type at \c rhs
    ///
    /// \param rhs the value to assign
    void assign( value_type&& rhs ) const noexcept( std::is_nothrow_move_assignable<T>::value );

    //------------------------------------------------------------------------
    // Friends
    //------------------------------------------------------------------------

    template<typename U,typename...Args>
    friend Lazy<U> make_lazy( Args&&...args );
  };

  //--------------------------------------------------------------------------
  // Utilities
  //--------------------------------------------------------------------------

  /// \brief Convenience utility to construct a \c Lazy object by specifying
  ///        \c T's constructor signature.
  ///
  /// The arguments are stored by copy until the object is constructed in order
  /// to avoid dangling references.
  ///
  /// \param args the arguments to the constructor
  /// \return an instance of the \c Lazy object
  template<typename T, typename...Args>
  Lazy<T> make_lazy( Args&&...args );

  /// \brief Implementation of \c swap for custom swapperations using ADL
  ///
  /// \param lhs the left-hand \c Lazy object
  /// \param rhs the right-hand \c Lazy object
  template<typename T>
  void swap(Lazy<T>& lhs, Lazy<T>& rhs) noexcept;


  //--------------------------------------------------------------------------
  // Constructors / Destructor / Assignment
  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::Lazy()
    : m_storage(),
      m_is_initialized(false),
      m_constructor([this](){this->construct(ctor_va_args_tag());}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_default_constructible<T>::value,"No matching default constructor for type T");
  }

  template<typename T>
  template<typename CtorFunc,typename DtorFunc,typename,typename>
  inline Lazy<T>::Lazy( const CtorFunc& constructor,
                        const DtorFunc& destructor )
    : m_storage(),
      m_is_initialized(false),
      m_constructor([this,constructor](){this->construct(constructor());}),
      m_destructor(destructor)
  {
    using return_type = typename detail::function_traits<CtorFunc>::result_type;

    static_assert(detail::is_tuple<return_type>::value,"Lazy-construction functions must return tuples containing constructor arguments");
    static_assert(detail::is_tuple_constructible<T,return_type>::value, "No matching constructor for type T with given arguments");
  }

  template<typename T>
  inline Lazy<T>::Lazy( const this_type& rhs )
    : m_storage(),
      m_is_initialized(false),
      m_constructor(rhs.m_constructor),
      m_destructor(rhs.m_destructor)
  {
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");

    if(rhs.m_is_initialized)
    {
      construct(*rhs);
    }
  }

  template<typename T>
  inline Lazy<T>::Lazy( this_type&& rhs )
    : m_storage(),
      m_is_initialized(false),
      m_constructor(std::move(rhs.m_constructor)),
      m_destructor(std::move(rhs.m_destructor))
  {
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");

    if(rhs.m_is_initialized)
    {
      construct(std::move(*rhs));
    }
    rhs.m_constructor = nullptr;
    rhs.m_destructor = nullptr;
  }

  template<typename T>
  inline Lazy<T>::Lazy( const value_type& rhs )
    : m_storage(),
      m_is_initialized(false),
      m_constructor([this,rhs](){this->construct(std::move(rhs));}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");
  }

  template<typename T>
  inline Lazy<T>::Lazy( value_type&& rhs )
    : m_storage(),
      m_is_initialized(false),
      m_constructor([this,rhs](){this->construct(std::move(rhs));}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");
  }

  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::~Lazy()
  {
    destruct();
  }

  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>& Lazy<T>::operator=( const this_type& rhs )
  {
    static_assert(std::is_copy_assignable<T>::value,"No matching copy assignment operator for type T");
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");

    if(rhs.m_is_initialized) {
      lazy_construct();
      assign(*rhs);
    } else {
      m_constructor = rhs.m_constructor;
    }
    m_destructor  = rhs.m_destructor;

    return (*this);
  }

  template<typename T>
  inline typename Lazy<T>::this_type& Lazy<T>::operator=( this_type&& rhs )
  {
    static_assert(std::is_move_assignable<T>::value,"No matching move assignment operator for type T");
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");

    if(rhs.m_is_initialized) {
      lazy_construct();
      assign(std::move(*rhs));
    } else {
      m_constructor = std::move(rhs.m_constructor);
      rhs.m_constructor = nullptr;
    }
    m_destructor = std::move(rhs.m_destructor);
    rhs.m_destructor = nullptr;

    return (*this);
  }

  template<typename T>
  inline typename Lazy<T>::value_type& Lazy<T>::operator=( const value_type& rhs )
  {
    static_assert(std::is_copy_assignable<T>::value,"No matching copy assignment operator for type T");

    lazy_construct();
    assign(rhs);

    return *ptr();
  }

  template<typename T>
  inline typename Lazy<T>::value_type& Lazy<T>::operator=( value_type&& rhs )
 {
    static_assert(std::is_move_assignable<T>::value,"No matching move assignment operator for type T");

    lazy_construct();
    assign(std::forward<value_type>(rhs));

    return *ptr();
  }

  //--------------------------------------------------------------------------
  // Casting
  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::operator reference()
    const
  {
    lazy_construct();
    return *ptr();
  }

  template<typename T>
  inline Lazy<T>::operator bool()
    const noexcept
  {
    return m_is_initialized;
  }

  //--------------------------------------------------------------------------
  // Operators
  //--------------------------------------------------------------------------

  template<typename T>
  inline void Lazy<T>::swap(Lazy<T>& rhs)
    noexcept
  {
    using std::swap; // for ADL

    swap(m_constructor,rhs.m_constructor);
    swap(m_destructor,rhs.m_destructor);
    swap(m_is_initialized,rhs.m_is_initialized);
    swap((*ptr()),(*rhs.ptr())); // Swap the values of the T types
  }

  template<typename T>
  inline bool Lazy<T>::is_initialized()
    const noexcept
  {
    return m_is_initialized;
  }

  template<typename T>
  inline typename Lazy<T>::pointer Lazy<T>::get()
    const
  {
    lazy_construct();
    return ptr();
  }

  template<typename T>
  inline typename Lazy<T>::reference Lazy<T>::operator*()
    const
  {
    lazy_construct();
    return *ptr();
  }

  template<typename T>
  inline typename Lazy<T>::pointer Lazy<T>::operator->()
    const
  {
    lazy_construct();
    return ptr();
  }

  //--------------------------------------------------------------------------
  // Private Static Member Functions
  //--------------------------------------------------------------------------

  template<typename T>
  inline void Lazy<T>::default_destructor(value_type&) noexcept{}

  //--------------------------------------------------------------------------
  // Private Constructors
  //--------------------------------------------------------------------------

  template<typename T>
  template<typename...Args>
  inline Lazy<T>::Lazy( ctor_va_args_tag, Args&&...args )
    : m_is_initialized(false),
      m_constructor([this,args...](){this->construct(ctor_va_args_tag(), std::move(args)...);}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_constructible<T,Args...>::value, "No matching constructor for type T with given arguments");
  }

  //--------------------------------------------------------------------------
  // Private Member Functions
  //--------------------------------------------------------------------------

  template<typename T>
  inline typename Lazy<T>::unqualified_pointer Lazy<T>::ptr()
    const noexcept
  {
    // address-of idiom (https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Address_Of)
    return reinterpret_cast<unqualified_pointer>(& const_cast<char&>(reinterpret_cast<const volatile char &>(m_storage)));
  }

  template<typename T>
  inline void Lazy<T>::lazy_construct( )
    const
  {
    if( !m_is_initialized )
    {
      m_constructor();
      m_is_initialized = true;
    }
  }

  template<typename T>
  inline void Lazy<T>::construct( const value_type& x )
    const
  {
    destruct();
    new (ptr()) value_type( x );
    m_is_initialized = true;
  }

  template<typename T>
  inline void Lazy<T>::construct( value_type&& x )
    const
  {
    destruct();
    new (ptr()) value_type( std::forward<value_type>(x) );
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args>
  inline void Lazy<T>::construct( ctor_va_args_tag, Args&&...args )
    const
  {
    destruct();
    new (ptr()) value_type( std::forward<Args>(args)... );
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args>
  inline void Lazy<T>::construct( const std::tuple<Args...>& args )
    const
  {
    destruct();
    tuple_construct(args,detail::index_sequence_for<Args...>());
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args, std::size_t...Ints>
  inline void Lazy<T>::tuple_construct(const std::tuple<Args...>& args,
                                       const detail::index_sequence<Ints...>& )
    const noexcept( std::is_nothrow_constructible<T,Args...>::value )
  {
    static_assert(std::is_constructible<T,Args...>::value,"No matching constructor for type T with given arguments");

    new (ptr()) T( std::get<Ints>(args)... );
  }

  template<typename T>
  inline void Lazy<T>::destruct( ) const
  {
    if( m_is_initialized )
    {
      if( m_destructor )
      {
        m_destructor(*ptr());
      }
      ptr()->~T();
      m_is_initialized = false;
    }
  }

  template<typename T>
  inline void Lazy<T>::assign( const value_type& rhs )
    const noexcept( std::is_nothrow_copy_assignable<T>::value )
  {
    (*ptr()) = rhs;
  }

  template<typename T>
  inline void Lazy<T>::assign( value_type&& rhs )
    const noexcept( std::is_nothrow_move_assignable<T>::value )
  {
    (*ptr()) = std::forward<value_type>(rhs);
  }

  //--------------------------------------------------------------------------
  // Utilities
  //--------------------------------------------------------------------------

  template<typename T, typename...Args>
  Lazy<T> make_lazy(Args&&...args)
  {
    return Lazy<T>(typename Lazy<T>::ctor_va_args_tag(), std::forward<Args>(args)...);
  }

  template<typename T>
  void swap(Lazy<T>& lhs, Lazy<T>& rhs) noexcept
  {
    lhs.swap(rhs);
  }

} // namespace lazy

#endif /* LAZYLAZY_HPP_ */
