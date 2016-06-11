/**
 * \file lazy_traits.hpp
 *
 * \brief This file contains a collection of helpful type traits to build the
 *        \c Lazy class.
 *
 * \author Matthew Rodusek (matthew.rodusek@gmail.com)
 * \version 1.0
 */

#ifndef LAZY_DETAIL_LAZY_TRAITS_HPP_
#define LAZY_DETAIL_LAZY_TRAITS_HPP_

#include <type_traits>
#include <utility>

namespace lazy{
  namespace detail{

    /// \brief Type-trait for identities (always defines \c T as \c type)
    template<typename T>
    struct identity{
      typedef T type;
    };

    /// \brief Implementation helper for the \c void_t idiom
    template<typename...Args>
    struct voidify : public identity<void>{};

    /// \brief Implementation of the \c void_t idiom
    template<typename...Args>
    using void_t = typename voidify<Args...>::type;

    template<size_t n, typename...Args>
    struct arg_tuple
      : public identity<
          typename std::tuple_element<n, std::tuple<Args...>
        >::type>
    {
      static_assert(n>=sizeof...(Args),"Index out of range");
    };

    /// \brief Type traits for retrieving various parts of a function
    ///
    ///
    template<typename T>
    struct function_traits_impl{};

    template <typename Ret, typename...Args>
    struct function_traits_impl<Ret(Args...)>
    {
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret result_type;                    /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

    template <typename Ret, typename...Args>
    struct function_traits_impl<Ret(*)(Args...)>
    {
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret result_type;                         /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

    /// \brief Type traits for retrieving various parts of a member function
    ///
    ///
    template<typename T>
    struct member_function_traits_impl{};

    template <typename C, typename Ret, typename... Args>
    struct member_function_traits_impl<Ret(C::*)(Args...)>
    {
      /// The number of arguments.
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret      result_type;                    /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

    template <typename C, typename Ret, typename... Args>
    struct member_function_traits_impl<Ret(C::*)(Args...) const>
    {
      /// The number of arguments.
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret result_type;                         /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

    /// \brief Type trait for retrieving various parts of a functor
    ///
    ///
    template<typename T>
    struct functor_traits_impl : public member_function_traits_impl<decltype(&T::operator())>{};

    /// \brief Type traits to retrieve the various parts of a callable
    ///        function-like object
    ///
    /// The number of args is aliased as \c ::arity
    /// The return type is aliased as \c ::result_type
    /// Arguments are aliased as \c ::arg<n>::type
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

    /// \brief type-trait to determine if a type provided is a \c std::tuple
    ///
    /// \note this also detects \c std::pair
    template<typename T>
    struct is_tuple : public std::false_type{};

    template<typename...Args>
    struct is_tuple<std::tuple<Args...> > : public std::true_type{};

    template<typename...Args>
    struct is_tuple<std::pair<Args...> > : public std::true_type{};

  }
}

#endif /* LAZY_DETAIL_LAZY_TRAITS_HPP_ */
