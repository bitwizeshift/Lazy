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

#ifndef LAZY_LAZY_HPP
#define LAZY_LAZY_HPP

#include "utility.hpp"
#include "detail/lazy_traits.hpp"

#include <initializer_list> // std::initializer_list
#include <utility>          // std::move
#include <functional>       // std::function

namespace lazy {

  //////////////////////////////////////////////////////////////////////////
  /// \brief Lazy class used for lazy-loading any type
  ///
  /// The stored lazy-loaded class, \c T, will always be instantiated
  /// before being accessed, and destructed when put out of scope.
  ///
  /// \tparam T the type contained within this \c Lazy
  //////////////////////////////////////////////////////////////////////////
  template<typename T>
  class Lazy
  {
    //----------------------------------------------------------------------
    // Public Member Types
    //----------------------------------------------------------------------
  public:

    using value_type = T; ///< The type to lazily construct

    //----------------------------------------------------------------------
    // Constructors
    //----------------------------------------------------------------------
  public:

    /// \brief Constructs a lazy type that will be default-constructed
    Lazy() noexcept;

    //----------------------------------------------------------------------

    /// \brief Copy-constructs a lazy type
    ///
    /// If the lazy being copied was already initialized, this too will
    /// be initialized. Otherwise this will be uninitialized
    ///
    /// \param other the lazy being copied
    Lazy( const Lazy& other );

    /// \brief Move-constructs a lazy type
    ///
    /// If the lazy being moved was already initialized, this too will
    /// be initialized. Otherwise this will be uninitialized
    ///
    /// \param other the lazy being moved
    Lazy( Lazy&& other );

    //----------------------------------------------------------------------

    /// \brief Copy-constructs a lazy type from a convertible lazy
    ///
    /// If the lazy being copied was already initialized, this too will
    /// be initialized. Otherwise this will be uninitialized
    ///
    /// \note This constructor is explicit if const U& is convertible to T
    ///
    /// \param other the lazy to copy
#ifndef BIT_DOXYGEN_BUILD
    template<typename U, std::enable_if_t<detail::lazy_is_enabled_copy_ctor<T,U>::value && !std::is_convertible<const U&, T>::value>* = nullptr>
#else
    template<typename U>
#endif
    Lazy( const Lazy<U>& other );

    /// \copydoc Lazy( const Lazy<U>& )
#ifndef BIT_DOXYGEN_BUILD
    template<typename U, std::enable_if_t<detail::lazy_is_enabled_copy_ctor<T,U>::value && std::is_convertible<const U&, T>::value>* = nullptr>
#else
    template<typename U>
#endif
    explicit Lazy( const Lazy<U>& other );

    //----------------------------------------------------------------------

    /// \brief Move-constructs a lazy type from a convertible lazy
    ///
    /// If the lazy being moved was already initialized, this too will
    /// be initialized. Otherwise this will be uninitialized
    ///
    /// \note This constructor is explicit if U&& is convertible to T
    ///
    /// \param other the lazy to copy
#ifndef BIT_DOXYGEN_BUILD
    template<typename U, std::enable_if_t<detail::lazy_is_enabled_move_ctor<T,U>::value && !std::is_convertible<U&&, T>::value>* = nullptr>
#else
    template<typename U>
#endif
    Lazy( Lazy<U>&& other );

    /// \copydoc Lazy( Lazy<U>&& )
#ifndef BIT_DOXYGEN_BUILD
    template<typename U, std::enable_if_t<detail::lazy_is_enabled_move_ctor<T,U>::value && std::is_convertible<U&&, T>::value>* = nullptr>
#else
    template<typename U>
#endif
    explicit Lazy( Lazy<U>&& other );

    //----------------------------------------------------------------------

    /// \brief Constructs an uninitialized lazy that will be constructed
    ///        with the arguments specified in \p args...
    ///
    /// \note The argument types must be move-constructible if passed by
    ///       rvalue, or copy-constructible if passed by lvalue
    ///
    /// \param args the arguments to use for deferred construction
#ifndef BIT_DOXYGEN_BUILD
    template<typename... Args, typename = std::enable_if_t<std::is_constructible<T,Args...>::value>>
#else
    template<typename... Args>
#endif
    explicit Lazy( in_place_t, Args&&... args );

