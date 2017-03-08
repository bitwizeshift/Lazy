#ifndef LAZY_UTILITY_HPP
#define LAZY_UTILITY_HPP

#include <tuple>   // std::tuple
#include <utility> // std::move
#include <cstdlib> // std::size_t

namespace lazy {

  //--------------------------------------------------------------------------
  // Memory Management
  //--------------------------------------------------------------------------

  /// \brief Constructs an instance of type \p T with the given \p tuple
  ///        at the memory location specified in \p ptr.
  ///
  /// This forwards the arguments from the \p tuple to the constructor
  /// of T, as if by calling make_from_tuple
  ///
  /// \param ptr   The memory location to construct into
  /// \param tuple The tuple containing arguments to forward to T
  /// \return Pointer ot the initialzied memory (cast of \p ptr)
  template<typename T, typename Tuple>
  T* uninitialized_tuple_construct_at( void* ptr, Tuple&& tuple );

  /// \brief Calls the destructor of the object pointed to by p, as if by p->~T().
  ///
  /// \param p a pointer to the object to be destroyed
  template<typename T>
  void destroy_at( T* p );


  //--------------------------------------------------------------------------
  // In Place
  //--------------------------------------------------------------------------

  namespace detail {

    /// \brief Used to disambiguate unspecialized in_place
    struct in_place_ctor_tag{};

    /// \brief Used to disambiguate typed in-place
    template<typename T>
    struct in_place_ctor_type_tag{};

    /// \brief Used to disambiguate indexed in-place
    template<std::size_t I>
    struct in_place_ctor_index_tag{};

  } // namespace detail

  /// \brief in_place_tag is an empty struct type used as the return types
  ///        of the in_place functions for disambiguation.
  struct in_place_tag
  {
    in_place_tag() = delete;
    in_place_tag(int){}
  };

  /// \brief This function is a special disambiguation tag for variadic functions, used in
  ///        any and optional
  ///
  /// \note Calling this function results in undefined behaviour.
#ifndef DOXYGEN_BUILD
  inline in_place_tag in_place( detail::in_place_ctor_tag = detail::in_place_ctor_tag() ){ return {0}; }
#else
  in_place_tag in_place( /* implementation defined */ );
#endif

  /// \brief This function is a special disambiguation tag for variadic functions, used in
  ///        any and optional
  ///
  /// \note Calling this function results in undefined behaviour.
  template<typename T>
#ifndef DOXYGEN_BUILD
  inline in_place_tag in_place( detail::in_place_ctor_type_tag<T> = detail::in_place_ctor_type_tag<T>() ){ return {0}; }
#else
  in_place_Tag in_place( /* implementation defined */ );
#endif

  /// \brief This function is a special disambiguation tag for variadic functions, used in
  ///        any and optional
  ///
  /// \note Calling this function results in undefined behaviour.
  template<std::size_t I>
#ifndef DOXYGEN_BUILD
  inline in_place_tag in_place( detail::in_place_ctor_index_tag<I> = detail::in_place_ctor_index_tag<I>() ){ return {0}; }
#else
  in_place_tag in_place( /* implementation defined */ );
#endif

  /// \brief A tag type used for dispatching in_place calls
  using in_place_t = in_place_tag(&)( detail::in_place_ctor_tag );

  /// \brief A tag type used for type-based dispatching in_place calls
  template<typename T>
  using in_place_type_t = in_place_tag(&)( detail::in_place_ctor_type_tag<T> );

  /// \brief A tag type used for type-based dispatching in_place calls
  template <std::size_t I>
  using in_place_index_t = in_place_tag(&)( detail::in_place_ctor_index_tag<I> );

} // namespace lazy

#include "detail/utility.inl"

#endif
