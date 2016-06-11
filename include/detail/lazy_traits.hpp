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
    /// This is primarily used for composition in \c function_traits
    ///
    /// The result is aliased as \c ::type
    template<size_t n, typename...Args>
    struct arg_tuple
      : public identity<
          typename std::tuple_element<n, std::tuple<Args...>
        >::type>
    {
      static_assert(n>=sizeof...(Args),"Index out of range");
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
    struct function_traits_impl<Ret(Args...)>{
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret result_type;                    /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

    template <typename Ret, typename...Args>
    struct function_traits_impl<Ret(*)(Args...)>{
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret result_type;                         /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

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
    struct member_function_traits_impl<Ret(C::*)(Args...)>{
      /// The number of arguments.
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret      result_type;                    /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

    template <typename C, typename Ret, typename... Args>
    struct member_function_traits_impl<Ret(C::*)(Args...) const>{
      /// The number of arguments.
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret result_type;                         /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

    template <typename C, typename Ret, typename... Args>
    struct member_function_traits_impl<Ret(C::*)(Args...) volatile>{
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret result_type;                         /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

    template <typename C, typename Ret, typename... Args>
    struct member_function_traits_impl<Ret(C::*)(Args...) const volatile>{
      static constexpr size_t arity = sizeof...(Args); /// Number of arguments
      typedef Ret result_type;                         /// Return type

      template<size_t n>
      using arg = arg_tuple<n,Args...>;
    };

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

    //------------------------------------------------------------------------

    /// \brief type-trait to determine if a type provided is a \c std::tuple
    ///
    /// \note this also detects \c std::pair
    ///
    /// The result is aliased as \c ::value
    template<typename T>
    struct is_tuple : public std::false_type{};

    template<typename...Args>
    struct is_tuple<std::tuple<Args...> > : public std::true_type{};

    template<typename...Args>
    struct is_tuple<std::pair<Args...> > : public std::true_type{};

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

  } // namespace detail
} // namespace lazy

#endif /* LAZY_DETAIL_LAZY_TRAITS_HPP_ */