    /// \brief Constructs an uninitialized lazy that will be constructed
    ///        with the arguments specified in \p ilist and \p args...
    ///
    /// \param ilist the initializer list to forward for deferred construction
    /// \param args the arguments to use for deferred construction
#ifndef BIT_DOXYGEN_BUILD
    template<typename U, typename... Args, typename = std::enable_if_t<std::is_constructible<T,std::initializer_list<U>,Args...>::value>>
#else
    template<typename U, typename... Args>
#endif
    explicit Lazy( in_place_t,
                    std::initializer_list<U> ilist,
                    Args&&... args );

    //----------------------------------------------------------------------

    /// \brief Constructs this lazy by deferring construction of the underlying
    ///        type as if by direct initializing by type U
    ///
    /// \param value the value to use to use to initialzie the lazy
#ifndef BIT_DOXYGEN_BUILD
    template<typename U = T, std::enable_if_t<detail::lazy_is_direct_initializable<T,U>::value && !detail::is_callable<U&>::value && std::is_convertible<U&&, T>::value>* = nullptr>
#else
    template<typename U>
#endif
    Lazy( U&& value );

    /// \copydoc Lazy( U&& )
#ifndef BIT_DOXYGEN_BUILD
    template<typename U = T, std::enable_if_t<detail::lazy_is_direct_initializable<T,U>::value && !detail::is_callable<U&>::value && !std::is_convertible<U&&, T>::value>* = nullptr>
#else
    template<typename U>
#endif
    explicit Lazy( U&& value );

    //----------------------------------------------------------------------

    /// \brief Destructs this lazy, and the contents of the lazy
    ~Lazy();

    //----------------------------------------------------------------------
    // Assignment
    //----------------------------------------------------------------------
  public:

    /// \brief Copy-assigns a lazy from another lazy
    ///
    /// If this lazy is currently initialized, \p other is copy-assigned.
    ///
    /// If both this and the other lazy is not initialized, the construction
    /// is deferred.
    ///
    /// \param other the other lazy to copy
    Lazy& operator=( const Lazy& other );

    /// \brief Move-assigns a lazy from another lazy
    ///
    /// If this lazy is currently initialized, \p other is move-assigned.
    ///
    /// If both this and the other lazy is not initialized, the construction
    /// is deferred.
    ///
    /// \param other the other lazy to copy
    Lazy& operator=( Lazy&& other );

    /// \brief Copy-assigns a lazy from a convertible lazy type
    ///
    /// If this lazy is currently initialized, \p other is copy-assigned.
    ///
    /// If both this and the other lazy is not initialized, only other
    /// will be initialized. This is needed to construct the type T from
    /// the value of type U.
    ///
    /// \param other the other lazy to copy
#ifndef BIT_DOXYGEN_BUILD
    template<typename U, std::enable_if_t<detail::lazy_is_enabled_copy_assignment<T,U>::value>* = nullptr>
#else
    template<typename U>
#endif
    Lazy& operator=( const Lazy<U>& other );

    /// \brief Move-assigns a lazy from a convertible lazy type
    ///
    /// If this lazy is currently initialized, \p other is move-assigned.
    ///
    /// If both this and the other lazy is not initialized, only other
    /// will be initialized. This is needed to construct the type T from
    /// the value of type U.
    ///
    /// \param other the other lazy to copy
#ifndef BIT_DOXYGEN_BUILD
    template<typename U, std::enable_if_t<detail::lazy_is_enabled_move_assignment<T,U>::value>* = nullptr>
#else
    template<typename U>
#endif
    Lazy& operator=( Lazy<U>&& other );

    /// \brief Direct initializes a value using perfect-forwarding
    ///
    /// \param value the value to initialize this lazy with
#ifndef BIT_DOXYGEN_BUILD
    template<typename U, std::enable_if_t<detail::lazy_is_direct_init_assignable<T,U>::value>* = nullptr>
#else
    template<typename U>
#endif
    Lazy& operator=( U&& value );

    //----------------------------------------------------------------------
    // Modifiers
    //----------------------------------------------------------------------
  public:

    /// \brief Explicitly initializes this lazy if not initialized before
    void initialize() const;

    /// \brief Destroys any contained value, if one is contained
    ///
    /// The arguments to be used for construction are maintained for
    /// future invocations, and can be changed by a call to \c operator=
    void reset();

    /// \brief Swaps this with the \p other lazy
    ///
    /// \param other the lazy to swap with this
    void swap( Lazy& other );

    //----------------------------------------------------------------------
    // Observers
    //----------------------------------------------------------------------
  public:

    /// \brief Returns \c true if this lazy contains a value (is initialized)
    ///
    /// This is an alias of has_value
    explicit operator bool() const noexcept;

    /// \brief Determines whether this lazy contains a value (is initialized)
    ///
    /// \return \c true when initialized, \c false otherwise
    bool has_value() const noexcept;

