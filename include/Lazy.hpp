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

#if __cplusplus < 201103L
# error This file requires compiler and library support for the ISO C++11 standard.
#endif

#include "detail/lazy_traits.hpp"

#include <type_traits>
#include <functional>
#include <tuple>

namespace lazy{

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

    using this_type = Lazy<T>;

    using value_type = T;
    using pointer    = T*;
    using reference  = T&;

    //------------------------------------------------------------------------
    // Construction / Destruction / Assignment
    //------------------------------------------------------------------------
  public:

    /// \brief Default constructor; no initialization takes place
    Lazy( ) noexcept;

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
    Lazy( const this_type& rhs );

    /// \brief Constructs a \c Lazy by moving another \c Lazy
    ///
    /// \note If \p rhs is initialized, then this moved version will
    ///       also be initialized
    ///
    /// \param rhs the \c Lazy to move
    Lazy( this_type&& rhs );

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
    operator reference() const;

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
    explicit Lazy( ctor_va_args_tag tag, Args&&...args ) noexcept( detail::are_nothrow_copy_constructible<Args...>::value );

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

} // namespace lazy

#include "detail/Lazy.inl"

#endif /* LAZYLAZY_HPP_ */