    //----------------------------------------------------------------------

    /// \brief Returns a pointer to the underlying lazy type
    ///
    /// \note It is undefined behaviour to call this if has_value returns
    ///       \c false
    ///
    /// \return a pointer to the contained type
    value_type* operator->();

    /// \copydoc operator->()
    const value_type* operator->() const;

    /// \brief Returns a reference to the underlying lazy type
    ///
    /// \note It is undefined behaviour to call this if has_value returns
    ///       \c false
    ///
    /// \return a reference to the contained type
    value_type& operator*() &;

    /// \copydoc operator*() &
    const value_type& operator*() const &;

    /// \copydoc operator*() &
    value_type&& operator*() &&;

    /// \copydoc operator*() &
    const value_type&& operator*() const &&;

    //----------------------------------------------------------------------

    /// \brief Returns a reference to the contained value
    ///
    /// If the lazy is not initialized prior to calling this, it will be
    /// initialized here
    ///
    /// \return a reference to the contained value
    value_type& value() &;

    /// \copydoc value() &
    const value_type& value() const &;

    /// \copydoc value() &
    value_type&& value() &&;

    /// \copydoc value() &
    const value_type&& value() const &&;

    //----------------------------------------------------------------------

    /// \brief Returns the contained value, if initialized,
    ///        otherwise returns \p default_value
    ///
    /// \param default_value the value to return if not initialized
    /// \return the contained value if initialized, \p default_value otherwise
    template<typename U>
    value_type value_or( U&& default_value ) const &;

    /// \copydoc value_or( U&& default_value ) &&
    template<typename U>
    value_type value_or( U&& default_value ) &&;

    //----------------------------------------------------------------------
    // Private Member Types
    //----------------------------------------------------------------------
  private:

    using storage_type = std::aligned_storage_t<sizeof(T),alignof(T)>;

    struct ctor_tag{};

    //----------------------------------------------------------------------
    // Private Members
    //----------------------------------------------------------------------
  private:

    std::function<void(void* ptr)> m_ctor_function;  ///< The construction function
    mutable storage_type           m_storage;        ///< The storage type
    mutable bool                   m_is_initialized; ///< Is this lazy initialized

    //----------------------------------------------------------------------
    // Private Constructors
    //----------------------------------------------------------------------
  private:

    /// \brief Constructs a lazy given a construction function
    ///
    /// \note The construction function must return a std::tuple containing the
    ///       arguments to forward to the constructor.
    ///
    /// \param tag the tag used for tag dispatch
    /// \param ctor the function specifying arguments to pass to the constructor
    template<typename Ctor>
    explicit Lazy( ctor_tag tag, Ctor&& ctor );

    //----------------------------------------------------------------------
    // Private Member Functions
    //----------------------------------------------------------------------
  private:

    /// \brief Retrieves a pointer to the internal lazy target
    value_type* ptr() const;

    /// \brief Lazy-constructs the underlying type
    void lazy_construct() const;

    /// \brief Destroys the underlying type of the lazy
    void destruct();

    //----------------------------------------------------------------------
    // Friends
    //----------------------------------------------------------------------
  private:

    template<typename>
    friend class Lazy;

    template<typename U,typename Ctor>
    friend Lazy<U> make_lazy_generator( Ctor&& );

  };

  //------------------------------------------------------------------------
  // Utilities
  //------------------------------------------------------------------------

  /// \brief Retrieves a hash of the underlying lazy, instantiating it if
  ///        not already instantiated
  ///
  /// \param val the value to retrieve the has of
  /// \return the hash of the lazy
  template<typename T>
  std::size_t hash_value( const Lazy<T>& val );

  //------------------------------------------------------------------------

  /// \brief Makes a lazy type from the given arguments
  ///
  /// This is just a wrapper around Lazy<T>( in_place, args... );
  ///
  /// \param args the arguments to forward to T's constructor
  /// \return the lazy instance
  template<typename T, typename...Args>
  Lazy<T> make_lazy( Args&&...args );


  /// \brief Makes a lazy type from the given arguments
  ///
  /// This is just a wrapper around Lazy<T>( in_place, ilist, args... );
  ///
  /// \param ilist an initializer list to forward to T's constructor
  /// \param args the arguments to forward to T's constructor
  /// \return the lazy instance
  template<typename T, typename U, typename...Args>
  Lazy<T> make_lazy( std::initializer_list<U> ilist, Args&&...args );

  /// \brief Makes a lazy type that uses a generator function to generate
  ///        the arguments that will be forwarded to T's constructor
  ///
  /// \tparam T The type of the lazy to construct
  /// \param ctor A function that returns a tuple of arguments used to
  ///             forward to T's constructor
  /// \return The newly constructed lazy
  template<typename T, typename Ctor>
  Lazy<T> make_lazy_generator( Ctor&& ctor );

  //-----------------------------------------------------------------------

  /// \brief Swaps the contents of the lazy from \p lhs and \p rhs
  ///
  /// \param lhs the left lazy to swap
  /// \param rhs the right lazy to swap
  template<typename T>
  void swap( Lazy<T>& lhs, Lazy<T>& rhs );

  //------------------------------------------------------------------------
  // Comparisons
  //------------------------------------------------------------------------

  /// \brief Compares the left and right lazy elements together for equality,
  ///        using the underlying type's operator==
  ///
  /// \note This will instantiate the lazy if it was not previously
  ///       instantiated
  ///
  /// \param lhs the left argument
  /// \param rhs the right argument
  template<typename T>
  bool operator==( const Lazy<T>& lhs, const Lazy<T>& rhs );

  /// \copydoc operator==( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator==( const Lazy<T>& lhs, const T& rhs );

  /// \copydoc operator==( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator==( const T& lhs, const Lazy<T>& rhs );

  //------------------------------------------------------------------------

  /// \brief Compares the left and right lazy elements together for inequality,
  ///        using the underlying type's operator!=
  ///
  /// \note This will instantiate the lazy if it was not previously
  ///       instantiated
  ///
  /// \param lhs the left argument
  /// \param rhs the right argument
  template<typename T>
  bool operator!=( const Lazy<T>& lhs, const Lazy<T>& rhs );

  /// \copydoc operator!=( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator!=( const Lazy<T>& lhs, const T& rhs );

  /// \copydoc operator!=( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator!=( const T& lhs, const Lazy<T>& rhs );

  //------------------------------------------------------------------------

  /// \brief Determines if \p lhs < \p rhs, using the underlying type's
  ///        \c operator<
  ///
  /// \note This will instantiate the lazy if it was not previously
  ///       instantiated
  ///
  /// \param lhs the left argument
  /// \param rhs the right argument
  template<typename T>
  bool operator<( const Lazy<T>& lhs, const Lazy<T>& rhs );

  /// \copydoc operator<( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator<( const Lazy<T>& lhs, const T& rhs );

  /// \copydoc operator<( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator<( const T& lhs, const Lazy<T>& rhs );

  //------------------------------------------------------------------------

  /// \brief Determines if \p lhs <= \p rhs, using the underlying type's
  ///        \c operator<=
  ///
  /// \note This will instantiate the lazy if it was not previously
  ///       instantiated
  ///
  /// \param lhs the left argument
  /// \param rhs the right argument
  template<typename T>
  bool operator<=( const Lazy<T>& lhs, const Lazy<T>& rhs );

  /// \copydoc operator<=( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator<=( const Lazy<T>& lhs, const T& rhs );

  /// \copydoc operator<=( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator<=( const T& lhs, const Lazy<T>& rhs );

  //------------------------------------------------------------------------

  /// \brief Determines if \p lhs > \p rhs, using the underlying type's
  ///        \c operator>
  ///
  /// \note This will instantiate the lazy if it was not previously
  ///       instantiated
  ///
  /// \param lhs the left argument
  /// \param rhs the right argument
  template<typename T>
  bool operator>( const Lazy<T>& lhs, const Lazy<T>& rhs );

  /// \copydoc operator>( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator>( const Lazy<T>& lhs, const T& rhs );

  /// \copydoc operator>( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator>( const T& lhs, const Lazy<T>& rhs );

  //------------------------------------------------------------------------

  /// \brief Determines if \p lhs >= \p rhs, using the underlying type's
  ///        \c operator>=
  ///
  /// \note This will instantiate the lazy if it was not previously
  ///       instantiated
  ///
  /// \param lhs the left argument
  /// \param rhs the right argument
  template<typename T>
  bool operator>=( const Lazy<T>& lhs, const Lazy<T>& rhs );

  /// \copydoc operator>=( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator>=( const Lazy<T>& lhs, const T& rhs );

  /// \copydoc operator>=( const Lazy<T>&, const Lazy<T>& )
  template<typename T>
  bool operator>=( const T& lhs, const Lazy<T>& rhs );

} // namespace lazy

#include "detail/Lazy.inl"

#endif /* LAZYLAZY_HPP */
